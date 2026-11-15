/**
 * @file  app_screenshot_test.cpp
 * @brief 整窗截图基线：每个 Tab 页 + 「测试」对话框各一张，拦截显示风格回归。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_tests
 *
 * @details
 * 复用 CarbonUI 子库的基线比对工具（`tests/support/screenshot_support.h`：
 * 抓图、逐像素比对、差异图、缺基线跳过），只是基线目录换成本工程的
 * `tests/visual/baseline/<平台>/`。
 *
 * 取景：主窗口固定 980x660，逐个 Tab 切换后整窗抓图（含菜单栏、工具栏、
 * 页签栏、状态栏，覆盖"最终显示风格"）；对话框用例直接调用
 * @ref DialogPage::createTestDialog，与运行时按钮弹出的窗是同一段构造代码。
 *
 * 运行开关（本工程专用，避免 CI 在没有 Linux 基线时误比）：
 * - `CARBONUI_SCREENSHOT_RECORD=1` 记录缺失基线，`=force` 强制重录；
 * - `CARBONUI_APP_SCREENSHOT_LOCAL=1` 本地比对；
 * - 两者都未设置时用例跳过（CI 默认即为跳过，不会因缺少某平台基线变红）。
 *
 * 生成/刷新基线：
 * @code
 * set CARBONUI_SCREENSHOT_RECORD=1
 * out\build\tdd-tests\Release\app_visual_tests.exe
 * @endcode
 */

#include <gtest/gtest.h>

#include <QtCore/QString>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

#include "DialogPage.h"
#include "MainWindow.h"
#include "screenshot_support.h"

namespace {
/// 抓图窗口宽度（像素），写死以保证基线尺寸稳定。
constexpr int kWindowWidth = 980;
/// 抓图窗口高度（像素）。
constexpr int kWindowHeight = 660;

/**
 * @brief 本工程的截图套件开关。
 *
 * @details 只看记录模式与显式本地开关，**不看** CI：Linux 基线尚未录制时，
 * CI 上按通用开关会直接比对失败。需要 CI 比对时，先补平台基线再把这里改成
 * 复用 `carbonui_test::screenshotSuiteEnabled()`。
 *
 * @return 应该抓图/比对时返回 true。
 */
bool appScreenshotEnabled() {
	const QString record = qEnvironmentVariable("CARBONUI_SCREENSHOT_RECORD").trimmed();
	if (!record.isEmpty() && record != QLatin1String("0")) {
		return true;
	}
	return !qEnvironmentVariableIsEmpty("CARBONUI_APP_SCREENSHOT_LOCAL");
}

/**
 * @brief 抓图并与基线比对；控件所有权仍归调用方。
 *
 * @details 与子库的 CARBONUI_CHECK_SCREENSHOT 宏行为一致（缺基线跳过、不一致
 * 失败并给出三份图路径），区别是不 delete 传入的控件——这里的被测对象是
 * 栈上的窗口或由用例自己管理的指针。
 *
 * @param[in] caseName 用例名，同时决定基线文件名，不可为空。
 * @param[in] widget   待抓图控件，不可为 NULL。
 */
void verifyAppShot(const QString& caseName, QWidget* widget) {
	if (!appScreenshotEnabled()) {
		GTEST_SKIP() << "app 截图套件默认跳过；记录基线设 CARBONUI_SCREENSHOT_RECORD=1，"
		                "本地比对设 CARBONUI_APP_SCREENSHOT_LOCAL=1";
	}

	const carbonui_test::ScreenshotCheck check = carbonui_test::verifyScreenshot(widget, caseName);
	if (check.baselineMissing) {
		GTEST_SKIP() << check.message.toStdString();
	}
	ASSERT_TRUE(check.matched) << check.message.toStdString();
}

/**
 * @brief 建一个固定尺寸的主窗口并切到指定 Tab。
 *
 * @param[in] tabIndex 目标页签下标，取值 [0, tabWidget->count())。
 * @return 新建窗口（堆对象，由用例释放）；切页失败时返回的窗口停在原页。
 */
MainWindow* makeWindowAt(int tabIndex) {
	auto* window = new MainWindow();
	// WA_DontShowOnScreen：让 show() 完成 polish/布局（工具栏按钮、状态栏等
	// 只有走过 show 才有几何）但不真正映射到屏幕，CI 与本地都不会弹窗。
	window->setAttribute(Qt::WA_DontShowOnScreen, true);
	window->resize(kWindowWidth, kWindowHeight);
	window->show();
	if (auto* tabs = window->findChild<QTabWidget*>(QStringLiteral("tabWidget"))) {
		tabs->setCurrentIndex(tabIndex);
	}
	QCoreApplication::processEvents();
	return window;
}
} // namespace

TEST(AppScreenshot, TabButtons) {
	MainWindow* window = makeWindowAt(0);
	verifyAppShot(QStringLiteral("app-tab-buttons"), window);
	delete window;
}

TEST(AppScreenshot, TabSelection) {
	MainWindow* window = makeWindowAt(1);
	verifyAppShot(QStringLiteral("app-tab-selection"), window);
	delete window;
}

TEST(AppScreenshot, TabInput) {
	MainWindow* window = makeWindowAt(2);
	verifyAppShot(QStringLiteral("app-tab-input"), window);
	delete window;
}

TEST(AppScreenshot, TabLists) {
	MainWindow* window = makeWindowAt(3);
	verifyAppShot(QStringLiteral("app-tab-lists"), window);
	delete window;
}

TEST(AppScreenshot, TabOverflow) {
	MainWindow* window = makeWindowAt(4);
	verifyAppShot(QStringLiteral("app-tab-overflow"), window);
	delete window;
}

TEST(AppScreenshot, TabDialogs) {
	MainWindow* window = makeWindowAt(5);
	verifyAppShot(QStringLiteral("app-tab-dialogs"), window);
	delete window;
}

TEST(AppScreenshot, DialogTest) {
	DialogPage page;
	QDialog* dialog = page.createTestDialog(nullptr);
	dialog->adjustSize();

	verifyAppShot(QStringLiteral("app-dialog-test"), dialog);

	delete dialog;
}
