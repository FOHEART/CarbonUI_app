/**
 * @file  ButtonPage.cpp
 * @brief 按钮测试页实现：装配 / CarbonUI 风格 / 点击反馈 / 双语覆写（接口见 ButtonPage.h）。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 *
 * @details
 * 控件树与布局沿用原 MainWindow.ui 中"按钮"Tab 的结构：滚动容器 +
 * 两个分组框。CarbonUI 风格在此处应用（构造一次），主窗口不再感知页面
 * 内部控件。
 *
 * 本源码基于 Mozilla Public License 2.0 发布：
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ButtonPage.h"

#include "CarbonUIClass.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

namespace {
/// objectName 常量：与原 MainWindow.ui 命名一致，供测试与查找使用。
const char* const kNameScrollArea = "scrollArea";
const char* const kNameGroupButtons = "groupBoxButtons";
const char* const kNamePrimary = "pushButton";
const char* const kNameSegL = "pushButton_7";
const char* const kNameSegM = "pushButton_9";
const char* const kNameSegR = "pushButton_10";
const char* const kNameHint = "labelSegButtonsHint";
const char* const kNameGroupLong = "groupBoxButtonLong";
const char* const kNameLongCn = "pushButtonLongCn";
const char* const kNameLongEn = "pushButtonLongEn";
} // namespace

/**
 * @brief 构造按钮页：装配 -> 风格 -> 信号（详见 ButtonPage.h）。
 * @param[in] parent 父控件指针，可为 NULL；通常由主窗口持有。
 */
ButtonPage::ButtonPage(QWidget* parent) : GalleryPage(parent) {
	buildUi();
	applyStyles();
	connectPageSignals();
}

/**
 * @brief 构建控件树与布局：滚动容器 + 按钮样式分组 + 长文本分组。
 */
void ButtonPage::buildUi() {
	auto* pageLayout = new QVBoxLayout(this);
	pageLayout->setContentsMargins(0, 0, 0, 0);

	scrollArea_ = new QScrollArea(this);
	scrollArea_->setObjectName(QLatin1String(kNameScrollArea));
	scrollArea_->setWidgetResizable(true);
	pageLayout->addWidget(scrollArea_);

	auto* contents = new QWidget(scrollArea_);
	contents->setObjectName(QStringLiteral("scrollAreaButtonsContents"));
	auto* contentsLayout = new QVBoxLayout(contents);
	contentsLayout->setContentsMargins(12, 12, 12, 12);
	contentsLayout->setSpacing(10);
	scrollArea_->setWidget(contents);

	// 按钮样式分组：主色按钮 + 三段式按钮组 + 说明文字
	groupBoxButtons_ = new QGroupBox(contents);
	groupBoxButtons_->setObjectName(QLatin1String(kNameGroupButtons));
	auto* groupLayout = new QVBoxLayout(groupBoxButtons_);
	groupLayout->setSpacing(8);

	pushButton_ = new QPushButton(groupBoxButtons_);
	pushButton_->setObjectName(QLatin1String(kNamePrimary));
	groupLayout->addWidget(pushButton_);

	auto* segLayout = new QHBoxLayout;
	pushButtonSegL_ = new QPushButton(groupBoxButtons_);
	pushButtonSegL_->setObjectName(QLatin1String(kNameSegL));
	pushButtonSegL_->setMinimumWidth(72);
	pushButtonSegM_ = new QPushButton(groupBoxButtons_);
	pushButtonSegM_->setObjectName(QLatin1String(kNameSegM));
	pushButtonSegM_->setMinimumWidth(72);
	pushButtonSegR_ = new QPushButton(groupBoxButtons_);
	pushButtonSegR_->setObjectName(QLatin1String(kNameSegR));
	pushButtonSegR_->setMinimumWidth(72);
	segLayout->addWidget(pushButtonSegL_);
	segLayout->addWidget(pushButtonSegM_);
	segLayout->addWidget(pushButtonSegR_);
	segLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	groupLayout->addLayout(segLayout);

	labelSegButtonsHint_ = new QLabel(groupBoxButtons_);
	labelSegButtonsHint_->setObjectName(QLatin1String(kNameHint));
	groupLayout->addWidget(labelSegButtonsHint_);
	contentsLayout->addWidget(groupBoxButtons_);

	// 长文本按钮分组：溢出观察样例
	groupBoxButtonLong_ = new QGroupBox(contents);
	groupBoxButtonLong_->setObjectName(QLatin1String(kNameGroupLong));
	auto* longLayout = new QVBoxLayout(groupBoxButtonLong_);
	longLayout->setSpacing(8);
	pushButtonLongCn_ = new QPushButton(groupBoxButtonLong_);
	pushButtonLongCn_->setObjectName(QLatin1String(kNameLongCn));
	pushButtonLongEn_ = new QPushButton(groupBoxButtonLong_);
	pushButtonLongEn_->setObjectName(QLatin1String(kNameLongEn));
	longLayout->addWidget(pushButtonLongCn_);
	longLayout->addWidget(pushButtonLongEn_);
	contentsLayout->addWidget(groupBoxButtonLong_);

	contentsLayout->addSpacerItem(
	    new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

/**
 * @brief 应用 CarbonUI 按钮风格：独立圆角 + 分段（左圆/矩形/右圆）+ 长文本。
 */
void ButtonPage::applyStyles() {
	CarbonUIClass::applyScrollAreaStyle(scrollArea_);
	CarbonUIClass::applyButtonStyle(pushButton_, CarbonUIClass::Shape_RoundAll);
	CarbonUIClass::applyButtonStyle(pushButtonSegL_, CarbonUIClass::Shape_LeftRound);
	CarbonUIClass::applyButtonStyle(pushButtonSegM_, CarbonUIClass::Shape_Rectangle);
	CarbonUIClass::applyButtonStyle(pushButtonSegR_, CarbonUIClass::Shape_RightRound);
	CarbonUIClass::applyButtonStyle(pushButtonLongCn_, CarbonUIClass::Shape_RoundAll);
	CarbonUIClass::applyButtonStyle(pushButtonLongEn_, CarbonUIClass::Shape_RoundAll);
}

/**
 * @brief 连接按钮点击到状态栏消息（文案在触发时按当前语言取值）。
 */
void ButtonPage::connectPageSignals() {
	connect(pushButton_, &QPushButton::clicked, this,
	        [this] { emit statusMessage(langText("主色按钮被点击", "Primary button clicked")); });
	connect(pushButtonSegL_, &QPushButton::clicked, this, [this] {
		emit statusMessage(langText("分段按钮[左段]被点击", "Segmented button [left] clicked"));
	});
	connect(pushButtonSegM_, &QPushButton::clicked, this, [this] {
		emit statusMessage(langText("分段按钮[中段]被点击", "Segmented button [middle] clicked"));
	});
	connect(pushButtonSegR_, &QPushButton::clicked, this, [this] {
		emit statusMessage(langText("分段按钮[右段]被点击", "Segmented button [right] clicked"));
	});
}

/**
 * @brief 按当前语言覆写页内全部文本（详见 ButtonPage.h）。
 */
void ButtonPage::retranslatePage() {
	groupBoxButtons_->setTitle(langText("按钮样式", "Button styles"));
	pushButton_->setText(langText("主色按钮", "Primary Button"));
	pushButtonSegL_->setText(langText("左段", "Left"));
	pushButtonSegM_->setText(langText("中段", "Middle"));
	pushButtonSegR_->setText(langText("右段", "Right"));
	labelSegButtonsHint_->setText(
	    langText("分段按钮组：左圆 / 矩形 / 右圆，点击后在状态栏汇报。",
		         "Segmented buttons: left-rounded / square / right-rounded; clicks are reported "
		         "in the status bar."));
	groupBoxButtonLong_->setTitle(
	    langText("长文本按钮（观察是否溢出）", "Long-text buttons (observe overflow)"));
	pushButtonLongCn_->setText(longTextZh());
	pushButtonLongEn_->setText(longTextEn());
}
