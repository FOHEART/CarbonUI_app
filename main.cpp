#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QtCore/QFile>

#include <QtCore/QDebug>
#include <QtGui/QFontDatabase>
#include <QtGui/QFont>
#include <iostream>

#if defined(Q_OS_WIN)
#include <windows.h>
#include <cstdio>
#include <io.h>
#include <fcntl.h>
#endif

int main(int argc, char *argv[])
{
    /*高清屏适配*/
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication app(argc, argv);

#if defined(Q_OS_WIN)
    // Allocate a console window for the GUI application so stdout/stderr are visible
    if (AttachConsole(ATTACH_PARENT_PROCESS) == 0) {
        AllocConsole();
    }
    // Redirect C standard streams to the console
    FILE *fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
    // Ensure C++ streams are in sync with C I/O
    std::ios::sync_with_stdio(false);
#endif

    // Load application font from runtime 'fonts' directory and set it as the app-wide font
    {
       // const QString fontPath = QStringLiteral("fonts/HarmonyOS_Sans_SC_Regular.ttf");
		const QString fontPath = QStringLiteral("fonts/HarmonyOS_Sans_SC_Bold.ttf");

        int id = QFontDatabase::addApplicationFont(fontPath);
        if (id != -1) {
            QString family = QFontDatabase::applicationFontFamilies(id).value(0);
            if (!family.isEmpty()) {
                app.setFont(QFont(family));
                qDebug() << "Application font set to:" << family;
            } else {
                qDebug() << "Loaded font but no family returned for" << fontPath;
            }
        } else {
            qDebug() << "Failed to load font from" << fontPath;
        }
    }
    
#if 0
    {
        QFile f(":/styles/app.qss");
		if (!f.exists())
        {
            qDebug() << "exists:" << f.exists();
        }
    }

    
    // Load application stylesheet from resources
    QFile f(":/styles/app.qss");
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        QString style = QString::fromUtf8(f.readAll());
        app.setStyleSheet(style);
    }
#endif
    MainWindow window;
    window.show();
    return app.exec();
}
