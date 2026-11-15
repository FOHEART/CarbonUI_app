/**
 * @file  MainWindow.cpp
 * @brief 实现主窗口骨架装配 / 页面调度 / 语言切换 / 状态栏反馈（接口文档见 MainWindow.h）。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 *
 * @details
 * 构造顺序：setupUi -> createPages（五个页面加入 Tab）->
 * applyCarbonStyles（容器级风格 + 工具栏动作）-> connectSignals（菜单与
 * 页面状态消息）-> setLanguage(中文) 完成首次重翻译。
 * retranslateUi 只覆写骨架文本（窗口标题、菜单、Tab 标题、状态栏指示），
 * 页面内部文本由各页面在自己的 retranslatePage 中覆写。
 *
 * 本源码基于 Mozilla Public License 2.0 发布：
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "ButtonPage.h"
#include "CarbonUIClass.h"
#include "InputPage.h"
#include "ListPage.h"
#include "SelectionPage.h"
#include "TextOverflowPage.h"

#include <QtCore/QList>
#include <QtGui/QIcon>
#include <QtWidgets/QActionGroup>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>

namespace {
/// @brief 状态栏临时消息展示时长（毫秒）。
constexpr int kStatusTimeoutMs = 2000;
} // namespace

/**
 * @brief 构造主窗口并完成骨架装配、页面创建、风格应用与首次重翻译（详见 MainWindow.h）。
 * @param[in] parent 父窗口指针，可为 NULL；为 NULL 时成为独立顶层窗口。
 */
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
	ui = new Ui::MainWindow;
	ui->setupUi(this);

	createPages();
	applyCarbonStyles();
	connectSignals();

	// 状态栏常驻指示：语言 + 版本（右侧 permanent 区，不随临时消息消失）
	statusLang_ = new QLabel(this);
	statusLang_->setObjectName(QStringLiteral("statusLang"));
	statusVersion_ = new QLabel(QStringLiteral("v1.0.0"), this);
	statusVersion_->setObjectName(QStringLiteral("statusVersion"));
	statusBar()->addPermanentWidget(statusLang_);
	statusBar()->addPermanentWidget(statusVersion_);

	lang_ = Language::English; // 先置为它值，保证 setLanguage(Chinese) 一定执行重翻译
	setLanguage(Language::Chinese);

	CarbonUIClass::applyTooltipStyle();
	ui->tabWidget->setCurrentIndex(0);
}

/**
 * @brief 析构主窗口：子控件随 Qt 父子树自动释放。
 */
MainWindow::~MainWindow() {
	delete ui;
}

/**
 * @brief 创建五个分类测试页并按顺序加入 TabWidget（详见 MainWindow.h）。
 * @details 页面父对象交给 TabWidget（addTab 重设父级），构造顺序即 Tab
 *          顺序，retranslateUi 按 0..4 覆写标题。
 */
void MainWindow::createPages() {
	buttonPage_ = new ButtonPage(this);
	selectionPage_ = new SelectionPage(this);
	inputPage_ = new InputPage(this);
	listPage_ = new ListPage(this);
	textOverflowPage_ = new TextOverflowPage(this);

	ui->tabWidget->addTab(buttonPage_, QString());
	ui->tabWidget->addTab(selectionPage_, QString());
	ui->tabWidget->addTab(inputPage_, QString());
	ui->tabWidget->addTab(listPage_, QString());
	ui->tabWidget->addTab(textOverflowPage_, QString());
}

/**
 * @brief 应用容器级 CarbonUI 风格并创建工具栏动作（详见 MainWindow.h）。
 * @details 页面内部控件的风格由各页面在自身构造中应用；此处只处理主窗口
 *          骨架（菜单栏 / 工具栏 / Tab）。工具栏动作文案为空，tooltip 由
 *          retranslateUi 按语言覆写。
 */
void MainWindow::applyCarbonStyles() {
	CarbonUIClass::setupMenuBarStyle(ui->menuBar);
	CarbonUIClass::setupTabWidgetStyle(ui->tabWidget);
	CarbonUIClass::setupToolBarStyle(ui->mainToolBar);

	auto* toolOpen = ui->mainToolBar->addAction(QIcon(":/images/config.png"), QString());
	toolOpen->setObjectName(QStringLiteral("actionToolOpen"));
	auto* toolDown = ui->mainToolBar->addAction(QIcon(":/images/down.png"), QString());
	toolDown->setObjectName(QStringLiteral("actionToolDown"));
}

/**
 * @brief 建立全部信号连接：菜单动作与页面状态消息（详见 MainWindow.h）。
 * @details
 * 页面状态消息在触发时已按页面当前语言拼好文案，语言切换后由页面重翻译
 * 跟随更新。语言动作经 QActionGroup 保证互斥勾选。
 */
void MainWindow::connectSignals() {
	// 语言菜单：互斥勾选 + 切换
	auto* langGroup = new QActionGroup(this);
	langGroup->setExclusive(true);
	langGroup->addAction(ui->actionLangChinese);
	langGroup->addAction(ui->actionLangEnglish);
	ui->actionLangChinese->setChecked(true);
	connect(ui->actionLangChinese, &QAction::triggered, this,
	        [this] { setLanguage(Language::Chinese); });
	connect(ui->actionLangEnglish, &QAction::triggered, this,
	        [this] { setLanguage(Language::English); });

	// 文件 / 帮助
	connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
	connect(ui->actionAbout, &QAction::triggered, this, [this] {
		statusBar()->showMessage(
		    langText("CarbonUI 控件测试台 v1.0.0：分类验证 CarbonUI 控件的功能与显示效果。",
			         "CarbonUI Widget Gallery v1.0.0: verify CarbonUI widgets' behavior and "
			         "visuals."),
		    5000);
	});

	// 各页面交互消息 -> 状态栏临时消息
	const QList<GalleryPage*> pages = {buttonPage_, selectionPage_, inputPage_, listPage_,
	                                   textOverflowPage_};
	for (GalleryPage* page : pages) {
		connect(page, &GalleryPage::statusMessage, this, [this](const QString& message) {
			statusBar()->showMessage(message, kStatusTimeoutMs);
		});
	}
}

/**
 * @brief 切换界面语言并全量重翻译（详见 MainWindow.h）。
 * @param[in] lang 目标语言；与当前语言相同则幂等返回。
 */
void MainWindow::setLanguage(Language lang) {
	if (lang == lang_) {
		return;
	}
	lang_ = lang;
	retranslateUi();
	emit languageChanged(lang);
}

/**
 * @brief 查询当前界面语言（详见 MainWindow.h）。
 * @return 当前语言枚举值。
 */
MainWindow::Language MainWindow::currentLanguage() const {
	return lang_;
}

/**
 * @brief 按当前语言取双语字符串（详见 MainWindow.h）。
 * @param[in] zh 中文文案（UTF-8 字面量）。
 * @param[in] en 英文文案（UTF-8 字面量）。
 * @return 当前语言对应的文案。
 */
QString MainWindow::langText(const char* zh, const char* en) const {
	return lang_ == Language::Chinese ? QString::fromUtf8(zh) : QString::fromUtf8(en);
}

/**
 * @brief 按当前语言覆写骨架文本并把语言下发给各页面（详见 MainWindow.h）。
 * @details
 * 骨架覆盖：窗口标题 -> 菜单 -> Tab 页标题 -> 状态栏指示 -> 工具栏提示。
 * 页面内部文本由各页面在 retranslatePage 中覆写；语言菜单里两个动作的
 * 文字保持各自语言原样，不翻译。
 */
void MainWindow::retranslateUi() {
	// 窗口标题
	setWindowTitle(langText("CarbonUI 控件测试台", "CarbonUI Widget Gallery"));

	// 菜单栏
	ui->menuFile->setTitle(langText("文件", "File"));
	ui->menuLanguage->setTitle(langText("语言", "Language"));
	ui->menuHelp->setTitle(langText("帮助", "Help"));
	ui->actionExit->setText(langText("退出", "Exit"));
	ui->actionAbout->setText(langText("关于", "About"));

	// Tab 页标题（顺序与 createPages 的 addTab 顺序一致）
	ui->tabWidget->setTabText(0, langText("按钮", "Buttons"));
	ui->tabWidget->setTabText(1, langText("选择与开关", "Selection"));
	ui->tabWidget->setTabText(2, langText("输入", "Input"));
	ui->tabWidget->setTabText(3, langText("列表与折叠", "Lists"));
	ui->tabWidget->setTabText(4, langText("文本溢出", "Overflow"));

	// 各页面内部文本
	buttonPage_->retranslateUi(lang_);
	selectionPage_->retranslateUi(lang_);
	inputPage_->retranslateUi(lang_);
	listPage_->retranslateUi(lang_);
	textOverflowPage_->retranslateUi(lang_);

	// 状态栏常驻指示
	statusLang_->setText(langText("语言：中文", "Language: English"));

	// 工具栏动作提示（动作在构造时创建）
	auto* toolOpen = findChild<QAction*>(QStringLiteral("actionToolOpen"));
	if (toolOpen != nullptr) {
		toolOpen->setToolTip(langText("打开配置", "Open configuration"));
	}
	auto* toolDown = findChild<QAction*>(QStringLiteral("actionToolDown"));
	if (toolDown != nullptr) {
		toolDown->setToolTip(langText("下载", "Download"));
	}
}
