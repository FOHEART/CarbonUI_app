/**
 * @file  SelectionPage.cpp
 * @brief 选择与开关测试页实现：装配 / CarbonUI 风格 / 双语覆写（接口见 SelectionPage.h）。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 *
 * @details
 * 控件树与布局沿用原 MainWindow.ui 中"选择与开关"Tab 的结构。自绘单选的
 * 文字由 CarbonUI 事件过滤器同步到内部标签，可直接 setText；自绘复选框
 * （指示灯居左形态）的文字被清空，这里只更新旁置标签；指示灯居右形态的
 * 文字由控件内部子标签承载，同样直接 setText。旁置标签可点击（eventFilter
 * 转发为复选框 toggle），使两种形态交互一致。复选框分组内的形态测试按钮
 * 会在两种形态间切换（销毁重建），retranslatePage 按当前形态分流覆写。
 *
 * 本源码基于 Mozilla Public License 2.0 发布：
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "SelectionPage.h"

#include "CarbonUIQCheckBox.h"
#include "CarbonUIQPushButton.h"
#include "CarbonUIQRadioButton.h"

#include <QtCore/QEvent>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

namespace {
/// objectName 常量：与原 MainWindow.ui 命名一致，供测试与查找使用。
const char* const kNameGroupRadios = "groupBoxRadios";
const char* const kNameRadioSolo = "radioButtonSolo";
const char* const kNameRadioSegL = "radioButtonSegL";
const char* const kNameRadioSegM = "radioButtonSegM";
const char* const kNameRadioSegR = "radioButtonSegR";
const char* const kNameRadioHint = "labelRadioHint";
const char* const kNameGroupChecks = "groupBoxChecks";
const char* const kNameCheckA = "checkIndicatorA";
const char* const kNameLabelCheckA = "labelCheckA";
const char* const kNameCheckB = "checkIndicatorB";
const char* const kNameLabelCheckB = "labelCheckB";
const char* const kNameCheckNative = "checkBoxNative";
const char* const kNameCheckHint = "labelCheckHint";
const char* const kNameBtnRightFormTest = "buttonRightFormTest";
const char* const kNameGroupChecksRight = "groupBoxChecksRight";
const char* const kNameCheckRightA = "checkIndicatorRightA";
const char* const kNameCheckRightB = "checkIndicatorRightB";
const char* const kNameCheckRightHint = "labelCheckRightHint";
const char* const kNameGroupRadioLong = "groupBoxRadioLong";
const char* const kNameRadioLongCn = "radioButtonLongCn";
const char* const kNameRadioLongEn = "radioButtonLongEn";
} // namespace

/**
 * @brief 构造选择页：装配 -> 风格（详见 SelectionPage.h）。
 * @param[in] parent 父控件指针，可为 NULL；通常由主窗口持有。
 */
SelectionPage::SelectionPage(QWidget* parent) : GalleryPage(parent) {
	buildUi();
	applyStyles();
}

/**
 * @brief 构建控件树与布局：单选分组 + 复选框分组 + 长文本单选分组。
 */
void SelectionPage::buildUi() {
	auto* pageLayout = new QVBoxLayout(this);
	pageLayout->setContentsMargins(12, 12, 12, 12);
	pageLayout->setSpacing(10);

	// 单选按钮分组：独立 + 三段式 + 说明文字
	groupBoxRadios_ = new QGroupBox(this);
	groupBoxRadios_->setObjectName(QLatin1String(kNameGroupRadios));
	auto* radiosLayout = new QVBoxLayout(groupBoxRadios_);
	radiosLayout->setSpacing(8);

	auto* soloLayout = new QHBoxLayout;
	radioButtonSolo_ = new QRadioButton(groupBoxRadios_);
	radioButtonSolo_->setObjectName(QLatin1String(kNameRadioSolo));
	radioButtonSolo_->setMinimumWidth(96);
	soloLayout->addWidget(radioButtonSolo_);
	soloLayout->addSpacerItem(
	    new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	radiosLayout->addLayout(soloLayout);

	auto* segLayout = new QHBoxLayout;
	radioButtonSegL_ = new QRadioButton(groupBoxRadios_);
	radioButtonSegL_->setObjectName(QLatin1String(kNameRadioSegL));
	radioButtonSegL_->setMinimumWidth(72);
	radioButtonSegM_ = new QRadioButton(groupBoxRadios_);
	radioButtonSegM_->setObjectName(QLatin1String(kNameRadioSegM));
	radioButtonSegM_->setMinimumWidth(72);
	radioButtonSegR_ = new QRadioButton(groupBoxRadios_);
	radioButtonSegR_->setObjectName(QLatin1String(kNameRadioSegR));
	radioButtonSegR_->setMinimumWidth(72);
	segLayout->addWidget(radioButtonSegL_);
	segLayout->addWidget(radioButtonSegM_);
	segLayout->addWidget(radioButtonSegR_);
	segLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	radiosLayout->addLayout(segLayout);

	labelRadioHint_ = new QLabel(groupBoxRadios_);
	labelRadioHint_->setObjectName(QLatin1String(kNameRadioHint));
	radiosLayout->addWidget(labelRadioHint_);
	pageLayout->addWidget(groupBoxRadios_);

	// 复选框分组：自绘指示灯 ×2（文案由旁置标签承载）+ 原生对照
	groupBoxChecks_ = new QGroupBox(this);
	groupBoxChecks_->setObjectName(QLatin1String(kNameGroupChecks));
	auto* checksLayout = new QVBoxLayout(groupBoxChecks_);
	checksLayout->setSpacing(8);

	checkRowLayoutA_ = new QHBoxLayout;
	checkIndicatorA_ = new QCheckBox(groupBoxChecks_);
	checkIndicatorA_->setObjectName(QLatin1String(kNameCheckA));
	labelCheckA_ = new QLabel(groupBoxChecks_);
	labelCheckA_->setObjectName(QLatin1String(kNameLabelCheckA));
	// 旁置标签可点击：与指示灯居右形态一致，点文字也能切换勾选状态
	labelCheckA_->installEventFilter(this);
	labelCheckA_->setCursor(Qt::PointingHandCursor);
	checkRowLayoutA_->addWidget(checkIndicatorA_);
	checkRowLayoutA_->addWidget(labelCheckA_);
	checkRowLayoutA_->addSpacerItem(
	    new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	checksLayout->addLayout(checkRowLayoutA_);

	checkRowLayoutB_ = new QHBoxLayout;
	checkIndicatorB_ = new QCheckBox(groupBoxChecks_);
	checkIndicatorB_->setObjectName(QLatin1String(kNameCheckB));
	labelCheckB_ = new QLabel(groupBoxChecks_);
	labelCheckB_->setObjectName(QLatin1String(kNameLabelCheckB));
	labelCheckB_->installEventFilter(this);
	labelCheckB_->setCursor(Qt::PointingHandCursor);
	checkRowLayoutB_->addWidget(checkIndicatorB_);
	checkRowLayoutB_->addWidget(labelCheckB_);
	checkRowLayoutB_->addSpacerItem(
	    new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	checksLayout->addLayout(checkRowLayoutB_);

	checkBoxNative_ = new QCheckBox(groupBoxChecks_);
	checkBoxNative_->setObjectName(QLatin1String(kNameCheckNative));
	checksLayout->addWidget(checkBoxNative_);

	labelCheckHint_ = new QLabel(groupBoxChecks_);
	labelCheckHint_->setObjectName(QLatin1String(kNameCheckHint));
	checksLayout->addWidget(labelCheckHint_);

	// 指示灯形态测试按钮：点击在居左/居右形态间切换（切换即销毁重建）
	auto* testButtonLayout = new QHBoxLayout;
	buttonRightFormTest_ = new QPushButton(groupBoxChecks_);
	buttonRightFormTest_->setObjectName(QLatin1String(kNameBtnRightFormTest));
	testButtonLayout->addWidget(buttonRightFormTest_);
	testButtonLayout->addSpacerItem(
	    new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	checksLayout->addLayout(testButtonLayout);
	connect(buttonRightFormTest_, &QPushButton::clicked, this, [this] { toggleIndicatorForm(); });
	pageLayout->addWidget(groupBoxChecks_);

	// 复选框（指示灯居右）分组：文字在按钮内部左对齐，上下排列时圆点右对齐
	groupBoxChecksRight_ = new QGroupBox(this);
	groupBoxChecksRight_->setObjectName(QLatin1String(kNameGroupChecksRight));
	auto* checksRightLayout = new QVBoxLayout(groupBoxChecksRight_);
	checksRightLayout->setSpacing(8);

	checkIndicatorRightA_ = new QCheckBox(groupBoxChecksRight_);
	checkIndicatorRightA_->setObjectName(QLatin1String(kNameCheckRightA));
	checksRightLayout->addWidget(checkIndicatorRightA_);

	checkIndicatorRightB_ = new QCheckBox(groupBoxChecksRight_);
	checkIndicatorRightB_->setObjectName(QLatin1String(kNameCheckRightB));
	checksRightLayout->addWidget(checkIndicatorRightB_);

	labelCheckRightHint_ = new QLabel(groupBoxChecksRight_);
	labelCheckRightHint_->setObjectName(QLatin1String(kNameCheckRightHint));
	checksRightLayout->addWidget(labelCheckRightHint_);
	pageLayout->addWidget(groupBoxChecksRight_);

	// 长文本单选分组：溢出观察样例
	groupBoxRadioLong_ = new QGroupBox(this);
	groupBoxRadioLong_->setObjectName(QLatin1String(kNameGroupRadioLong));
	auto* longLayout = new QVBoxLayout(groupBoxRadioLong_);
	longLayout->setSpacing(8);

	auto* longCnLayout = new QHBoxLayout;
	radioButtonLongCn_ = new QRadioButton(groupBoxRadioLong_);
	radioButtonLongCn_->setObjectName(QLatin1String(kNameRadioLongCn));
	longCnLayout->addWidget(radioButtonLongCn_);
	longCnLayout->addSpacerItem(
	    new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	longLayout->addLayout(longCnLayout);

	auto* longEnLayout = new QHBoxLayout;
	radioButtonLongEn_ = new QRadioButton(groupBoxRadioLong_);
	radioButtonLongEn_->setObjectName(QLatin1String(kNameRadioLongEn));
	longEnLayout->addWidget(radioButtonLongEn_);
	longEnLayout->addSpacerItem(
	    new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	longLayout->addLayout(longEnLayout);
	pageLayout->addWidget(groupBoxRadioLong_);

	pageLayout->addSpacerItem(
	    new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

/**
 * @brief 应用 CarbonUI 选择类风格：自绘圆角单选 + 自绘指示灯复选框（非幂等）。
 */
void SelectionPage::applyStyles() {
	CarbonUIQRadioButton::applyStyle(radioButtonSolo_, CarbonUIQRadioButton::Radio_FourRound, 96);
	CarbonUIQRadioButton::applyStyle(radioButtonSegL_, CarbonUIQRadioButton::Radio_LeftRound, 72);
	CarbonUIQRadioButton::applyStyle(radioButtonSegM_, CarbonUIQRadioButton::Radio_Rectangle, 72);
	CarbonUIQRadioButton::applyStyle(radioButtonSegR_, CarbonUIQRadioButton::Radio_RightRound, 72);
	CarbonUIQRadioButton::applyStyle(radioButtonLongCn_, CarbonUIQRadioButton::Radio_FourRound,
	                                 120);
	CarbonUIQRadioButton::applyStyle(radioButtonLongEn_, CarbonUIQRadioButton::Radio_FourRound,
	                                 120);
	CarbonUIQCheckBox::applyStyle(checkIndicatorA_);
	CarbonUIQCheckBox::applyStyle(checkIndicatorB_);
	CarbonUIQCheckBox::applyStyle(checkIndicatorRightA_, CarbonUIQCheckBox::Check_IndicatorRight);
	CarbonUIQCheckBox::applyStyle(checkIndicatorRightB_, CarbonUIQCheckBox::Check_IndicatorRight);
	CarbonUIQPushButton::applyStyle(buttonRightFormTest_, CarbonUIQPushButton::Shape_RoundAll);
}

/**
 * @brief 按当前语言覆写页内全部文本（详见 SelectionPage.h）。
 */
void SelectionPage::retranslatePage() {
	groupBoxRadios_->setTitle(langText("单选按钮", "Radio buttons"));
	radioButtonSolo_->setText(langText("独立按钮", "Standalone"));
	radioButtonSegL_->setText(langText("左段", "Left"));
	radioButtonSegM_->setText(langText("中段", "Middle"));
	radioButtonSegR_->setText(langText("右段", "Right"));
	labelRadioHint_->setText(
	    langText("CarbonUI 自绘圆角单选：文字居中显示在按钮内部。",
		         "CarbonUI custom rounded radios: text is centered inside the button."));
	groupBoxChecks_->setTitle(langText("复选框", "Check boxes"));
	if (indicatorRightForm_) {
		// 居右形态：文案由控件内部子标签承载，旁置标签隐藏
		checkIndicatorA_->setText(langText("启用自绘指示灯", "Enable custom indicator"));
		checkIndicatorB_->setText(langText("开机自动连接", "Auto-connect on startup"));
	} else {
		// 居左形态：文案由旁置标签承载
		labelCheckA_->setText(langText("启用自绘指示灯", "Enable custom indicator"));
		labelCheckB_->setText(langText("开机自动连接", "Auto-connect on startup"));
	}
	buttonRightFormTest_->setText(
	    indicatorRightForm_ ? langText("恢复指示灯居左形态", "Restore indicator on the left")
		                    : langText("测试指示灯居右形态", "Test indicator on the right"));
	checkBoxNative_->setText(langText("原生复选框对照", "Native check box (reference)"));
	labelCheckHint_->setText(
	    langText("自绘指示灯复选框只显示 16x16 圆点，点击文字同样可切换勾选状态。",
		         "Custom indicator check boxes show a 16x16 dot only; clicking the caption "
		         "toggles the state as well."));
	groupBoxChecksRight_->setTitle(
	    langText("复选框（指示灯居右）", "Check boxes (indicator on the right)"));
	checkIndicatorRightA_->setText(langText("启用智慧指示灯", "Enable smart indicator"));
	checkIndicatorRightB_->setText(langText("记住窗口位置", "Remember window position"));
	labelCheckRightHint_->setText(
	    langText("指示灯居右形态：文字在按钮内部左对齐，多项上下排列时圆点右对齐。",
		         "Right-side indicator variant: captions are left-aligned inside the button; "
		         "stacked check boxes align their dots on the right."));
	groupBoxRadioLong_->setTitle(
	    langText("长文本单选（观察是否溢出）", "Long-text radios (observe overflow)"));
	radioButtonLongCn_->setText(longTextZh());
	radioButtonLongEn_->setText(longTextEn());
}

/**
 * @brief 指示灯形态测试按钮：在居左/居右形态间切换本分组两个自绘复选框。
 *
 * @details
 * CarbonUI 的 applyCheckStyle 非幂等（重复调用会叠加子控件与布局），因此
 * 切换即销毁重建：旧控件经 deleteLater 释放，新控件以相同 objectName 插回
 * 原行布局首列；勾选状态予以保留。旁置文案标签仅在居左形态下显示，居右
 * 形态下文案由控件内部子标签承载（retranslatePage 按形态分流覆写）。
 */
void SelectionPage::toggleIndicatorForm() {
	const bool toRight = !indicatorRightForm_;
	// 两个自绘复选框由 buildUi 创建，点击按钮时必然存在
	const bool checkedA = checkIndicatorA_->isChecked();
	const bool checkedB = checkIndicatorB_->isChecked();
	const auto position =
	    toRight ? CarbonUIQCheckBox::Check_IndicatorRight : CarbonUIQCheckBox::Check_IndicatorLeft;

	// A 行：销毁旧控件，新控件以相同 objectName 插回行首并恢复勾选状态
	checkRowLayoutA_->removeWidget(checkIndicatorA_);
	checkIndicatorA_->deleteLater();
	checkIndicatorA_ = new QCheckBox(groupBoxChecks_);
	checkIndicatorA_->setObjectName(QLatin1String(kNameCheckA));
	checkRowLayoutA_->insertWidget(0, checkIndicatorA_);
	CarbonUIQCheckBox::applyStyle(checkIndicatorA_, position);
	checkIndicatorA_->setChecked(checkedA);

	// B 行同理
	checkRowLayoutB_->removeWidget(checkIndicatorB_);
	checkIndicatorB_->deleteLater();
	checkIndicatorB_ = new QCheckBox(groupBoxChecks_);
	checkIndicatorB_->setObjectName(QLatin1String(kNameCheckB));
	checkRowLayoutB_->insertWidget(0, checkIndicatorB_);
	CarbonUIQCheckBox::applyStyle(checkIndicatorB_, position);
	checkIndicatorB_->setChecked(checkedB);

	// 旁置标签只服务居左形态；居右形态下隐藏，避免与控件内子标签重复
	labelCheckA_->setVisible(!toRight);
	labelCheckB_->setVisible(!toRight);
	indicatorRightForm_ = toRight;

	retranslatePage();
}

/**
 * @brief 事件过滤器：旁置文案标签被点击时切换对应复选框的勾选状态。
 *
 * @details
 * 指示灯居左形态下，复选框本体只剩 16x16 的指示灯，点击命中区太小；
 * 把文案标签的按下事件转发为复选框 toggle()，使两种形态交互一致
 * （与指示灯居右形态"点文字即可切换"相同）。事件消费掉，不再下传。
 */
bool SelectionPage::eventFilter(QObject* watched, QEvent* event) {
	if (event->type() == QEvent::MouseButtonPress) {
		QCheckBox* target = nullptr;
		if (watched == labelCheckA_) {
			target = checkIndicatorA_;
		} else if (watched == labelCheckB_) {
			target = checkIndicatorB_;
		}
		if (target != nullptr) {
			target->toggle();
			return true;
		}
	}
	return GalleryPage::eventFilter(watched, event);
}
