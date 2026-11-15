/**
 * @file  main.cpp
 * @brief CarbonUI_app 程序入口：初始化高清屏适配、控制台重定向与应用字体，启动主窗口。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 *
 * @details
 * 启动顺序（顺序敏感）：
 *  1. 高清屏属性必须在 QApplication 构造前设置；
 *  2. Windows 下附加/分配控制台并重定向标准流，便于 CI 与调试输出；
 *  3. 通过 appcore::loadApplicationFont 加载 fonts/ 目录下的应用字体；
 *  4. 显示 MainWindow 并进入事件循环。
 *
 * 线程安全：全部逻辑在主线程执行。
 */

#include "MainWindow.h"
#include "AppFont.h"
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

/**
 * @brief 应用入口：完成 Qt 应用初始化后启动主窗口并进入事件循环。
 *
 * @details
 * 高清屏缩放属性必须在 QApplication 构造之前设置；Windows 分支为
 * WIN32_EXECUTABLE（无控制台）程序附加控制台，使 qDebug/stdout 可见。
 * 字体加载委托给 appcore::loadApplicationFont，失败时仅打印日志，
 * 应用仍以默认字体继续启动（降级不阻断）。
 *
 * @param[in] argc 命令行参数个数，取值 >= 0，由运行时传入。
 * @param[in] argv 命令行参数数组，不可为 NULL；元素个数由 @p argc 指定。
 *
 * @return 事件循环退出码，转交 main 的返回值；正常退出为
 *         QApplication::exec() 的返回值（通常为 0）。
 */
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
        const QString fontPath = QStringLiteral("fonts/HarmonyOS_Sans_SC_Bold.ttf");

        // 失败返回空串：应用降级为默认字体，不阻断启动
        const QString family = appcore::loadApplicationFont(fontPath);
        if (!family.isEmpty()) {
            app.setFont(QFont(family));
            qDebug() << "Application font set to:" << family;
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
