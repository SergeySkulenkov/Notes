#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTranslator translator;
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if(translator.load("qt_" + QLocale::system().name(), QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
            app.installTranslator(&translator);
    #else
        if(translator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
            app.installTranslator(&translator);
    #endif

    MainWindow window;
    window.resize(640, 512);
    window.show();
    return app.exec();
}

