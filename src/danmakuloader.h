#ifndef DANMAKULOADER_H
#define DANMAKULOADER_H

#include <QObject>
class QNetworkReply;
class QProcess;

class DanmakuLoader : public QObject
{
    Q_OBJECT
public:
    explicit DanmakuLoader(QObject *parent = 0);

public slots:
    void load(const QString &srcFile, int width, int height);

signals:
    void finished(const QString &assFile);

private slots:
    void reload(void);
    void onXmlDownloaded(void);
    void onProcessFinished(void);

private:
    QNetworkReply *reply;
    QProcess *process;
    QString xmlFile;
    QString outputFile;
    int width;
    int height;
};

#endif // DANMAKULOADER_H
