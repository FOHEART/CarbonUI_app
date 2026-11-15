/**
 * @file  DialogPage.cpp
 * @brief 对话框测试页实现：装配 / CarbonUI 风格 / 弹出「测试」对话框（接口见 DialogPage.h）。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 *
 * @details
 * 页面只有一个分组：说明文字 + 打开按钮。对话框本体由 CarbonUI 子库的
 * CarbonUIDialog 负责（无边框圆角面板 + 标题条 + 键值行），本页只提供
 * 演示数据；数据按当前语言取标签、取值保持产品化的英文原样。
 *
 * 本源码基于 Mozilla Public License 2.0 发布：
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "DialogPage.h"

#include "CarbonUIClass.h"
#include "CarbonUIDialog.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

namespace {
/// objectName 常量：供界面测试与查找使用。
const char* const kNameGroupDialogs = "groupBoxDialogs";
const char* const kNameDialogHint = "labelDialogHint";
const char* const kNameOpenDialog = "pushButtonOpenDialog";

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
 * @brief 构建控件树与布局：一个"对话框"分组（说明 + 按钮）。
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

	auto* buttonLayout = new QHBoxLayout;
	pushButtonOpenDialog_ = new QPushButton(groupBoxDialogs_);
	pushButtonOpenDialog_->setObjectName(QLatin1String(kNameOpenDialog));
	buttonLayout->addWidget(pushButtonOpenDialog_);
	buttonLayout->addSpacerItem(
	    new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	groupLayout->addLayout(buttonLayout);

	pageLayout->addWidget(groupBoxDialogs_);
	pageLayout->addSpacerItem(
	    new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

	// 点按钮弹出模态对话框；对话框自身是堆对象，exec 返回后释放
	connect(pushButtonOpenDialog_, &QPushButton::clicked, this, [this] {
		QDialog* dialog = createTestDialog(this);
		dialog->exec();
		dialog->deleteLater();
	});
}

/**
 * @brief 应用 CarbonUI 按钮风格：打开按钮用四角圆角主色样式。
 */
void DialogPage::applyStyles() {
	CarbonUIClass::applyButtonStyle(pushButtonOpenDialog_, CarbonUIClass::Shape_RoundAll);
}

/**
 * @brief 构造「测试」对话框（详见 DialogPage.h）。
 * @param[in] parent 对话框父窗口，可为 NULL。
 * @return 新建的 CarbonUIDialog，四行信息已填充。
 */
QDialog* DialogPage::createTestDialog(QWidget* parent) const {
	auto* dialog = new CarbonUIDialog(langText("测试", "Test dialog"), parent);
	dialog->addInfoRow(langText("应用修订号", "Application revision number"),
	                   QLatin1String(kRevision));
	dialog->addInfoRow(langText("应用构建时间", "Application date"), QLatin1String(kBuildDate));
	dialog->addInfoRow(langText("程序版本", "Program version"), QLatin1String(kProgramVersion));
	dialog->addInfoRow(langText("许可证", "License"), QLatin1String(kLicense));
	return dialog;
}

/**
 * @brief 按当前语言覆写页内全部文本（详见 DialogPage.h）。
 */
void DialogPage::retranslatePage() {
	groupBoxDialogs_->setTitle(langText("对话框", "Dialogs"));
	labelDialogHint_->setText(langText(
	    "CarbonUI 深色对话框：无系统边框、圆角面板、黑色标题条与键值行正文。",
	    "CarbonUI dark dialog: frameless rounded panel with a black title bar and key/value "
	    "rows."));
	pushButtonOpenDialog_->setText(
	    langText("打开「测试」对话框", "Open the \"Test dialog\" dialog"));
}
