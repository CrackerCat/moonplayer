#include "jsapi.h"
#include <QVariant>
#include "parserwebcatch.h"
#include <QDebug>

JSAPI apiObject;

JSAPI::JSAPI(QObject *parent) : QObject(parent)
{

}


void JSAPI::finish_parsing(const QJSValue &dict)
{
#ifdef MP_ENABLE_WEBENGINE
    QVariantHash data = dict.toVariant().toHash();
    qDebug() << data;
    parser_webcatch->onParseFinished(data);
#endif
}
