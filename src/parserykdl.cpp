#include "parserykdl.h"
#include "platform/paths.h"
#include "settings_network.h"
#include <QDir>
#include <QGridLayout>
#include <QLabel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QProcess>

ParserYkdl *parser_ykdl;

ParserYkdl::ParserYkdl(QObject *parent) : ParserBase(parent)
{
    process = new QProcess(this);
    connect(process, SIGNAL(finished(int)),this, SLOT(parseOutput()));
    msgWindow = NULL;
}

ParserYkdl::~ParserYkdl()
{
    if (process->state() == QProcess::Running)
    {
        process->kill();
        process->waitForFinished();
    }
}

bool ParserYkdl::isSupported(const QString &host)
{
    QDir extractorsDir(getUserPath() + "/ykdl/ykdl/extractors");
    QStringList extractors = extractorsDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
    QString shortName = host.section('.', -2, -2);
    if (shortName == "com" || shortName == "net" || shortName == "org")
        shortName = host.section('.', -3, -3);
    return extractors.contains(shortName) || extractors.contains(shortName + ".py");
}


void ParserYkdl::runParser(const QString &url)
{
    if (process->state() == QProcess::Running)
    {
        QMessageBox::warning(NULL, "Error", tr("Another file is being parsed."));
        return;
    }
    if (msgWindow == NULL)
    {
        msgWindow = new QWidget;
        msgWindow->setWindowFlags(msgWindow->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        QLabel *label = new QLabel(tr("Parsing with Ykdl..."));
        QGridLayout *layout = new QGridLayout(msgWindow);
        layout->addWidget(label);
    }

    QStringList args;
    args << (getAppPath() + "/plugins/ykdl_patched.py");
    args << "-t" << "15" << "--json";
    if (!Settings::proxy.isEmpty() &&
            (Settings::proxyType == "http" || (Settings::proxyType == "http_unblockcn")))
        args << "--proxy" << (Settings::proxy + ':' + QString::number(Settings::port));
    args << url;
    process->start(PYTHON_PATH, args, QProcess::ReadOnly);
    msgWindow->show();
}


void ParserYkdl::parseOutput()
{
    msgWindow->close();
    QByteArray output = process->readAllStandardOutput();
    QJsonParseError json_error;
    QJsonObject obj = QJsonDocument::fromJson(output, &json_error).object();
    if (json_error.error != QJsonParseError::NoError)
    {
        showErrorDialog(QString::fromUtf8(process->readAllStandardError()));
        return;
    }
    if (obj.contains("streams"))
    {
        result.title = obj["title"].toString();
        QJsonObject streams = obj["streams"].toObject();
        QJsonObject selectedItem;
        QJsonObject::const_iterator i;

        // Select video quality
        // get all available qualities
        QStringList items;
        for (i = streams.constBegin(); i != streams.constEnd(); i++)
        {
            QString profile = i.value().toObject()["video_profile"].toString();
            items << QString("%1 (%2)").arg(i.key(), profile);
        }

        // show dialog
        int index = selectQuality(items);
        if (index == -1)
            return;
        QString selected = items[index].section(" (", 0, 0);
        selectedItem = streams[selected].toObject();

        // Write names-urls-list
        QJsonArray json_urls = selectedItem["src"].toArray();
        result.container = selectedItem["container"].toString();
        result.referer = obj["extra"].toObject()["referer"].toString();
        result.ua = obj["extra"].toObject()["ua"].toString();
        result.danmaku_url = obj["danmaku_url"].toString();

        if (json_urls.size() == 0)
        {
            showErrorDialog(QString::fromUtf8(process->readAllStandardError()));
            return;
        }

        // Make urls list
        for (int i = 0; i < json_urls.size(); i++)
            result.urls << json_urls[i].toString();
        finishParsing();
    }
    else
        showErrorDialog(QString::fromUtf8(process->readAllStandardError()));
}


