#ifndef PLUGINBASE_H
#define PLUGINBASE_H

#include <QJSEngine>

class PluginBase : public QJSEngine
{
    Q_OBJECT
public:
    PluginBase(const QString &filename, bool *ok, QObject *parent = NULL);
};

#endif // PLUGINBASE_H
