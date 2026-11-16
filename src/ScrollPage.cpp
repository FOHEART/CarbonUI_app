/**
 * @file  ScrollPage.cpp
 * @brief 滚动测试页实现：装配 / 交互反馈 / 双语覆写（接口见 ScrollPage.h）。
 * @author FOHEART
 * @date  2026-11-16
 * @version 1.0.0
 *
 * @details
 * 外观全部由 CarbonUI 子库控件自身负责：@c CarbonUIQScrollArea 的构造函数已
 * 应用"无边框 + 内容底色 #3b3b3b + 委托 CarbonUIQScrollBar 刷滚动条"，本页只
 * 用 @c CarbonUIQScrollBar 的构造函数演示竖向细条，用其静态
 * @c applyStyle() 演示给原生 QScrollBar 刷同一套外观（两条条外观一致，便于
 * 目视对照常态 4px / 悬停 8px）。
 *
 * 本源码基于 Mozilla Public License 2.0 发布：
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ScrollPage.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

namespace {
/// objectName 常量：供界面测试与查找使用。
const char* const kNameGroupScrollArea = "groupBoxScrollArea";
const char* const kNameScrollAreaHint = "labelScrollAreaHint";
const char* const kNameScrollAreaDemo = "scrollAreaDemo";
const char* const kNameScrollAreaContents = "scrollAreaDemoContents";
const char* const kNameGroupScrollBar = "groupBoxScrollBar";
const char* const kNameScrollBarHint = "labelScrollBarHint";
const char* const kNameScrollBarVerticalCaption = "labelScrollBarVerticalCaption";
const char* const kNameScrollBarHorizontalCaption = "labelScrollBarHorizontalCaption";
const char* const kNameScrollBarVertical = "scrollBarVertical";
const char* const kNameScrollBarHorizontal = "scrollBarHorizontal";
const char* const kNameScrollBarVerticalValue = "labelScrollBarVerticalValue";
const char* const kNameScrollBarHorizontalValue = "labelScrollBarHorizontalValue";

/// 演示滚动区内容尺寸（像素）：必须大于视口，垂直与水平滚动条才会同时出现。
constexpr int kDemoContentsWidth = 520;
constexpr int kDemoContentsHeight = 360;
/// 演示滚动区固定尺寸（像素）：宽与高都小于内容，两个方向的滚动条才都会出现。
constexpr int kDemoScrollAreaWidth = 420;
constexpr int kDemoScrollAreaHeight = 160;
/// 内容行数：制造足够的高度溢出。
constexpr int kDemoRowCount = 12;
/// 竖直滚动条固定高度 / 水平滚动条固定宽度（像素）；厚度由 QSS 决定。
constexpr int kVerticalBarHeight = 120;
constexpr int kHorizontalBarWidth = 280;
/// 滚动条取值范围与初始取值：初始值不为 0，肉眼即可确认滑块位置生效。
constexpr int kBarMaximum = 100;
constexpr int kBarPageStep = 10;
constexpr int kVerticalBarValue = 30;
constexpr int kHorizontalBarValue = 60;
/// 行标题的最小宽度（像素）：两行标题左对齐。
constexpr int kCaptionWidth = 96;
} // namespace

/**
 * @brief 构造滚动页：装配 -> 交互接线（详见 ScrollPage.h）。
 * @param[in] parent 父控件指针，可为 NULL；通常由主窗口持有。
 */
ScrollPage::ScrollPage(QWidget* parent) : GalleryPage(parent) {
	buildUi();
	connectPageSignals();
}

/**
 * @brief 构建控件树与布局：滚动区分组 + 滚动条分组。
 */
void ScrollPage::buildUi() {
	auto* pageLayout = new QVBoxLayout(this);
	pageLayout->setContentsMargins(12, 12, 12, 12);
	pageLayout->setSpacing(10);

	// 滚动区分组：固定高度的滚动区装一块超出视口的内容
	groupBoxScrollArea_ = new QGroupBox(this);
	groupBoxScrollArea_->setObjectName(QLatin1String(kNameGroupScrollArea));
	auto* areaLayout = new QVBoxLayout(groupBoxScrollArea_);
	areaLayout->setSpacing(8);
	labelScrollAreaHint_ = new QLabel(groupBoxScrollArea_);
	labelScrollAreaHint_->setObjectName(QLatin1String(kNameScrollAreaHint));
	labelScrollAreaHint_->setWordWrap(true);
	areaLayout->addWidget(labelScrollAreaHint_);

	scrollAreaDemo_ = new CarbonUIQScrollArea(groupBoxScrollArea_);
	scrollAreaDemo_->setObjectName(QLatin1String(kNameScrollAreaDemo));
	// 固定视口尺寸（宽高均小于内容）+ 内容保持自身尺寸（不随视口拉伸），
	// 两个方向的溢出才会稳定出现；内容贴左上角而非默认居中
	scrollAreaDemo_->setWidgetResizable(false);
	scrollAreaDemo_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	scrollAreaDemo_->setFixedSize(kDemoScrollAreaWidth, kDemoScrollAreaHeight);
	areaLayout->addWidget(scrollAreaDemo_);

	auto* contents = new QWidget;
	contents->setObjectName(QLatin1String(kNameScrollAreaContents));
	contents->setFixedSize(kDemoContentsWidth, kDemoContentsHeight);
	auto* contentsLayout = new QVBoxLayout(contents);
	contentsLayout->setContentsMargins(12, 12, 12, 12);
	contentsLayout->setSpacing(6);
	contentRows_.reserve(kDemoRowCount);
	for (int i = 0; i < kDemoRowCount; ++i) {
		auto* row = new QLabel(contents);
		contentRows_.append(row);
		contentsLayout->addWidget(row);
	}
	contentsLayout->addSpacerItem(
	    new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
	scrollAreaDemo_->setWidget(contents);

	pageLayout->addWidget(groupBoxScrollArea_);

	// 滚动条分组：竖直条走构造函数，水平条走静态 applyStyle()
	groupBoxScrollBar_ = new QGroupBox(this);
	groupBoxScrollBar_->setObjectName(QLatin1String(kNameGroupScrollBar));
	auto* barLayout = new QVBoxLayout(groupBoxScrollBar_);
	barLayout->setSpacing(8);
	labelScrollBarHint_ = new QLabel(groupBoxScrollBar_);
	labelScrollBarHint_->setObjectName(QLatin1String(kNameScrollBarHint));
	labelScrollBarHint_->setWordWrap(true);
	barLayout->addWidget(labelScrollBarHint_);

	auto* verticalRow = new QHBoxLayout;
	labelScrollBarVerticalCaption_ = new QLabel(groupBoxScrollBar_);
	labelScrollBarVerticalCaption_->setObjectName(QLatin1String(kNameScrollBarVerticalCaption));
	labelScrollBarVerticalCaption_->setMinimumWidth(kCaptionWidth);
	verticalRow->addWidget(labelScrollBarVerticalCaption_);
	scrollBarVertical_ = new CarbonUIQScrollBar(Qt::Vertical, groupBoxScrollBar_);
	scrollBarVertical_->setObjectName(QLatin1String(kNameScrollBarVertical));
	scrollBarVertical_->setRange(0, kBarMaximum);
	scrollBarVertical_->setPageStep(kBarPageStep);
	scrollBarVertical_->setSingleStep(1);
	scrollBarVertical_->setValue(kVerticalBarValue);
	scrollBarVertical_->setFixedHeight(kVerticalBarHeight);
	verticalRow->addWidget(scrollBarVertical_);
	labelScrollBarVerticalValue_ = new QLabel(groupBoxScrollBar_);
	labelScrollBarVerticalValue_->setObjectName(QLatin1String(kNameScrollBarVerticalValue));
	verticalRow->addWidget(labelScrollBarVerticalValue_);
	verticalRow->addSpacerItem(
	    new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	barLayout->addLayout(verticalRow);

	auto* horizontalRow = new QHBoxLayout;
	labelScrollBarHorizontalCaption_ = new QLabel(groupBoxScrollBar_);
	labelScrollBarHorizontalCaption_->setObjectName(QLatin1String(kNameScrollBarHorizontalCaption));
	labelScrollBarHorizontalCaption_->setMinimumWidth(kCaptionWidth);
	horizontalRow->addWidget(labelScrollBarHorizontalCaption_);
	scrollBarHorizontal_ = new QScrollBar(Qt::Horizontal, groupBoxScrollBar_);
	scrollBarHorizontal_->setObjectName(QLatin1String(kNameScrollBarHorizontal));
	// 原生滚动条没有 CarbonUI 外观，静态接口刷同一套 QSS（含垂直 / 水平规则）
	CarbonUIQScrollBar::applyStyle(scrollBarHorizontal_);
	scrollBarHorizontal_->setRange(0, kBarMaximum);
	scrollBarHorizontal_->setPageStep(kBarPageStep);
	scrollBarHorizontal_->setValue(kHorizontalBarValue);
	scrollBarHorizontal_->setFixedWidth(kHorizontalBarWidth);
	horizontalRow->addWidget(scrollBarHorizontal_);
	labelScrollBarHorizontalValue_ = new QLabel(groupBoxScrollBar_);
	labelScrollBarHorizontalValue_->setObjectName(QLatin1String(kNameScrollBarHorizontalValue));
	horizontalRow->addWidget(labelScrollBarHorizontalValue_);
	horizontalRow->addSpacerItem(
	    new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	barLayout->addLayout(horizontalRow);

	pageLayout->addWidget(groupBoxScrollBar_);
	pageLayout->addSpacerItem(
	    new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

/**
 * @brief 连接滚动条取值到回显与状态栏消息（文案在触发时按当前语言取值）。
 */
void ScrollPage::connectPageSignals() {
	connect(scrollBarVertical_, &QScrollBar::valueChanged, this, [this](int value) {
		refreshValueLabels();
		emit statusMessage(langText("竖直滚动条：", "Vertical scroll bar: ") +
		                   QString::number(value));
	});
	connect(scrollBarHorizontal_, &QScrollBar::valueChanged, this, [this](int value) {
		refreshValueLabels();
		emit statusMessage(langText("水平滚动条：", "Horizontal scroll bar: ") +
		                   QString::number(value));
	});
}

/**
 * @brief 按当前取值刷新两个数值标签（取值变化与语言切换共用同一文案模板）。
 */
void ScrollPage::refreshValueLabels() {
	labelScrollBarVerticalValue_->setText(langText("值：", "Value: ") +
	                                      QString::number(scrollBarVertical_->value()));
	labelScrollBarHorizontalValue_->setText(langText("值：", "Value: ") +
	                                        QString::number(scrollBarHorizontal_->value()));
}

/**
 * @brief 按当前语言覆写页内全部文本并重建滚动区内容行（详见 ScrollPage.h）。
 */
void ScrollPage::retranslatePage() {
	groupBoxScrollArea_->setTitle(langText("滚动区", "Scroll area"));
	labelScrollAreaHint_->setText(langText(
	    "CarbonUIQScrollArea：无边框、内容底色 #3b3b3b，垂直与水平滚动条命中范围 "
	    "8px，常态只画中间 4px、鼠标悬停加厚到 8px（拖动内容即可看到两条滚动条同时工作）。",
	    "CarbonUIQScrollArea: borderless with a #3b3b3b content background. Its vertical and "
	    "horizontal bars reserve an 8 px hit area, paint the middle 4 px in normal state and the "
	    "full 8 px on hover (drag the content to drive both bars)."));

	// 内容行为动态文本：每次重翻译重建，行数是固定的
	for (int i = 0; i < contentRows_.size(); ++i) {
		contentRows_.at(i)->setText(langText("条目 ", "Item ") + QString::number(i + 1) +
		                            langText("：滚动区内的固定宽高内容，超出视口即出现滚动条。",
		                                     ": fixed-size content inside the scroll area; the "
		                                     "bars appear once it exceeds the viewport."));
	}

	groupBoxScrollBar_->setTitle(langText("滚动条", "Scroll bars"));
	labelScrollBarHint_->setText(langText(
	    "竖直条用 CarbonUIQScrollBar 的构造函数创建，水平条是原生 QScrollBar 经 "
	    "CarbonUIQScrollBar::applyStyle() 刷同一套外观；鼠标悬停观察加厚，拖动或滚轮改变取值。",
	    "The vertical bar comes from the CarbonUIQScrollBar constructor; the horizontal one is a "
	    "plain QScrollBar styled through CarbonUIQScrollBar::applyStyle(). Hover to see the bar "
	    "thicken, then drag or wheel to change the value."));
	labelScrollBarVerticalCaption_->setText(langText("竖直滚动条", "Vertical bar"));
	labelScrollBarHorizontalCaption_->setText(langText("水平滚动条", "Horizontal bar"));
	refreshValueLabels();
}
