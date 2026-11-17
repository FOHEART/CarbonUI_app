/**
 * @file  DialogPage.cpp
 * @brief 对话框测试页实现：装配 / CarbonUI 风格 / 弹出「测试」对话框（接口见 DialogPage.h）。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 *
 * @details
 * 页面只有一个分组：说明文字 + 四个打开按钮（无窗口按钮 / 仅最小化 / 仅最大化 /
 * 最小化 + 最大化）。对话框本体由 CarbonUI 子库的 CarbonUIDialog 负责（无边框
 * 圆角面板 + 标题条 + 键值行 + 窗口按钮），本页只提供演示数据与按钮组合；数据
 * 按当前语言取标签、取值保持产品化的英文原样。
 *
 * 本源码基于 Mozilla Public License 2.0 发布：
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "DialogPage.h"

#include "CarbonUIQPushButton.h"
#include "CarbonUIDialog.h"
#include "CarbonUIColorPickWidget.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

namespace {
/// objectName 常量：供界面测试与查找使用。
const char* const kNameGroupDialogs = "groupBoxDialogs";
const char* const kNameDialogHint = "labelDialogHint";
const char* const kNameOpenDialog = "pushButtonOpenDialog";
const char* const kNameOpenDialogMinimize = "pushButtonOpenDialogMinimize";
const char* const kNameOpenDialogMaximize = "pushButtonOpenDialogMaximize";
const char* const kNameOpenDialogBoth = "pushButtonOpenDialogBoth";
const char* const kNameOpenColorPick = "pushButtonOpenColorPick";
const char* const kNameGroupColorPick = "groupBoxColorPick";
const char* const kNameColorSwatch = "labelColorSwatch";
const char* const kNameColorHex = "labelColorHex";

/// 演示用的修订号（与 CarbonUI 产品线 About 对话框一致的长哈希）。
const char* const kRevision = "2109c6b5729501538b788d424a783067a175beca";
/// 演示用的构建时间。
const char* const kBuildDate = "Tue Jul 29 10:38:14 2025";
/// 演示用的程序版本。
const char* const kProgramVersion = "2025.2";
/// 演示用的许可证类型。
const char* const kLicense = "Perpetual";
} // namespace

/**
 * @brief 构造对话框页：装配 -> 风格（详见 DialogPage.h）。
 * @param[in] parent 父控件指针，可为 NULL；通常由主窗口持有。
 */
DialogPage::DialogPage(QWidget* parent) : GalleryPage(parent) {
	buildUi();
	applyStyles();
}

/**
 * @brief 构建控件树与布局：「对话框」分组（说明 + 四个打开按钮）与
 *        「颜色选择」分组（打开按钮 + 结果颜色块 + 结果十六进制）。
 */
void DialogPage::buildUi() {
	auto* pageLayout = new QVBoxLayout(this);
	pageLayout->setContentsMargins(12, 12, 12, 12);
	pageLayout->setSpacing(10);

	groupBoxDialogs_ = new QGroupBox(this);
	groupBoxDialogs_->setObjectName(QLatin1String(kNameGroupDialogs));
	auto* groupLayout = new QVBoxLayout(groupBoxDialogs_);
	groupLayout->setSpacing(8);

	labelDialogHint_ = new QLabel(groupBoxDialogs_);
	labelDialogHint_->setObjectName(QLatin1String(kNameDialogHint));
	labelDialogHint_->setWordWrap(true);
	groupLayout->addWidget(labelDialogHint_);

	// 一行四个按钮：无窗口按钮 / 仅最小化 / 仅最大化 / 最小化 + 最大化
	auto* buttonLayout = new QHBoxLayout;
	buttonLayout->setSpacing(8);
	pushButtonOpenDialog_ = new QPushButton(groupBoxDialogs_);
	pushButtonOpenDialog_->setObjectName(QLatin1String(kNameOpenDialog));
	buttonLayout->addWidget(pushButtonOpenDialog_);

	pushButtonOpenDialogMinimize_ = new QPushButton(groupBoxDialogs_);
	pushButtonOpenDialogMinimize_->setObjectName(QLatin1String(kNameOpenDialogMinimize));
	buttonLayout->addWidget(pushButtonOpenDialogMinimize_);

	pushButtonOpenDialogMaximize_ = new QPushButton(groupBoxDialogs_);
	pushButtonOpenDialogMaximize_->setObjectName(QLatin1String(kNameOpenDialogMaximize));
	buttonLayout->addWidget(pushButtonOpenDialogMaximize_);

	pushButtonOpenDialogBoth_ = new QPushButton(groupBoxDialogs_);
	pushButtonOpenDialogBoth_->setObjectName(QLatin1String(kNameOpenDialogBoth));
	buttonLayout->addWidget(pushButtonOpenDialogBoth_);

	buttonLayout->addSpacerItem(
	    new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	groupLayout->addLayout(buttonLayout);

	pageLayout->addWidget(groupBoxDialogs_);

	// 「颜色选择」分组：打开按钮 + 确认后的结果展示（颜色块 + 十六进制）
	groupBoxColorPick_ = new QGroupBox(this);
	groupBoxColorPick_->setObjectName(QLatin1String(kNameGroupColorPick));
	auto* colorLayout = new QHBoxLayout(groupBoxColorPick_);
	colorLayout->setSpacing(8);
	pushButtonOpenColorPick_ = new QPushButton(groupBoxColorPick_);
	pushButtonOpenColorPick_->setObjectName(QLatin1String(kNameOpenColorPick));
	colorLayout->addWidget(pushButtonOpenColorPick_);

	labelColorSwatch_ = new QLabel(groupBoxColorPick_);
	labelColorSwatch_->setObjectName(QLatin1String(kNameColorSwatch));
	labelColorSwatch_->setFixedSize(48, 26);
	labelColorSwatch_->setAlignment(Qt::AlignCenter);
	colorLayout->addWidget(labelColorSwatch_);

	labelColorHex_ = new QLabel(groupBoxColorPick_);
	labelColorHex_->setObjectName(QLatin1String(kNameColorHex));
	colorLayout->addWidget(labelColorHex_);

	// 初始展示取色面板的默认颜色（黑色），点击 Ok 后被所选颜色覆盖
	labelColorSwatch_->setStyleSheet(QStringLiteral(
	    "background-color: #000000; border: 1px solid #5f5f5f; border-radius: 6px;"));
	labelColorHex_->setText(QStringLiteral("#000000"));

	colorLayout->addSpacerItem(
	    new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	pageLayout->addWidget(groupBoxColorPick_);

	pageLayout->addSpacerItem(
	    new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

	// 四个按钮只差"显示哪个窗口按钮"；对话框是堆对象，exec 返回后释放
	connect(pushButtonOpenDialog_, &QPushButton::clicked, this,
	        [this] { openDialog(false, false); });
	connect(pushButtonOpenDialogMinimize_, &QPushButton::clicked, this,
	        [this] { openDialog(true, false); });
	connect(pushButtonOpenDialogMaximize_, &QPushButton::clicked, this,
	        [this] { openDialog(false, true); });
	connect(pushButtonOpenDialogBoth_, &QPushButton::clicked, this,
	        [this] { openDialog(true, true); });
	connect(pushButtonOpenColorPick_, &QPushButton::clicked, this, [this] { openColorPicker(); });
}

/**
 * @brief 应用 CarbonUI 按钮风格：四个打开按钮统一主色四角圆角样式。
 */
void DialogPage::applyStyles() {
	CarbonUIQPushButton::applyStyle(pushButtonOpenDialog_, CarbonUIQPushButton::Shape_RoundAll);
	CarbonUIQPushButton::applyStyle(pushButtonOpenDialogMinimize_,
	                                CarbonUIQPushButton::Shape_RoundAll);
	CarbonUIQPushButton::applyStyle(pushButtonOpenDialogMaximize_,
	                                CarbonUIQPushButton::Shape_RoundAll);
	CarbonUIQPushButton::applyStyle(pushButtonOpenDialogBoth_, CarbonUIQPushButton::Shape_RoundAll);
	CarbonUIQPushButton::applyStyle(pushButtonOpenColorPick_, CarbonUIQPushButton::Shape_RoundAll);
}

/**
 * @brief 以模态方式弹出「测试」对话框，返回后释放（详见 DialogPage.h）。
 * @param[in] minimize true 显示最小化按钮。
 * @param[in] maximize true 显示最大化按钮。
 */
void DialogPage::openDialog(bool minimize, bool maximize) {
	QDialog* dialog = createTestDialog(this, minimize, maximize);
	dialog->exec();
	dialog->deleteLater();
}

/**
 * @brief 构造「测试」对话框（详见 DialogPage.h）。
 * @param[in] parent 对话框父窗口，可为 NULL。
 * @return 新建的 CarbonUIDialog，四行信息已填充、两个窗口按钮均隐藏。
 */
QDialog* DialogPage::createTestDialog(QWidget* parent) const {
	return createTestDialog(parent, false, false);
}

/**
 * @brief 构造「测试」对话框并分别开关窗口按钮（详见 DialogPage.h）。
 * @param[in] parent   对话框父窗口，可为 NULL。
 * @param[in] minimize true 显示最小化按钮。
 * @param[in] maximize true 显示最大化按钮。
 * @return 新建的 CarbonUIDialog，四行信息已填充。
 */
QDialog* DialogPage::createTestDialog(QWidget* parent, bool minimize, bool maximize) const {
	auto* dialog = new CarbonUIDialog(langText("测试", "Test dialog"), parent);
	dialog->addInfoRow(langText("应用修订号", "Application revision number"),
	                   QLatin1String(kRevision));
	dialog->addInfoRow(langText("应用构建时间", "Application date"), QLatin1String(kBuildDate));
	dialog->addInfoRow(langText("程序版本", "Program version"), QLatin1String(kProgramVersion));
	dialog->addInfoRow(langText("许可证", "License"), QLatin1String(kLicense));
	// 最小化 / 最大化按钮按需显示，点击即执行真实的窗口操作
	dialog->setMinimizeButtonVisible(minimize);
	dialog->setMaximizeButtonVisible(maximize);
	return dialog;
}

/**
 * @brief 以模态方式弹出「颜色选择」对话框，确认后把结果回填到分组内的
 *        颜色块与十六进制标签，并上抛状态栏消息（详见 DialogPage.h）。
 */
void DialogPage::openColorPicker() {
	QDialog* dialog = createColorPickerDialog(this);
	auto* picker = dialog->findChild<CarbonUIColorPickWidget*>();
	connect(dialog, &QDialog::accepted, this, [this, picker] {
		const QColor picked = (picker != nullptr ? picker->color() : QColor(Qt::black));
		// 颜色块：直接以所选颜色填充；十六进制：形如 #RRGGBB
		labelColorSwatch_->setStyleSheet(
		    QStringLiteral("background-color: %1; border: 1px solid #5f5f5f; border-radius: 6px;")
		        .arg(picked.name()));
		labelColorHex_->setText(picked.name().toUpper());
		emit statusMessage(langText("已选择颜色：", "Color selected: ") + picked.name());
	});
	dialog->exec();
	dialog->deleteLater();
}

/**
 * @brief 构造「颜色选择」对话框（详见 DialogPage.h）。
 * @param[in] parent 对话框父窗口，可为 NULL。
 * @return 新建的无边框 QDialog，内嵌 CarbonUIColorPickWidget，Ok/Cancel 分别
 *         驱动 accept()/reject()。
 */
QDialog* DialogPage::createColorPickerDialog(QWidget* parent) const {
	auto* dialog = new QDialog(parent);
	// 无边框 + 半透明背景：去掉白色系统标题栏，四角露出取色面板的圆角；Esc 仍可取消
	dialog->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	dialog->setAttribute(Qt::WA_TranslucentBackground, true);
	// 底色与边框/圆角全部由内嵌取色面板自绘（#1f1f1f 面板 + 2px #0f0f0f 边框）
	auto* layout = new QVBoxLayout(dialog);
	layout->setContentsMargins(0, 0, 0, 0);
	auto* picker = new CarbonUIColorPickWidget(dialog);
	layout->addWidget(picker);
	connect(picker, &CarbonUIColorPickWidget::accepted, dialog, &QDialog::accept);
	connect(picker, &CarbonUIColorPickWidget::rejected, dialog, &QDialog::reject);
	return dialog;
}

/**
 * @brief 按当前语言覆写页内全部文本（详见 DialogPage.h）。
 */
void DialogPage::retranslatePage() {
	groupBoxDialogs_->setTitle(langText("对话框", "Dialogs"));
	groupBoxColorPick_->setTitle(langText("颜色选择", "Color picker"));
	labelDialogHint_->setText(langText(
	    "CarbonUI 深色对话框：无系统边框、圆角面板、黑色标题条与键值行正文。标题条的"
	    "最小化与最大化按钮可按需显示，点击即生效（最小化到任务栏、最大化铺满并切换"
	    "为还原形）。「颜色选择」对话框内嵌 CarbonUIColorPickWidget 取色面板：选色"
	    "区/明度滑条取色、RGB 与十六进制输入联动，Ok/Cancel 驱动对话框返回。",
	    "CarbonUI dark dialog: frameless rounded panel with a black title bar and key/value "
	    "rows. The title-bar minimize and maximize buttons can be shown on demand and they "
	    "really work (minimize to the task bar, maximize to fill and toggle back). The "
	    "\"Color picker\" dialog embeds the CarbonUIColorPickWidget panel: picking on the "
	    "color area/value slider stays in sync with the RGB and hex editors, and Ok/Cancel "
	    "drive the dialog result."));
	pushButtonOpenDialog_->setText(
	    langText("打开「测试」对话框", "Open the \"Test dialog\" dialog"));
	pushButtonOpenDialogMinimize_->setText(langText("仅最小化", "Minimize only"));
	pushButtonOpenDialogMaximize_->setText(langText("仅最大化", "Maximize only"));
	pushButtonOpenDialogBoth_->setText(langText("最小化 + 最大化", "Minimize + maximize"));
	pushButtonOpenColorPick_->setText(
	    langText("打开「颜色选择」对话框", "Open the \"Color picker\" dialog"));
}
