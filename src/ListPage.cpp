/**
 * @file  ListPage.cpp
 * @brief 列表与折叠测试页实现：装配 / CarbonUI 风格 / 交互反馈 / 双语覆写（接口见 ListPage.h）。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 *
 * @details
 * 控件树与布局沿用原 MainWindow.ui 中"列表与折叠"Tab 的结构。树条目为
 * 动态内容：每次 retranslatePage 清空重建，折叠器的展开状态不受影响
 * （折叠器接管的是分组框标题栏，与内容无关）。
 *
 * 本源码基于 Mozilla Public License 2.0 发布：
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ListPage.h"

#include "CarbonUIQTreeWidget.h"
#include "groupboxcollapser.h"

#include <QtWidgets/QVBoxLayout>

namespace {
/// objectName 常量：与原 MainWindow.ui 命名一致，供测试与查找使用。
const char* const kNameTreeHint = "labelTreeHint";
const char* const kNameTree = "treeWidget";
const char* const kNameCollapseA = "groupBoxCollapseA";
const char* const kNameCollapseA1 = "labelCollapseA1";
const char* const kNameCollapseB = "groupBoxCollapseB";
const char* const kNameCollapseB1 = "labelCollapseB1";
} // namespace

/**
 * @brief 构造列表页：装配 -> 风格 -> 信号（详见 ListPage.h）。
 * @param[in] parent 父控件指针，可为 NULL；通常由主窗口持有。
 */
ListPage::ListPage(QWidget* parent) : GalleryPage(parent) {
	buildUi();
	applyStyles();
	connectPageSignals();
}

/**
 * @brief 构建控件树与布局：说明文字 + 树 + 两个可折叠分组框。
 */
void ListPage::buildUi() {
	auto* pageLayout = new QVBoxLayout(this);
	pageLayout->setContentsMargins(12, 12, 12, 12);
	pageLayout->setSpacing(10);

	labelTreeHint_ = new QLabel(this);
	labelTreeHint_->setObjectName(QLatin1String(kNameTreeHint));
	pageLayout->addWidget(labelTreeHint_);

	treeWidget_ = new QTreeWidget(this);
	treeWidget_->setObjectName(QLatin1String(kNameTree));
	treeWidget_->setColumnCount(2);
	pageLayout->addWidget(treeWidget_);

	// 可折叠面板 A / B：内容为说明文字（折叠器接管标题栏）
	groupBoxCollapseA_ = new QGroupBox(this);
	groupBoxCollapseA_->setObjectName(QLatin1String(kNameCollapseA));
	auto* collapseALayout = new QVBoxLayout(groupBoxCollapseA_);
	labelCollapseA1_ = new QLabel(groupBoxCollapseA_);
	labelCollapseA1_->setObjectName(QLatin1String(kNameCollapseA1));
	labelCollapseA1_->setWordWrap(true);
	collapseALayout->addWidget(labelCollapseA1_);
	pageLayout->addWidget(groupBoxCollapseA_);

	groupBoxCollapseB_ = new QGroupBox(this);
	groupBoxCollapseB_->setObjectName(QLatin1String(kNameCollapseB));
	auto* collapseBLayout = new QVBoxLayout(groupBoxCollapseB_);
	labelCollapseB1_ = new QLabel(groupBoxCollapseB_);
	labelCollapseB1_->setObjectName(QLatin1String(kNameCollapseB1));
	labelCollapseB1_->setWordWrap(true);
	collapseBLayout->addWidget(labelCollapseB1_);
	pageLayout->addWidget(groupBoxCollapseB_);

	pageLayout->addSpacerItem(
	    new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

/**
 * @brief 应用 CarbonUI 树风格并安装折叠器：A 默认展开、B 默认折叠（非幂等）。
 */
void ListPage::applyStyles() {
	CarbonUIQTreeWidget::applyStyle(treeWidget_);
	QGroupBoxCollapser::installIntoGroupBox(groupBoxCollapseA_, nullptr, true);
	QGroupBoxCollapser::installIntoGroupBox(groupBoxCollapseB_, nullptr, false);
}

/**
 * @brief 连接树条目点击到状态栏消息（直接回显条目文本）。
 */
void ListPage::connectPageSignals() {
	connect(treeWidget_, &QTreeWidget::itemClicked, this,
	        [this](QTreeWidgetItem* item, int column) { emit statusMessage(item->text(column)); });
}

/**
 * @brief 按当前语言覆写页内全部文本并重建树条目（详见 ListPage.h）。
 */
void ListPage::retranslatePage() {
	labelTreeHint_->setText(
	    langText("树控件：表头加粗、缩进压平，点击条目在状态栏汇报。",
		         "Tree: bold header, flattened indent; item clicks are echoed to the status "
		         "bar."));
	treeWidget_->setHeaderLabels({langText("名称", "Name"), langText("值", "Value")});
	treeWidget_->clear();

	auto* rowFont = new QTreeWidgetItem(treeWidget_);
	rowFont->setText(0, langText("字体", "Font"));
	rowFont->setText(1, QStringLiteral("HarmonyOS Sans SC"));
	auto* rowTheme = new QTreeWidgetItem(treeWidget_);
	rowTheme->setText(0, langText("主题", "Theme"));
	rowTheme->setText(1, langText("Carbon 暗色", "Carbon Dark"));
	auto* rowLang = new QTreeWidgetItem(treeWidget_);
	rowLang->setText(0, langText("语言", "Language"));
	rowLang->setText(1, langText("中文", "English"));

	groupBoxCollapseA_->setTitle(
	    langText("可折叠面板 A（默认展开）", "Collapsible panel A (expanded)"));
	labelCollapseA1_->setText(
	    langText("面板 A 内容：点击标题可折叠 / 展开（180ms 动画）。",
		         "Panel A: click the title to collapse / expand (180 ms animation)."));
	groupBoxCollapseB_->setTitle(
	    langText("可折叠面板 B（默认折叠）", "Collapsible panel B (collapsed)"));
	labelCollapseB1_->setText(
	    langText("面板 B 内容：标题栏由 CarbonUI 折叠器接管。",
		         "Panel B: the title bar is managed by the CarbonUI collapser."));
}
