/**
 * @file  InputPage.cpp
 * @brief 输入测试页实现：装配 / CarbonUI 风格 / 交互反馈 / 双语覆写（接口见 InputPage.h）。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 *
 * @details
 * 控件树与布局沿用原 MainWindow.ui 中"输入"Tab 的结构。下拉框选项为动态
 * 内容：每次 retranslatePage 清空重建，选中索引随之复位，属预期行为。
 *
 * 本源码基于 Mozilla Public License 2.0 发布：
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "InputPage.h"

#include "CarbonUIQComboBox.h"
#include "CarbonUIQLineEdit.h"

#include <QtWidgets/QVBoxLayout>

namespace {
/// objectName 常量：与原 MainWindow.ui 命名一致，供测试与查找使用。
const char* const kNameGroupNumber = "groupBoxNumber";
const char* const kNameNumberHint = "labelNumberHint";
const char* const kNameLineEditNumber = "lineEditNumber";
const char* const kNameGroupLineEdit = "groupBoxLineEdit";
const char* const kNameLineEditFree = "lineEditFree";
const char* const kNameGroupCombo = "groupBoxCombo";
const char* const kNameComboHint = "labelComboHint";
const char* const kNameCombo = "comboBox";
} // namespace

/**
 * @brief 构造输入页：装配 -> 风格 -> 信号（详见 InputPage.h）。
 * @param[in] parent 父控件指针，可为 NULL；通常由主窗口持有。
 */
InputPage::InputPage(QWidget* parent) : GalleryPage(parent) {
	buildUi();
	applyStyles();
	connectPageSignals();
}

/**
 * @brief 构建控件树与布局：数字输入分组 + 普通输入分组 + 下拉框分组。
 */
void InputPage::buildUi() {
	auto* pageLayout = new QVBoxLayout(this);
	pageLayout->setContentsMargins(12, 12, 12, 12);
	pageLayout->setSpacing(10);

	// 数字输入框分组
	groupBoxNumber_ = new QGroupBox(this);
	groupBoxNumber_->setObjectName(QLatin1String(kNameGroupNumber));
	auto* numberLayout = new QVBoxLayout(groupBoxNumber_);
	numberLayout->setSpacing(8);
	labelNumberHint_ = new QLabel(groupBoxNumber_);
	labelNumberHint_->setObjectName(QLatin1String(kNameNumberHint));
	numberLayout->addWidget(labelNumberHint_);
	lineEditNumber_ = new QLineEdit(groupBoxNumber_);
	lineEditNumber_->setObjectName(QLatin1String(kNameLineEditNumber));
	numberLayout->addWidget(lineEditNumber_);
	pageLayout->addWidget(groupBoxNumber_);

	// 普通输入框分组
	groupBoxLineEdit_ = new QGroupBox(this);
	groupBoxLineEdit_->setObjectName(QLatin1String(kNameGroupLineEdit));
	auto* lineLayout = new QVBoxLayout(groupBoxLineEdit_);
	lineLayout->setSpacing(8);
	lineEditFree_ = new QLineEdit(groupBoxLineEdit_);
	lineEditFree_->setObjectName(QLatin1String(kNameLineEditFree));
	lineLayout->addWidget(lineEditFree_);
	pageLayout->addWidget(groupBoxLineEdit_);

	// 下拉框分组
	groupBoxCombo_ = new QGroupBox(this);
	groupBoxCombo_->setObjectName(QLatin1String(kNameGroupCombo));
	auto* comboLayout = new QVBoxLayout(groupBoxCombo_);
	comboLayout->setSpacing(8);
	labelComboHint_ = new QLabel(groupBoxCombo_);
	labelComboHint_->setObjectName(QLatin1String(kNameComboHint));
	comboLayout->addWidget(labelComboHint_);
	comboBox_ = new QComboBox(groupBoxCombo_);
	comboBox_->setObjectName(QLatin1String(kNameCombo));
	comboLayout->addWidget(comboBox_);
	pageLayout->addWidget(groupBoxCombo_);

	pageLayout->addSpacerItem(
	    new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

/**
 * @brief 应用 CarbonUI 风格：数字输入框（聚焦全选 + 校验）与下拉框。
 */
void InputPage::applyStyles() {
	CarbonUIQLineEdit::applyStyle(lineEditNumber_, 200);
	CarbonUIQComboBox::applyStyle(comboBox_);
}

/**
 * @brief 连接交互事件到状态栏消息（文案在触发时按当前语言取值）。
 */
void InputPage::connectPageSignals() {
	connect(comboBox_, &QComboBox::currentTextChanged, this, [this](const QString& text) {
		emit statusMessage(langText("下拉框选中：", "Combo selected: ") + text);
	});
	connect(lineEditNumber_, &QLineEdit::returnPressed, this, [this] {
		emit statusMessage(langText("数字输入：", "Number input: ") + lineEditNumber_->text());
	});
}

/**
 * @brief 按当前语言覆写页内全部文本并重建下拉项（详见 InputPage.h）。
 */
void InputPage::retranslatePage() {
	groupBoxNumber_->setTitle(langText("数字输入框", "Number input"));
	labelNumberHint_->setText(
	    langText("聚焦自动全选，仅接受 C 语言环境数值（最多 6 位小数）；回车在状态栏回显。",
		         "Selects all on focus; accepts C-locale numbers (max 6 decimals); Enter echoes "
		         "to the status bar."));
	lineEditNumber_->setPlaceholderText(langText("请输入数字", "Enter a number"));
	groupBoxLineEdit_->setTitle(langText("普通输入框", "Text input"));
	lineEditFree_->setPlaceholderText(langText("请输入任意文本", "Enter any text"));
	groupBoxCombo_->setTitle(langText("下拉框", "Combo box"));
	labelComboHint_->setText(
	    langText("选中项实时汇报到状态栏，含长文本选项用于观察溢出。",
		         "Selections are echoed to the status bar; long items observe overflow."));
	comboBox_->clear();
	comboBox_->addItems({langText("选项一", "Option One"), langText("选项二", "Option Two"),
	                     langText("选项三", "Option Three"),
	                     langText("一个很长很长的下拉选项文本用于观察是否超出边界",
	                              "A very long combo item text used to observe overflow")});
}
