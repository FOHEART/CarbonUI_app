/**
 * @file  main.cpp
 * @brief 应用层截图基线测试入口：可绘制文字的原生后端 + 内置字体 + 固定缩放。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_tests
 *
 * @details
 * 与 `tests/ui` 的离屏冒烟测试分工不同：本入口专门服务"整窗截图与基线比对"，
 * 因此三件事必须在 QApplication 构造前后固定下来：
 *
 * - **平台后端**：Windows 默认 `windows`（`offscreen` 在 Windows 上没有字体库，
 *   文字不会被绘制）；其它平台保持 `offscreen`。调用方显式设置了
 *   `QT_QPA_PLATFORM` 时尊重调用方。
 * - **缩放与 DPI**：`QT_SCALE_FACTOR=1`、`QT_AUTO_SCREEN_SCALE_FACTOR=0`、
 *   `QT_FONT_DPI=96`，否则 150% 缩放的开发机会抓出比基线大 1.5 倍的图。
 * - **字体**：加载仓库内置的 HarmonyOS Sans SC（编译期宏
 *   `CARBONUI_APP_VISUAL_FONT` 给出源树绝对路径）并设为应用默认字体，
 *   本地与 CI 渲染同一份 TTF，字形度量一致。
 *
 * 本程序从不 `show()` 主窗口：抓帧统一走 `QWidget::grab()`，不会弹窗。
 *
 * 线程约定：与所有 Qt Widgets 用例一样，只在主线程执行。
 */

#include <gtest/gtest.h>

#include <cstdio>

#include <QtCore/QByteArray>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtGui/QFont>
#include <QtWidgets/QApplication>

#include "AppFont.h"

namespace {
/**
 * @brief 固定应用默认字体，让文字渲染跨机器可复现。
 *
 * @details
 * 走与产品一致的字体加载链路（appcore::loadApplicationFont），只是字体文件
 * 取自源码树而非运行时 `Resources/` 目录，避免测试依赖部署产物。
 * 加载失败时保留系统字体并打警告，此时跨机器差异只能靠容差吸收。
 */
void pinReproducibleFont() {
	const QString family = appcore::loadApplicationFont(QStringLiteral(CARBONUI_APP_VISUAL_FONT));
	if (family.isEmpty()) {
		std::fprintf(stderr,
		             "[WARN] 内置字体加载失败：%s\n"
		             "       截图基线可能跨机器不一致。\n",
		             CARBONUI_APP_VISUAL_FONT);
		return;
	}

	QFont font = QApplication::font();
	font.setFamily(family);
	font.setPointSize(9);
	font.setBold(false);
	QApplication::setFont(font);
}

/**
 * @brief 加载与产品相同的全局样式表，避免"测试里好看、线上不一样"。
 *
 * @details 产品 main.cpp 从运行时 `Resources/styles/app.qss` 读取并
 * `app.setStyleSheet`；视觉测试改为编译期给出源码树里的同一份文件
 * （`CARBONUI_APP_VISUAL_QSS`），保证截图反映真实外观。读不到时保留
 * MainWindow 自身的控件级样式并打警告（此时页签栏底色等全局规则缺失）。
 *
 * @param[in] app 已构造的 QApplication，不可为 NULL。
 */
void loadProductionStyleSheet(QApplication* app) {
	QFile file(QStringLiteral(CARBONUI_APP_VISUAL_QSS));
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		std::fprintf(stderr, "[WARN] 全局样式表读取失败：%s\n", CARBONUI_APP_VISUAL_QSS);
		return;
	}
	app->setStyleSheet(QString::fromUtf8(file.readAll()));
}
} // namespace

/**
 * @brief 截图基线测试入口：先初始化 gtest，固定后端/缩放/字体，再跑用例。
 * @param[in] argc 命令行参数个数，由运行器提供，取值 [1, +∞)。
 * @param[in] argv 命令行参数数组，不可为 NULL；`--gtest_*` 由 gtest 摘除。
 * @return 用例执行结果，0 表示全部通过，非 0 表示存在失败。
 */
int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
#ifdef Q_OS_WIN
		qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("windows"));
#else
		qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("offscreen"));
#endif
	}
	if (qEnvironmentVariableIsEmpty("QT_SCALE_FACTOR")) {
		qputenv("QT_SCALE_FACTOR", QByteArrayLiteral("1"));
	}
	if (qEnvironmentVariableIsEmpty("QT_AUTO_SCREEN_SCALE_FACTOR")) {
		qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", QByteArrayLiteral("0"));
	}
	if (qEnvironmentVariableIsEmpty("QT_FONT_DPI")) {
		qputenv("QT_FONT_DPI", QByteArrayLiteral("96"));
	}

	QApplication app(argc, argv);
	// 与产品 main.cpp 同因：qrc 编在静态库 applib 里，必须显式引用注册符号，
	// 否则工具栏图标 / 下拉箭头等资源图片全为空，截图与线上不符。
	Q_INIT_RESOURCE(Resource);
	pinReproducibleFont();
	loadProductionStyleSheet(&app);

	return RUN_ALL_TESTS();
}
