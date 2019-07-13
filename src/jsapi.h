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
    Q_INVOKABLE void finish_parsing(const QJSValue &dict);
};

extern JSAPI apiObject;

#endif // JSAPI_H
