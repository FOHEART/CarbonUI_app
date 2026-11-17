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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>

#include "CarbonUIAboutQtDialog.h"
#include "DialogPage.h"
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

	/**
	 * @test   MainWindowUiTest.DialogPageOpensTestDialog
	 * @brief  行为断言：对话框页的按钮存在，且能构造出标题为「测试」的对话框。
	 * @details 目的：锁住"新 Tab 页已接入主窗口"与"按钮弹的窗与测试用的窗是
	 *          同一段构造代码"两条契约。不点按钮（exec 会阻塞），直接调用
	 *          DialogPage::createTestDialog 检查返回值。
	 * @expected 能按 objectName 找到 pushButtonOpenDialog；DialogPage 能从
	 *           主窗口对象树上查到；createTestDialog() 非 NULL、标题为「测试」、
	 *           含 CarbonUIDialogClose 关闭按钮。
	 * @priority P1
	 */
	void dialogPageOpensTestDialog() {
		MainWindow window; // Arrange
		window.show();

		auto* page = window.findChild<DialogPage*>();
		QVERIFY(page != nullptr);
		auto* openButton = window.findChild<QPushButton*>(QStringLiteral("pushButtonOpenDialog"));
		QVERIFY(openButton != nullptr);
		if (page == nullptr || openButton == nullptr) {
			return; // 兜底：显式返回帮助静态分析器确认解引用安全
		}

		QDialog* dialog = page->createTestDialog(nullptr); // Act
		QVERIFY(dialog != nullptr);
		if (dialog == nullptr) {
			return;
		}

		// Assert：标题、关闭按钮与四条信息行都在
		auto* title = dialog->findChild<QLabel*>(QStringLiteral("CarbonUIDialogTitle"));
		QVERIFY(title != nullptr);
		if (title != nullptr) {
			QCOMPARE(title->text(), QStringLiteral("测试"));
		}
		QVERIFY(dialog->findChild<QPushButton*>(QStringLiteral("CarbonUIDialogClose")) != nullptr);
		QCOMPARE(dialog->findChildren<QLabel*>(QStringLiteral("CarbonUIDialogRow")).size(), 4);

		delete dialog;
	}

	/**
	 * @test   MainWindowUiTest.DialogPageWindowButtonVariants
	 * @brief  行为断言：对话框页四个按钮能分别弹出「无 / 仅最小化 / 仅最大化 / 两者」。
	 * @details 目的：锁住 createTestDialog 的两个开关确实映射到 CarbonUIDialog 的
	 *          窗口按钮可见性且互不影响，同时验证最大化按钮不是摆设（点击后窗口
	 *          进入最大化、再点回到常规）。不点页面按钮（exec 会阻塞），直接调用
	 *          工厂函数检查按钮隐藏状态。
	 * @expected 四个打开按钮都能按 objectName 找到；不带开关的对话框两个按钮都隐
	 *           藏；仅最小化 → 最小化显示/最大化隐藏；仅最大化 → 反之；两者 → 都
	 *           显示；最大化按钮点击后 isMaximized() 为真、再点回假。
	 * @priority P1
	 */
	void dialogPageWindowButtonVariants() {
		MainWindow window; // Arrange
		window.show();

		auto* page = window.findChild<DialogPage*>();
		QVERIFY(page != nullptr);
		QVERIFY(window.findChild<QPushButton*>(QStringLiteral("pushButtonOpenDialogMinimize")) !=
		        nullptr);
		QVERIFY(window.findChild<QPushButton*>(QStringLiteral("pushButtonOpenDialogMaximize")) !=
		        nullptr);
		QVERIFY(window.findChild<QPushButton*>(QStringLiteral("pushButtonOpenDialogBoth")) !=
		        nullptr);
		if (page == nullptr) {
			return; // 兜底：显式返回帮助静态分析器确认解引用安全
		}

		// 窗口按钮是否显示：隐藏状态即可判定，无需 show()
		const auto shown = [](QDialog* dialog, const QString& name) {
			auto* button = dialog->findChild<QPushButton*>(name);
			return button != nullptr && !button->isHidden();
		};

		// Act + Assert：四种组合逐一看窗口按钮
		QDialog* plain = page->createTestDialog(nullptr);
		QVERIFY(!shown(plain, QStringLiteral("CarbonUIDialogMinimize")));
		QVERIFY(!shown(plain, QStringLiteral("CarbonUIDialogMaximize")));
		delete plain;

		QDialog* minimizeOnly = page->createTestDialog(nullptr, true, false);
		QVERIFY(shown(minimizeOnly, QStringLiteral("CarbonUIDialogMinimize")));
		QVERIFY(!shown(minimizeOnly, QStringLiteral("CarbonUIDialogMaximize")));
		delete minimizeOnly;

		QDialog* maximizeOnly = page->createTestDialog(nullptr, false, true);
		QVERIFY(!shown(maximizeOnly, QStringLiteral("CarbonUIDialogMinimize")));
		QVERIFY(shown(maximizeOnly, QStringLiteral("CarbonUIDialogMaximize")));
		delete maximizeOnly;

		QDialog* both = page->createTestDialog(nullptr, true, true);
		QVERIFY(shown(both, QStringLiteral("CarbonUIDialogMinimize")));
		QVERIFY(shown(both, QStringLiteral("CarbonUIDialogMaximize")));

		// 最大化按钮必须是真功能：切换窗口状态，不只是画个字形
		auto* maximizeButton =
		    both->findChild<QPushButton*>(QStringLiteral("CarbonUIDialogMaximize"));
		QVERIFY(maximizeButton != nullptr);
		if (maximizeButton != nullptr) {
			both->show();
			maximizeButton->click();
			QVERIFY(both->isMaximized());
			maximizeButton->click();
			QVERIFY(!both->isMaximized());
		}
		delete both;
	}

	/**
	 * @test   MainWindowUiTest.HelpMenuAboutDialogs
	 * @brief  行为断言：帮助菜单的「关于」「关于 Qt」都接到 CarbonUI 深色对话框。
	 * @details 目的：锁住"帮助菜单不是空壳"，且「关于」用 CarbonUI 风格对话框
	 *          （而非系统浅色 QMessageBox）、「关于 Qt」走库内置
	 *          CarbonUIAboutQtDialog，同时覆盖中英文重翻译。不点菜单项
	 *          （exec 会阻塞），直接调用 createAboutDialog / 构造
	 *          CarbonUIAboutQtDialog 检查标题与内容。
	 * @expected actionAbout / actionAboutQt 存在且中文文案为「关于」「关于 Qt」；
	 *           关于框标题「关于」、五行、首行含 1.0.0；关于 Qt 框标题
	 *           "ABOUT QT"、富文本正文含运行时 qVersion()；切英文后动作文案
	 *           变 About / About Qt（关于 Qt 对话框为库内置固定文案，与语言无关）。
	 * @priority P1
	 */
	void helpMenuAboutDialogs() {
		MainWindow window; // Arrange
		window.show();

		auto* aboutAction = window.findChild<QAction*>(QStringLiteral("actionAbout"));
		auto* aboutQtAction = window.findChild<QAction*>(QStringLiteral("actionAboutQt"));
		QVERIFY(aboutAction != nullptr);
		QVERIFY(aboutQtAction != nullptr);
		if (aboutAction == nullptr || aboutQtAction == nullptr) {
			return; // 兜底：显式返回帮助静态分析器确认解引用安全
		}
		QCOMPARE(aboutAction->text(), QStringLiteral("关于"));
		QCOMPARE(aboutQtAction->text(), QStringLiteral("关于 Qt"));

		// 「关于」：CarbonUI 深色对话框 + 五行信息
		QDialog* about = window.createAboutDialog(nullptr);
		QVERIFY(about != nullptr);
		if (about != nullptr) {
			auto* title = about->findChild<QLabel*>(QStringLiteral("CarbonUIDialogTitle"));
			QVERIFY(title != nullptr);
			if (title != nullptr) {
				QCOMPARE(title->text(), QStringLiteral("关于"));
			}
			const auto rows = about->findChildren<QLabel*>(QStringLiteral("CarbonUIDialogRow"));
			QCOMPARE(rows.size(), 5);
			if (!rows.isEmpty()) {
				QVERIFY(rows.first()->text().contains(QStringLiteral("1.0.0")));
			}
			delete about;
		}

		// 「关于 Qt」：库内置 CarbonUIAboutQtDialog，标题固定 ABOUT QT，
		// 富文本正文含运行时 Qt 版本（栈上构造，无生命周期管理）
		CarbonUIAboutQtDialog aboutQtDialog;
		auto* aboutQtTitle =
		    aboutQtDialog.findChild<QLabel*>(QStringLiteral("CarbonUIDialogTitle"));
		QVERIFY(aboutQtTitle != nullptr);
		if (aboutQtTitle != nullptr) {
			QCOMPARE(aboutQtTitle->text(), QStringLiteral("ABOUT QT"));
		}
		auto* aboutQtBody = aboutQtDialog.findChild<QLabel*>(QStringLiteral("CarbonUIAboutQtText"));
		QVERIFY(aboutQtBody != nullptr);
		if (aboutQtBody != nullptr) {
			QVERIFY(aboutQtBody->text().contains(QString::fromLatin1(qVersion())));
		}

		// 英文：菜单动作跟随语言；关于 Qt 对话框文案由库内置，与语言无关
		window.setLanguage(MainWindow::Language::English);
		QCOMPARE(aboutAction->text(), QStringLiteral("About"));
		QCOMPARE(aboutQtAction->text(), QStringLiteral("About Qt"));
	}

	/**
	 * @test   MainWindowUiTest.SelectionLabelClickTogglesCheck
	 * @brief  行为断言：指示灯居左形态下，点击旁置文案标签也能切换勾选状态。
	 * @details 目的：锁住 SelectionPage::eventFilter 的"标签点击 → 复选框
	 *          toggle"转发，避免两种指示灯形态的交互再次分叉（居右形态点文字
	 *          本来就生效）。用 QTest::mouseClick 把点击打在标签中心。
	 * @expected 初始未勾选；点 labelCheckA 后 checkIndicatorA 变为勾选；
	 *           再点一次回到未勾选。
	 * @priority P1
	 */
	void selectionLabelClickTogglesCheck() {
		MainWindow window; // Arrange
		window.show();
		auto* tabs = window.findChild<QTabWidget*>(QStringLiteral("tabWidget"));
		QVERIFY(tabs != nullptr);
		if (tabs != nullptr) {
			tabs->setCurrentIndex(1); // 「选择与开关」页
		}

		auto* checkBox = window.findChild<QCheckBox*>(QStringLiteral("checkIndicatorA"));
		auto* label = window.findChild<QLabel*>(QStringLiteral("labelCheckA"));
		QVERIFY(checkBox != nullptr);
		QVERIFY(label != nullptr);
		if (checkBox == nullptr || label == nullptr) {
			return; // 兜底：显式返回帮助静态分析器确认解引用安全
		}

		const bool initial = checkBox->isChecked();

		QTest::mouseClick(label, Qt::LeftButton); // Act
		QCOMPARE(checkBox->isChecked(), !initial);

		QTest::mouseClick(label, Qt::LeftButton);
		QCOMPARE(checkBox->isChecked(), initial);
	}

	/**
	 * @test   MainWindowUiTest.ScrollPageDemonstratesAreaAndBars
	 * @brief  行为断言：新增「滚动」页装好 CarbonUIQScrollArea 与两种滚动条。
	 * @details 目的：锁住第 7 个 Tab（滚动）确实存在且演示对象齐全——滚动区内部
	 *          两条滚动条由 CarbonUIQScrollBar 刷过（8px 命中范围、常态 4px），竖直条来自
	 *          CarbonUIQScrollBar 构造函数、水平条来自原生 QScrollBar +
	 *          applyStyle()，取值变化会同步数值标签。
	 * @expected 末页标题为「滚动」；scrollAreaDemo 有内容且两条滚动条带 CarbonUI
	 *           QSS；scrollBarVertical 为竖直条、scrollBarHorizontal 为水平条且
	 *           都带 #999999 滑块规则；setValue(72) 后竖直条数值标签含 72。
	 * @priority P1
	 */
	void scrollPageDemonstratesAreaAndBars() {
		MainWindow window; // Arrange
		window.show();

		auto* tabs = window.findChild<QTabWidget*>(QStringLiteral("tabWidget"));
		QVERIFY(tabs != nullptr);
		if (tabs != nullptr) {
			QCOMPARE(tabs->tabText(tabs->count() - 1), QStringLiteral("滚动"));
		}

		// 滚动区：内部滚动条走 CarbonUIQScrollBar（8px 命中范围、常态 4px、悬停 8px）
		auto* area = window.findChild<QScrollArea*>(QStringLiteral("scrollAreaDemo"));
		QVERIFY(area != nullptr);
		if (area != nullptr) {
			QVERIFY(area->widget() != nullptr);
			QVERIFY(area->verticalScrollBar()->styleSheet().contains(
			    QStringLiteral("QScrollBar:vertical { background: #2e2e2e; width: 8px;")));
			QVERIFY(area->horizontalScrollBar()->styleSheet().contains(
			    QStringLiteral("QScrollBar:horizontal { background: #2e2e2e; height: 8px;")));
		}

		// 独立滚动条：竖直条来自构造，水平条来自 applyStyle 静态接口
		auto* verticalBar = window.findChild<QScrollBar*>(QStringLiteral("scrollBarVertical"));
		auto* horizontalBar = window.findChild<QScrollBar*>(QStringLiteral("scrollBarHorizontal"));
		QVERIFY(verticalBar != nullptr);
		QVERIFY(horizontalBar != nullptr);
		if (verticalBar == nullptr || horizontalBar == nullptr) {
			return; // 兜底：显式返回帮助静态分析器确认解引用安全
		}
		QCOMPARE(verticalBar->orientation(), Qt::Vertical);
		QCOMPARE(horizontalBar->orientation(), Qt::Horizontal);
		QVERIFY(verticalBar->styleSheet().contains(
		    QStringLiteral("QScrollBar::handle:vertical { background: #999999;")));
		QVERIFY(horizontalBar->styleSheet().contains(
		    QStringLiteral("QScrollBar::handle:horizontal { background: #999999;")));

		// 取值回显：拖动滚动条后数值标签同步
		auto* value = window.findChild<QLabel*>(QStringLiteral("labelScrollBarVerticalValue"));
		QVERIFY(value != nullptr);
		if (value != nullptr) {
			verticalBar->setValue(72);
			QVERIFY(value->text().contains(QStringLiteral("72")));
		}
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
