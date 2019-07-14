#ifndef PATHS_H
#define PATHS_H

#include <QString>

#if defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD)
#define PYTHON_PATH "python3"
#else
#define PYTHON_PATH "python"
#endif

QString getAppPath(void);
QString getUserPath(void);
void createUserPath(void);

// videos path
QString getVideosPath(void);
QString getPicturesPath(void);

// translation files
QString getQtTranslationsPath(void);

// Get FFmpeg's file path
QString ffmpegFilePath(void);

// parser upgrader
QString parserUpgraderPath(void);

#endif // PATHS_H
