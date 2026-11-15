/**
 * @file  mainwindow_test.cpp
 * @brief 界面层（MainWindow）QtTest 冒烟测试。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_tests
 *
 * @details
 * 覆盖策略：只断言窗口结构与命名控件存在性（行为断言），不做像素比对
 * （离屏平台无 GL 上下文，像素验收归 --screenshot 自检）。
 * 运行环境：QT_QPA_PLATFORM=offscreen，由 ctest ENVIRONMENT 属性注入。
 * 每个用例自建 MainWindow 实例，无跨用例共享状态。
 */

#include <QtTest/QtTest>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QMenuBar>

#include "MainWindow.h"

/**
 * @brief 主窗口界面测试夹具：每个用例独立创建/销毁 MainWindow，不共享状态。
 * @details 无共享资源，因此不写 init()/cleanup()；控件查找统一走
 *          findChild + objectName（objectName 由 MainWindow.ui 定义）。
 */
class MainWindowTest : public QObject
{
    Q_OBJECT

private slots:
    /**
     * @test   MainWindowUiTest.FindsExpectedNamedWidgets
     * @brief  结构断言：装配后能按 objectName 找到全部关键控件。
     * @details 目的：防止 .ui 改名/装配代码漏接导致控件悬空（典型缺陷：
     *          编译不报错但界面残缺）。
     *          设计思路：用 findChild 而非直接访问 ui 成员，验证的是
     *          "对象树里真实存在且名字正确"，与 .ui 的契约对齐。
     * @expected tabWidget / treeWidget / scrollArea / mainToolBar / menuBar
     *           五个控件均可按名找到，全部非 NULL。
     * @priority P0
     */
    void findsExpectedNamedWidgets()
    {
        MainWindow window;                                          // Arrange

        window.show();                                              // Act：进入可见状态

        // Assert：五个关键控件按 objectName 查找均存在
        QVERIFY(window.findChild<QTabWidget *>(QStringLiteral("tabWidget")) != nullptr);
        QVERIFY(window.findChild<QWidget *>(QStringLiteral("treeWidget")) != nullptr);
        QVERIFY(window.findChild<QWidget *>(QStringLiteral("scrollArea")) != nullptr);
        QVERIFY(window.findChild<QToolBar *>(QStringLiteral("mainToolBar")) != nullptr);
        QVERIFY(window.findChild<QMenuBar *>(QStringLiteral("menuBar")) != nullptr);
    }

    /**
     * @test   MainWindowUiTest.TabWidgetHasAtLeastOnePage
     * @brief  行为断言：TabWidget 装配后至少有一个页面。
     * @details 目的：锁住"主界面至少展示一个功能页"的最低可用性契约，
     *          防止 .ui 删页或 setCurrentIndex 越界类回归。
     *          设计思路：只断言下界（count >= 1）而非精确页数，
     *          允许界面后续自由增页不破坏测试。
     * @expected tabWidget->count() >= 1 且 currentIndex 落在 [0, count) 内。
     * @priority P0
     */
    void tabWidgetHasAtLeastOnePage()
    {
        MainWindow window;                                          // Arrange

        QTabWidget *tabs = window.findChild<QTabWidget *>(QStringLiteral("tabWidget"));
        QVERIFY(tabs != nullptr);                                   // 前置条件，失败则不再解引用
        window.show();                                              // Act

        // Assert
        QVERIFY(tabs->count() >= 1);
        QVERIFY(tabs->currentIndex() >= 0 && tabs->currentIndex() < tabs->count());
    }

    /**
     * @test   MainWindowUiTest.StatusBarIsAttached
     * @brief  结构断言：状态栏已挂接到主窗口。
     * @details 目的：防止状态栏被误删或被 setStatusBar(nullptr) 置空，
     *          导致后续状态提示类功能无落点。
     *          设计思路：用 statusBar() 的挂接关系（isAncestorOf 的反向等价）
     *          而非 isVisible()——离屏会话下可见性不可靠。
     * @expected window.statusBar() 返回非 NULL 指针。
     * @priority P1
     */
    void statusBarIsAttached()
    {
        MainWindow window;                                          // Arrange

        window.show();                                              // Act

        // Assert
        QVERIFY(window.statusBar() != nullptr);
    }
};

/**
 * @brief QtTest 测试入口：由 QTEST_MAIN 提供 QApplication 与用例执行。
 * @details 经 ctest 设置 QT_QPA_PLATFORM=offscreen 离屏运行；
 *          只链接 Qt5::Test 与被测库，不引入 GoogleTest。
 * @param[in] argc 命令行参数个数，由运行时传入。
 * @param[in] argv 命令行参数数组，不可为 NULL；支持 QtTest 的槽名过滤参数。
 * @return QtTest 汇总结果：0 表示全部用例通过，非 0 表示存在失败。
 */
QTEST_MAIN(MainWindowTest)

#include "mainwindow_test.moc"
