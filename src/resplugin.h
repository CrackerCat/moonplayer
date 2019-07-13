#ifndef RESPLUGIN_H
#define RESPLUGIN_H

#include "pluginbase.h"
#include <QStringList>

class ResPlugin: public PluginBase
{
    Q_OBJECT
public:
    ResPlugin(const QString &filename, bool *ok, QObject *parent = NULL);
    void explore(const QString &tag, const QString &region, int page);
    void search(const QString &key, int page);
    void loadItem(const QString &flag);
    inline QString &getName(){return name;}

    QStringList tagsList;
    QStringList regionsList;

private:
    QString name;
    QJSValue searchFunc;
    QJSValue exploreFunc;
    QJSValue loadItemFunc;
};

extern ResPlugin **resplugins;
extern int n_resplugins;
void initResPlugins(void);

#endif // RESPLUGIN_H
