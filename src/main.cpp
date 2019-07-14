#include <QApplication>
#include <QTranslator>
#include "settingsdialog.h"
#include "accessmanager.h"
#include <locale.h>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include "platform/application.h"
#include "platform/detectopengl.h"
#include "platform/paths.h"
#include "playerview.h"
#include "parserykdl.h"
#include "parseryoutubedl.h"

#ifdef MP_ENABLE_WEBENGINE
#include "parserwebcatch.h"
#endif

#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "19260");

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    detectOpenGLEarly();

    Application a(argc, argv);
    if (!a.parseArgs())
        return 0;

    detectOpenGLLate();


    // Print python error to console on Windows
#ifdef Q_OS_WIN
    qputenv("PYTHONHOME", getAppPath().toUtf8());
    qputenv("PYTHONPATH", getAppPath().toUtf8() + "/Lib;" + getAppPath().toUtf8() + "/DLLs");
    bool win_debug = AttachConsole(ATTACH_PARENT_PROCESS);
    if (win_debug)
    {
        freopen("CON", "w", stdout);
        freopen("CON", "w", stderr);
        freopen("CON", "r", stdin);
    }
#endif

    //for mpv
    setlocale(LC_NUMERIC, "C");

    //init
    access_manager = new NetworkAccessManager(&a);
    printf("Initialize settings...\n");
    initSettings();

    // Translate moonplayer
    printf("Initialize language support...\n");
    QTranslator qtTranslator;
    if (qtTranslator.load("qt_" + QLocale::system().name(), getQtTranslationsPath()))
        a.installTranslator(&qtTranslator);

    QTranslator translator;
    if (translator.load("moonplayer_" + QLocale::system().name(), getAppPath() + "/translations"))
        a.installTranslator(&translator);

    // Create window
    PlayerView *player_view = new PlayerView;
    player_view->show();

    // Create video parsers
    parser_ykdl = new ParserYkdl(&a);
    parser_youtubedl = new ParserYoutubeDL(&a);
#ifdef MP_ENABLE_WEBENGINE
    parser_webcatch = new ParserWebCatch(&a);
#endif

    a.exec();
    delete player_view;
    return 0;
}
