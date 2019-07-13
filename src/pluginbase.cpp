#include "pluginbase.h"
#include <QFile>
#include "jsapi.h"

PluginBase::PluginBase(const QString &filename, bool *ok, QObject *parent) :
    QJSEngine(parent)
{
    // install api
    QJSValue api = newQObject(&apiObject);
    globalObject().setProperty("moonplayer", api);
    installExtensions(QJSEngine::ConsoleExtension);

    // open file
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        *ok = false;
        return;
    }

    // evaluate
    QString content = QString::fromUtf8(file.readAll());
    file.close();
    QJSValue result = evaluate(content, filename);
    if (result.isError())
    {
        qDebug("JS Error: %s", result.toString().toUtf8().constData());
        *ok = false;
        return;
    }
}
