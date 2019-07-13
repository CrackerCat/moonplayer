#ifndef JSAPI_H
#define JSAPI_H

#include <QObject>
#include <QJSEngine>
#include <QJSValue>

class JSAPI : public QObject
{
    Q_OBJECT
public:
    explicit JSAPI(QObject *parent = nullptr);

    Q_INVOKABLE void get_content(const QString &url, const QJSValue &callbackFunc);
    Q_INVOKABLE void post_content(const QString &url, const QByteArray &postData, const QJSValue &callbackFunc);
    Q_INVOKABLE void warning(const QString &msg);
    Q_INVOKABLE bool question(const QString &msg);
    Q_INVOKABLE void show_resources(const QJSValue &list);
    Q_INVOKABLE void show_detail(const QJSValue &detail);
    Q_INVOKABLE void finish_parsing(const QJSValue &dict);
};

extern JSAPI apiObject;

#endif // JSAPI_H
