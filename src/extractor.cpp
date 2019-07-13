#include "extractor.h"
#include "platform/paths.h"
#include <QDir>

Extractor **extractors = NULL;
int n_extractors = 0;
QStringList Extractor::supportedHosts;

void initExtractors()
{
    static Extractor *array[128];
    extractors = array;

    QDir pluginsDir = QDir(getUserPath() + "/plugins");
    QStringList list = pluginsDir.entryList(QDir::Files, QDir::Name);

    while (!list.isEmpty())
    {
        QString filename = list.takeFirst();
        if (filename.startsWith("ext_") && filename.endsWith(".js"))
        {
            bool ok = false;
            Extractor *extractor = new Extractor(pluginsDir.filePath(filename), &ok);
            if (ok)
            {
                array[n_extractors] = extractor;
                n_extractors++;
            }
            else
            {
                delete extractor;
                qDebug("[plugin] Fails to load: %s", filename.toUtf8().constData());
            }
        }
    }
}


Extractor::Extractor(const QString &filename, bool *ok, QObject *parent) :
    PluginBase(filename, ok, parent)
{
    // hosts, urlpatterns and parseFunc
    supportedHosts = globalObject().property("supported_hosts").toVariant().toStringList();
    urlPattern = QRegularExpression(globalObject().property("url_pattern").toString(),
                                    QRegularExpression::DotMatchesEverythingOption);
    parseFunc = globalObject().property("parse");

    // ok
    *ok = true;
}


bool Extractor::match(const QString &url)
{
    QRegularExpressionMatch match = urlPattern.match(url);
    return match.hasMatch();
}

Extractor *Extractor::getMatchedExtractor(const QString &url)
{
    for (int i = 0; i < n_extractors; i++)
    {
        if (extractors[i]->match(url))
            return extractors[i];
    }
    return NULL;
}


QString Extractor::parse(const QByteArray &data)
{
    QJSValueList args;
    args << QString::fromUtf8(data);
    QJSValue result = parseFunc.call(args);
    if (result.isError())
        return "JS Error: " + result.toString() + "\n\nResponse Content: " + QString::fromUtf8(data);
    return QString();
}

bool Extractor::isSupported(const QString &host)
{
    return supportedHosts.contains(host);
}
