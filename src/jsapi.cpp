#include "jsapi.h"
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QVariant>
#include "accessmanager.h"
#include "parserwebcatch.h"
#include "reslibrary.h"

JSAPI apiObject;

JSAPI::JSAPI(QObject *parent) : QObject(parent)
{

}


// get_content() and post_content
static void get_post_content(const QString &url, const QByteArray &postData, const QJSValue &callbackFunc)
{
    QNetworkRequest request = QNetworkRequest(url);
    QNetworkReply *reply;
    if (postData.isEmpty())
        reply = access_manager->get(request);
    else
    {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        reply = access_manager->post(request, postData);
    }

    QObject::connect(reply, &QNetworkReply::finished, [reply, postData, callbackFunc](){
        reply->deleteLater();
        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // redirection
        if (status == 301 || status == 302)
        {
            QByteArray final_url = reply->rawHeader("Location");
            get_post_content(QString::fromUtf8(final_url), postData, callbackFunc);
            return;
        }

        // Error
        if (reply->error() != QNetworkReply::NoError)
        {
            int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            QString errStr = QString().sprintf("Network Error: %d\n%s\n", status, reply->errorString().toUtf8().constData());
            QMessageBox::warning(NULL, "Error", errStr);
            return;
        }

        // Call callback function
        QJSValueList args;
        args << QString::fromUtf8(reply->readAll());
        QJSValue func = callbackFunc;
        func.call(args);
    });
}

void JSAPI::get_content(const QString &url, const QJSValue &callbackFunc)
{
    get_post_content(url, QByteArray(), callbackFunc);
}

void JSAPI::post_content(const QString &url, const QByteArray &postData, const QJSValue &callbackFunc)
{
    get_post_content(url, postData, callbackFunc);
}

// Dialog functions
void JSAPI::warning(const QString &msg)
{
    QMessageBox::warning(NULL, "Warning", msg);
}

bool JSAPI::question(const QString &msg)
{
    return QMessageBox::question(NULL, "question", msg, QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes;
}


// Show resource list
void JSAPI::show_resources(const QJSValue &list)
{
    res_library->clearItem();
    QVariantList resList = list.toVariant().toList();
    foreach (QVariant item, resList) {
        QVariantHash dict = item.toHash();
        res_library->addItem(dict["title"].toString(), dict["url"].toString(), dict["image_url"].toString());
    }
}


// Show detail view
void JSAPI::show_detail(const QJSValue &detail)
{
    res_library->openDetailPage(detail.toVariant().toHash());
}


// Finish video parsing
void JSAPI::finish_parsing(const QJSValue &dict)
{
#ifdef MP_ENABLE_WEBENGINE
    QVariantHash data = dict.toVariant().toHash();
    qDebug() << data;
    parser_webcatch->onParseFinished(data);
#endif
}
