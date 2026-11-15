/**
 * @file  mainwindow_test.cpp
 * @brief 界面层（MainWindow）QtTest 冒烟测试。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_tests
 *
 * @details
 * 覆盖策略：只断言窗口结构与命名控件存在性、语言切换行为（行为断言），
 * 不做像素比对（离屏平台无 GL 上下文，像素验收归 --screenshot 自检）。
 * 运行环境：QT_QPA_PLATFORM=offscreen，由 ctest ENVIRONMENT 属性注入。
 * 每个用例自建 MainWindow 实例，无跨用例共享状态。
 */

#include <QtTest/QtTest>
#include <QtWidgets/QAction>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QLabel>

#include "MainWindow.h"

/**
 * @brief 主窗口界面测试夹具：每个用例独立创建/销毁 MainWindow，不共享状态。
 * @details 无共享资源，因此不写 init()/cleanup()；控件查找统一走
 *          findChild + objectName（objectName 由 MainWindow.ui 定义）。
 */
class MainWindowTest : public QObject {
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
	 *           五个控件均可按名找到，全部非 NULL；语言菜单动作
	 *           actionLangChinese / actionLangEnglish 亦存在且可勾选。
	 * @priority P0
	 */
	void findsExpectedNamedWidgets() {
		MainWindow window; // Arrange

		window.show(); // Act：进入可见状态

		// Assert：关键控件按 objectName 查找均存在
		QVERIFY(window.findChild<QTabWidget*>(QStringLiteral("tabWidget")) != nullptr);
		QVERIFY(window.findChild<QWidget*>(QStringLiteral("treeWidget")) != nullptr);
		QVERIFY(window.findChild<QWidget*>(QStringLiteral("scrollArea")) != nullptr);
		QVERIFY(window.findChild<QToolBar*>(QStringLiteral("mainToolBar")) != nullptr);
		QVERIFY(window.findChild<QMenuBar*>(QStringLiteral("menuBar")) != nullptr);

		auto* langZh = window.findChild<QAction*>(QStringLiteral("actionLangChinese"));
		auto* langEn = window.findChild<QAction*>(QStringLiteral("actionLangEnglish"));
		QVERIFY(langZh != nullptr);
		QVERIFY(langEn != nullptr);
		if (langZh == nullptr || langEn == nullptr) {
			return; // 兜底：显式返回帮助静态分析器确认解引用安全
		}
		QVERIFY(langZh->isCheckable());
		QVERIFY(langEn->isCheckable());
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
	void tabWidgetHasAtLeastOnePage() {
		MainWindow window; // Arrange

		auto* tabs = window.findChild<QTabWidget*>(QStringLiteral("tabWidget"));
		QVERIFY(tabs != nullptr); // 前置条件，失败则不再解引用
		if (tabs == nullptr) {
			return; // 兜底：显式返回帮助静态分析器确认解引用安全
		}
		window.show(); // Act

		// Assert
		QVERIFY(tabs->count() >= 1);
		QVERIFY(tabs->currentIndex() >= 0 && tabs->currentIndex() < tabs->count());
	}

	/**
	 * @test   MainWindowUiTest.StatusBarIsAttached
	 * @brief  结构断言：状态栏已挂接到主窗口且含常驻语言指示。
	 * @details 目的：防止状态栏被误删或被 setStatusBar(nullptr) 置空，
	 *          导致后续状态提示类功能无落点。
	 *          设计思路：用 statusBar() 的挂接关系而非 isVisible()——
	 *          离屏会话下可见性不可靠；常驻标签按 objectName 断言。
	 * @expected window.statusBar() 非 NULL，statusLang 标签非空文本。
	 * @priority P1
	 */
	void statusBarIsAttached() {
		MainWindow window; // Arrange

		window.show(); // Act

		// Assert
		QVERIFY(window.statusBar() != nullptr);
		auto* statusLang = window.findChild<QLabel*>(QStringLiteral("statusLang"));
		QVERIFY(statusLang != nullptr);
		if (statusLang == nullptr) {
			return; // 兜底：显式返回帮助静态分析器确认解引用安全
		}
		QVERIFY(!statusLang->text().isEmpty());
	}

	/**
	 * @test   MainWindowUiTest.LanguageToggleRetranslatesUi
	 * @brief  行为断言：语言菜单切换后全窗口重翻译，且语言动作互斥勾选。
	 * @details 目的：锁住"中英文切换"契约——Tab 标题、窗口标题、状态栏
	 *          语言指示都要跟随切换；两个语言动作勾选状态互斥。
	 *          设计思路：触发 actionLangEnglish 后断言第 0 页标题变为
	 *          "Buttons"，切回中文后恢复"按钮"；currentLanguage() 与
	 *          勾选状态同步变化；幂等重复设置不发信号。
	 * @expected 英文模式下 tabText(0) == "Buttons"、标题含 "Gallery"；
	 *           中文模式下恢复"按钮"；语言动作勾选互斥；languageChanged
	 *           信号共发出 2 次（幂等调用不计数）。
	 * @priority P0
	 */
	void languageToggleRetranslatesUi() {
		MainWindow window; // Arrange
		window.show();

		auto* tabs = window.findChild<QTabWidget*>(QStringLiteral("tabWidget"));
		QVERIFY(tabs != nullptr);
		auto* langZh = window.findChild<QAction*>(QStringLiteral("actionLangChinese"));
		auto* langEn = window.findChild<QAction*>(QStringLiteral("actionLangEnglish"));
		QVERIFY(langZh != nullptr && langEn != nullptr);
		if (tabs == nullptr || langZh == nullptr || langEn == nullptr) {
			return; // 兜底：显式返回帮助静态分析器确认解引用安全
		}

		int changeCount = 0;
		QObject::connect(&window, &MainWindow::languageChanged, [&changeCount] { ++changeCount; });

		QVERIFY(window.currentLanguage() == MainWindow::Language::Chinese); // 初始为中文
		QCOMPARE(tabs->tabText(0), QStringLiteral("按钮"));

		langEn->trigger();                                     // Act：切到英文
		QCOMPARE(tabs->tabText(0), QStringLiteral("Buttons")); // Assert：标题已重翻译
		QVERIFY(window.windowTitle().contains(QStringLiteral("Gallery")));
		QVERIFY(langEn->isChecked());
		QVERIFY(!langZh->isChecked());
		QCOMPARE(changeCount, 1);

		langZh->trigger();                                  // Act：切回中文
		QCOMPARE(tabs->tabText(0), QStringLiteral("按钮")); // Assert：恢复中文
		QVERIFY(window.currentLanguage() == MainWindow::Language::Chinese);
		QVERIFY(langZh->isChecked());
		QCOMPARE(changeCount, 2);

		window.setLanguage(MainWindow::Language::Chinese); // Act：幂等重复设置
		QCOMPARE(changeCount, 2);                          // Assert：不发新信号
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
