#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include "pluginbase.h"
#include <QJSValue>
#include <QByteArray>
#include <QRegularExpression>

class Extractor : public PluginBase
{
    Q_OBJECT
public:
    Extractor(const QString &filename, bool *ok, QObject *parent = NULL);

    // Parse the catched data, return error string
    QString parse(const QByteArray &data);

    // Check if the catched data with url can be processed
    bool match(const QString &url);

    // Check if the website with host can be extracted
    static bool isSupported(const QString &host);

    // Get matched extractor
    static Extractor *getMatchedExtractor(const QString &url);

private:
    QJSValue parseFunc;
    QRegularExpression urlPattern;
    static QStringList supportedHosts;
};

void initExtractors(void);

extern Extractor **extractors;
extern int n_extractors;

#endif // EXTRACTOR_H
