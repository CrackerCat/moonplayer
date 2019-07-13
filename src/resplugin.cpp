#include "resplugin.h"
#include <QDir>
#include "platform/paths.h"

/************************
 ** Initialize plugins **
 ************************/
int n_resplugins = 0;
ResPlugin **resplugins = NULL;

void initResPlugins()
{
    static ResPlugin *array[128];
    resplugins = array;

    QDir pluginsDir = QDir(getUserPath() + "/plugins");
    QStringList list = pluginsDir.entryList(QDir::Files, QDir::Name);

    while (!list.isEmpty())
    {
        QString filename = list.takeFirst();
        if (filename.startsWith("res_") && filename.endsWith(".js"))
        {
            bool ok = false;
            ResPlugin *plugin = new ResPlugin(pluginsDir.filePath(filename), &ok);
            if (ok)
            {
                array[n_resplugins] = plugin;
                n_resplugins++;
            }
            else
            {
                delete plugin;
                qDebug("[plugin] Fails to load: %s", filename.toUtf8().constData());
            }
        }
    }
}


ResPlugin::ResPlugin(const QString &filename, bool *ok, QObject *parent) :
    PluginBase(filename, ok, parent)
{
    //get name
    name = globalObject().property("res_name").toString();
    if (name.isEmpty())
        name = filename.section("res_", -1).replace(".js", "");

    //get search() and load_item()
    searchFunc = globalObject().property("search");
    exploreFunc = globalObject().property("explore");
    loadItemFunc = globalObject().property("load_item");

    //get tags and regions
    tagsList = globalObject().property("tags").toVariant().toStringList();
    regionsList = globalObject().property("regions").toVariant().toStringList();

    *ok = true;
}


void ResPlugin::explore(const QString &tag, const QString &region, int page)
{
    QJSValueList args;
    args << tag << region << page;
    QJSValue retVal = exploreFunc.call(args);
    if (retVal.isError())
        qDebug("JS Error:\n    In line %i:\n    %s", retVal.property("lineNumber").toInt(), retVal.toString().toUtf8().constData());
}

void ResPlugin::search(const QString &key, int page)
{
    QJSValueList args;
    args << key << page;
    QJSValue retVal = searchFunc.call(args);
    if (retVal.isError())
        qDebug("JS Error:\n    In line %i:\n    %s", retVal.property("lineNumber").toInt(), retVal.toString().toUtf8().constData());
}

void ResPlugin::loadItem(const QString &flag)
{
    QJSValueList args;
    args << flag;
    QJSValue retVal = loadItemFunc.call(args);
    if (retVal.isError())
        qDebug("JS Error:\n    In line %i:\n    %s", retVal.property("lineNumber").toInt(), retVal.toString().toUtf8().constData());
}
