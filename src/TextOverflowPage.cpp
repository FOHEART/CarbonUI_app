/**
 * @file  TextOverflowPage.cpp
 * @brief 文本溢出测试页实现：装配 / CarbonUI 风格 / 双语覆写（接口见 TextOverflowPage.h）。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 *
 * @details
 * 控件树与布局沿用原 MainWindow.ui 中"文本溢出"Tab 的结构：滚动容器 +
 * 逐行"类别说明 + 固定尺寸观察对象"。固定尺寸在装配阶段用 setMinimum/
 * setMaximum 写死，不依赖布局推算，保证观察条件稳定。
 *
 * 本源码基于 Mozilla Public License 2.0 发布：
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "TextOverflowPage.h"

#include "CarbonUIClass.h"

#include <QtWidgets/QVBoxLayout>

namespace {
/// objectName 常量：与原 MainWindow.ui 命名一致，供测试与查找使用。
const char* const kNameScrollArea = "scrollAreaOverflow";
const char* const kNameOvTitle = "labelOvTitle";
const char* const kNameOvBtnCn = "labelOvBtnCn";
const char* const kNameOvButtonCn = "pushButtonOvCn";
const char* const kNameOvBtnEn = "labelOvBtnEn";
const char* const kNameOvButtonEn = "pushButtonOvEn";
const char* const kNameOvRadio = "labelOvRadio";
const char* const kNameOvRadioBtn = "radioButtonOv";
const char* const kNameOvCheckCaption = "labelOvCheckCaption";
const char* const kNameOvCheck = "checkIndicatorOv";
const char* const kNameOvCheckLabel = "labelOvCheck";
const char* const kNameOvLineEdit = "labelOvLineEdit";
const char* const kNameOvLineEditText = "lineEditOv";
const char* const kNameOvCombo = "labelOvCombo";
const char* const kNameOvComboBox = "comboBoxOv";
const char* const kNameOvWrapCaption = "labelOvWrapCaption";
const char* const kNameOvWrap = "labelOvWrap";
const char* const kNameOvNoWrapCaption = "labelOvNoWrapCaption";
const char* const kNameOvNoWrap = "labelOvNoWrap";
} // namespace

/**
 * @brief 构造溢出页：装配 -> 风格（详见 TextOverflowPage.h）。
 * @param[in] parent 父控件指针，可为 NULL；通常由主窗口持有。
 */
TextOverflowPage::TextOverflowPage(QWidget* parent) : GalleryPage(parent) {
	buildUi();
	applyStyles();
}

/**
 * @brief 创建单行观察条目的标题部分：标题标签（最小宽 140）+ 弹簧。
 *
 * @details 标题标签先插入行布局，观察对象由调用方插在标题与弹簧之间。
 *
 * @param[in] pageLayout  页面主布局，行布局完成后再加入其中，不可为 NULL。
 * @param[in] captionName 标题标签 objectName，不可为 NULL。
 * @param[out] captionLabel 标题标签指针回传给调用方用于覆写文本，不可为 NULL。
 * @return 行布局指针（已含标题与弹簧，尚未加入页面布局）。
 */
QHBoxLayout* TextOverflowPage::addOverflowRow(QVBoxLayout* pageLayout, const char* captionName,
                                              QLabel** captionLabel) {
	auto* row = new QHBoxLayout;
	auto* caption = new QLabel(this);
	caption->setObjectName(QLatin1String(captionName));
	caption->setMinimumWidth(140);
	*captionLabel = caption;
	row->addWidget(caption);
	row->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	pageLayout->addLayout(row);
	return row;
}

/**
 * @brief 构建控件树与布局：滚动容器 + 逐行固定尺寸观察对象。
 */
void TextOverflowPage::buildUi() {
	auto* pageLayout = new QVBoxLayout(this);
	pageLayout->setContentsMargins(0, 0, 0, 0);

	scrollArea_ = new QScrollArea(this);
	scrollArea_->setObjectName(QLatin1String(kNameScrollArea));
	scrollArea_->setWidgetResizable(true);
	pageLayout->addWidget(scrollArea_);

	auto* contents = new QWidget(scrollArea_);
	contents->setObjectName(QStringLiteral("scrollAreaOverflowContents"));
	auto* contentsLayout = new QVBoxLayout(contents);
	contentsLayout->setContentsMargins(12, 12, 12, 12);
	contentsLayout->setSpacing(10);
	scrollArea_->setWidget(contents);

	labelOvTitle_ = new QLabel(contents);
	labelOvTitle_->setObjectName(QLatin1String(kNameOvTitle));
	labelOvTitle_->setWordWrap(true);
	contentsLayout->addWidget(labelOvTitle_);

	// 按钮（中文 / 英文）：固定 96x28
	QHBoxLayout* row = addOverflowRow(contentsLayout, kNameOvBtnCn, &labelOvBtnCn_);
	pushButtonOvCn_ = new QPushButton(contents);
	pushButtonOvCn_->setObjectName(QLatin1String(kNameOvButtonCn));
	pushButtonOvCn_->setMinimumSize(96, 28);
	pushButtonOvCn_->setMaximumSize(96, 28);
	row->insertWidget(1, pushButtonOvCn_);

	row = addOverflowRow(contentsLayout, kNameOvBtnEn, &labelOvBtnEn_);
	pushButtonOvEn_ = new QPushButton(contents);
	pushButtonOvEn_->setObjectName(QLatin1String(kNameOvButtonEn));
	pushButtonOvEn_->setMinimumSize(96, 28);
	pushButtonOvEn_->setMaximumSize(96, 28);
	row->insertWidget(1, pushButtonOvEn_);

	// 单选
	row = addOverflowRow(contentsLayout, kNameOvRadio, &labelOvRadio_);
	radioButtonOv_ = new QRadioButton(contents);
	radioButtonOv_->setObjectName(QLatin1String(kNameOvRadioBtn));
	row->insertWidget(1, radioButtonOv_);

	// 复选框：自绘指示灯 + 长标签（固定宽 120）
	row = addOverflowRow(contentsLayout, kNameOvCheckCaption, &labelOvCheckCaption_);
	checkIndicatorOv_ = new QCheckBox(contents);
	checkIndicatorOv_->setObjectName(QLatin1String(kNameOvCheck));
	labelOvCheck_ = new QLabel(contents);
	labelOvCheck_->setObjectName(QLatin1String(kNameOvCheckLabel));
	labelOvCheck_->setMinimumWidth(120);
	labelOvCheck_->setMaximumWidth(120);
	row->insertWidget(1, checkIndicatorOv_);
	row->insertWidget(2, labelOvCheck_);

	// 输入框（固定宽 120）
	row = addOverflowRow(contentsLayout, kNameOvLineEdit, &labelOvLineEdit_);
	lineEditOv_ = new QLineEdit(contents);
	lineEditOv_->setObjectName(QLatin1String(kNameOvLineEditText));
	lineEditOv_->setMinimumWidth(120);
	lineEditOv_->setMaximumWidth(120);
	row->insertWidget(1, lineEditOv_);

	// 下拉框（固定宽 110）
	row = addOverflowRow(contentsLayout, kNameOvCombo, &labelOvCombo_);
	comboBoxOv_ = new QComboBox(contents);
	comboBoxOv_->setObjectName(QLatin1String(kNameOvComboBox));
	comboBoxOv_->setMinimumWidth(110);
	comboBoxOv_->setMaximumWidth(110);
	row->insertWidget(1, comboBoxOv_);

	// 标签：自动换行（固定宽 160）
	row = addOverflowRow(contentsLayout, kNameOvWrapCaption, &labelOvWrapCaption_);
	labelOvWrap_ = new QLabel(contents);
	labelOvWrap_->setObjectName(QLatin1String(kNameOvWrap));
	labelOvWrap_->setMinimumWidth(160);
	labelOvWrap_->setMaximumWidth(160);
	labelOvWrap_->setWordWrap(true);
	row->insertWidget(1, labelOvWrap_);

	// 标签：不换行（固定宽 160）
	row = addOverflowRow(contentsLayout, kNameOvNoWrapCaption, &labelOvNoWrapCaption_);
	labelOvNoWrap_ = new QLabel(contents);
	labelOvNoWrap_->setObjectName(QLatin1String(kNameOvNoWrap));
	labelOvNoWrap_->setMinimumWidth(160);
	labelOvNoWrap_->setMaximumWidth(160);
	row->insertWidget(1, labelOvNoWrap_);

	contentsLayout->addSpacerItem(
	    new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

/**
 * @brief 应用 CarbonUI 风格：滚动区、按钮、单选、复选指示灯、下拉框。
 */
void TextOverflowPage::applyStyles() {
	CarbonUIClass::applyScrollAreaStyle(scrollArea_);
	CarbonUIClass::applyButtonStyle(pushButtonOvCn_, CarbonUIClass::Shape_RoundAll);
	CarbonUIClass::applyButtonStyle(pushButtonOvEn_, CarbonUIClass::Shape_RoundAll);
	CarbonUIClass::applyRadioStyle(radioButtonOv_, CarbonUIClass::Radio_FourRound, 96);
	CarbonUIClass::applyCheckStyle(checkIndicatorOv_);
	CarbonUIClass::setupComboBoxStyle(comboBoxOv_);
}

/**
 * @brief 按当前语言覆写页内全部文本并重建下拉项（详见 TextOverflowPage.h）。
 */
void TextOverflowPage::retranslatePage() {
	labelOvTitle_->setText(
	    langText("以下控件为固定尺寸，长中英文文本用于观察是否超出边界（截断 / 省略 / 滚动）。"
		         "切换语言后逐一检查。",
		         "The widgets below have fixed sizes with long CN/EN texts to observe clipping / "
		         "eliding / scrolling."));
	labelOvBtnCn_->setText(langText("按钮 · 中文长文本", "Button · long CN"));
	pushButtonOvCn_->setText(langText("确定参数设置", "确定参数设置"));
	labelOvBtnEn_->setText(langText("按钮 · 英文长文本", "Button · long EN"));
	pushButtonOvEn_->setText(langText("确定参数设置", "Apply Settings"));
	labelOvRadio_->setText(langText("单选 · 长文本", "Radio · long text"));
	radioButtonOv_->setText(langText("确定参数设置", "Apply Settings"));
	labelOvCheckCaption_->setText(langText("复选框 · 长标签", "Check box · long label"));
	labelOvCheck_->setText(
	    langText("开机时启用自动重连功能", "Enable automatic reconnection on startup"));
	labelOvLineEdit_->setText(langText("输入框 · 长文本", "Line edit · long text"));
	lineEditOv_->setText(
	    langText("确定参数设置并保存到配置文件", "Apply settings and save to the config file"));
	labelOvCombo_->setText(langText("下拉框 · 长选项", "Combo · long items"));
	comboBoxOv_->clear();
	comboBoxOv_->addItems(
	    {langText("确定参数设置", "Apply Settings"), langText("恢复默认参数", "Restore Defaults"),
		 langText("一个很长很长的下拉选项用于观察溢出", "A very long item to observe overflow")});
	labelOvWrapCaption_->setText(langText("标签 · 自动换行", "Label · wrap"));
	labelOvWrap_->setText(
	    langText("这是一段很长的中文说明文本，用于观察自动换行在固定宽度内的表现。",
		         "A long caption to observe word wrapping inside a fixed width."));
	labelOvNoWrapCaption_->setText(langText("标签 · 不换行", "Label · no wrap"));
	labelOvNoWrap_->setText(langText("这是一段不换行的长文本用于观察省略表现。",
	                                 "A long non-wrapping label text to observe eliding."));
}
