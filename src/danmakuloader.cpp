#include "danmakuloader.h"
#include "accessmanager.h"
#include "settings_danmaku.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QTimer>
#include "platform/paths.h"

DanmakuLoader::DanmakuLoader(QObject *parent) : QObject(parent)
{
    reply = NULL;
    process = new QProcess(this);
    connect(process, SIGNAL(finished(int)), this, SLOT(onProcessFinished()));
}

void DanmakuLoader::reload()
{
    load(xmlFile, width, height);
}

void DanmakuLoader::load(const QString &xmlFile, int width, int height)
{
    this->xmlFile = xmlFile;
    if (height > QApplication::desktop()->height())
    {
        this->height = QApplication::desktop()->height();
        this->width = width * QApplication::desktop()->height() / height;
    }
    else if (width > QApplication::desktop()->width())
    {
        this->height = height * QApplication::desktop()->width() / width;
        this->width = QApplication::desktop()->width();
    }
    else
    {
        this->width = width;
        this->height = height;
    }
    if (reply) //another task is running
    {
        reply->abort();
        QTimer::singleShot(0, this, SLOT(reload())); //after event loop
        return;
    }
    reply = access_manager->get(QNetworkRequest(xmlFile));
    connect(reply, &QNetworkReply::finished, this, &DanmakuLoader::onXmlDownloaded);
}



void DanmakuLoader::onXmlDownloaded()
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QStringList args;
        args << getAppPath() + "/plugins/danmaku2ass_py3.py";

        // Output file
        outputFile = QDir::temp().filePath("moonplayer_danmaku.ass").toUtf8();
        args << "-o" << outputFile;

        // Size
        args << "-s" << QString().sprintf("%dx%d", width, height);

        // Font
#ifdef Q_OS_MAC
        args << "-fn" << (Settings::danmakuFont.isEmpty() ? "PingFang SC" : Settings::danmakuFont.toUtf8());
#else
        args << "-fn" << (Settings::danmakuFont.isEmpty() ? "sans-serif" : Settings::danmakuFont.toUtf8());
#endif

        // Font size
        args << "-fs";
        if (Settings::danmakuSize)
            args << QString::number(Settings::danmakuSize);
        else
        {
            if (width > 960)
                args << "36";
            else if (width > 640)
                args << "32";
            else
                args << "28";
        }

        // Duration of comment display
        args << "-dm";
        if (Settings::durationScrolling)
            args << QString::number(Settings::durationScrolling);
        else
        {
            if (width > 960)
                args << "10";
            else if (width > 640)
                args << "8";
            else
                args << "6";
        }

        // Duration of still danmaku
        args << "-ds" << QString::number(Settings::durationStill);

        // text opacity
        args << "-a" << QString::number(Settings::danmakuAlpha);

        // input
        args << "/dev/stdin";

        // run
        process->start(PYTHON_PATH, args);
        process->waitForStarted(-1);
        process->write(reply->readAll());
        process->closeWriteChannel();
    }
    reply->deleteLater();
    reply = NULL;
}

void DanmakuLoader::onProcessFinished()
{
    qDebug("%s", process->readAllStandardError().constData());
    emit finished(outputFile);
}
