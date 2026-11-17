/**
 * @file  HeaderButtonPage.cpp
 * @brief 带按钮标题栏控件测试页实现（接口见 HeaderButtonPage.h）。
 * @author FOHEART
 * @date  2026-11-16
 * @version 1.0.0
 *
 * @details
 * 页面用 QGridLayout 把四个 CarbonUIWidgetWithHdrBtn 排成 2x2 网格。图标用
 * QPainter 直接绘制成 16x16 像素图，不引入新的图片资源；六个按钮的 id / 名称 /
 * 事件文案集中在 @c kButtons 表，四个控件共用这份数据，各自独立计数并回显到
 * 自己内容区的事件标签，避免语言切换后事件文案与按钮语义脱节。
 *
 * 本源码基于 Mozilla Public License 2.0 发布：
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "HeaderButtonPage.h"

#include "CarbonUIWidgetWithHdrBtn.h"

#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QIcon>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QPixmap>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

#include <array>

namespace {

/// @brief 图标样式标识：与参考图中的六个按钮一一对应。
enum class IconKind : quint8 { Copy, Sync, List, Eye, Italic, Underline };

/// @brief 按钮描述表：id / 中英文名称 / 中英文事件文案 / 图标样式。
struct ButtonSpec {
	const char* id;
	const char* nameZh;
	const char* nameEn;
	const char* eventZh;
	const char* eventEn;
	IconKind icon;
};

/// @brief 测试页按钮的固定清单（顺序即标题栏从左到右的顺序）。
const std::array<ButtonSpec, 6> kButtons = {{
    {"copy", "复制", "Copy", "已触发复制事件", "Copy event triggered", IconKind::Copy},
    {"sync", "同步", "Sync", "已触发同步事件", "Sync event triggered", IconKind::Sync},
    {"list", "列表", "List", "已触发列表事件", "List event triggered", IconKind::List},
    {"eye", "显示", "Visibility", "已触发显示事件", "Visibility event triggered", IconKind::Eye},
    {"italic", "斜体", "Italic", "已触发斜体事件", "Italic event triggered", IconKind::Italic},
    {"underline", "下划线", "Underline", "已触发下划线事件", "Underline event triggered",
	 IconKind::Underline},
}};

/// @brief 页面内控件总数（2 行 x 2 列）。
constexpr int kWidgetCount = 4;
/// @brief 网格列数。
constexpr int kGridColumns = 2;

/**
 * @brief 按 id 查找按钮在 kButtons 中的下标。
 * @param[in] id 按钮标识。
 * @return 找到时返回下标 [0, kButtons.size())，否则返回 -1。
 */
int buttonIndex(const QString& id) {
	for (std::size_t i = 0; i < kButtons.size(); ++i) {
		if (id == QLatin1String(kButtons[i].id)) {
			return static_cast<int>(i);
		}
	}
	return -1;
}

/**
 * @brief 用 QPainter 绘制 16x16 单色图标。
 * @param[in] kind 图标样式。
 * @return 可设置到 QToolButton 的图标。
 */
QIcon makeIcon(IconKind kind) {
	QPixmap pixmap(16, 16);
	pixmap.fill(Qt::transparent);

	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing, true);
	QPen pen(QColor(QStringLiteral("#b9b9b9")));
	pen.setWidthF(1.2);
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);

	switch (kind) {
	case IconKind::Copy:
		painter.drawRect(QRectF(1.5, 4.5, 9.0, 9.0));
		painter.drawRect(QRectF(4.5, 1.5, 9.0, 9.0));
		break;
	case IconKind::Sync:
		painter.drawArc(QRectF(2.0, 2.0, 12.0, 12.0), 30 * 16, 280 * 16);
		painter.drawLine(QPointF(12.5, 3.0), QPointF(14.0, 5.5));
		painter.drawLine(QPointF(14.0, 5.5), QPointF(11.2, 6.2));
		painter.drawLine(QPointF(3.5, 13.0), QPointF(2.0, 10.5));
		painter.drawLine(QPointF(2.0, 10.5), QPointF(4.8, 9.8));
		break;
	case IconKind::List:
		for (int y = 4; y <= 12; y += 4) {
			painter.drawLine(QPointF(2.0, static_cast<qreal>(y)),
			                 QPointF(14.0, static_cast<qreal>(y)));
		}
		break;
	case IconKind::Eye:
		painter.drawEllipse(QRectF(1.5, 5.0, 13.0, 6.0));
		painter.drawEllipse(QRectF(6.5, 6.5, 3.0, 3.0));
		break;
	case IconKind::Italic: {
		QFont font = painter.font();
		font.setItalic(true);
		font.setBold(true);
		painter.setFont(font);
		painter.drawText(QRectF(0.0, 0.0, 16.0, 14.0), Qt::AlignCenter, QStringLiteral("I"));
		break;
	}
	case IconKind::Underline:
		painter.drawText(QRectF(0.0, 0.0, 16.0, 12.0), Qt::AlignCenter, QStringLiteral("U"));
		painter.drawLine(QPointF(3.0, 13.0), QPointF(13.0, 13.0));
		break;
	}
	painter.end();
	return {pixmap};
}

} // namespace

/**
 * @brief 构造测试页：装配控件并连接按钮信号；首次文本覆写由主窗口统一触发。
 */
HeaderButtonPage::HeaderButtonPage(QWidget* parent) : GalleryPage(parent) {
	buildUi();
	connectSignals();
}

/**
 * @brief 构建控件树：页面说明 + 四个被测控件的 2x2 网格。
 */
void HeaderButtonPage::buildUi() {
	auto* pageLayout = new QVBoxLayout(this);
	pageLayout->setContentsMargins(16, 16, 16, 16);
	pageLayout->setSpacing(12);

	hintLabel_ = new QLabel(this);
	hintLabel_->setObjectName(QStringLiteral("labelHeaderButtonHint"));
	hintLabel_->setWordWrap(true);
	pageLayout->addWidget(hintLabel_);

	auto* grid = new QGridLayout;
	grid->setSpacing(12);
	for (int column = 0; column < kGridColumns; ++column) {
		grid->setColumnStretch(column, 1);
	}
	const int rows = (kWidgetCount + kGridColumns - 1) / kGridColumns;
	for (int row = 0; row < rows; ++row) {
		grid->setRowStretch(row, 1);
	}

	for (int w = 0; w < kWidgetCount; ++w) {
		const QString indexText = QString::number(w + 1);
		auto* widget = new CarbonUIWidgetWithHdrBtn(QString(), this);
		widget->setObjectName(QStringLiteral("widgetWithHeaderButtonDemo_") + indexText);
		for (const ButtonSpec& spec : kButtons) {
			widget->addButton(QLatin1String(spec.id), makeIcon(spec.icon), QString());
		}
		grid->addWidget(widget, w / kGridColumns, w % kGridColumns);

		auto* contentLayout = new QVBoxLayout(widget->contentWidget());
		contentLayout->setContentsMargins(12, 10, 12, 10);
		contentLayout->setSpacing(6);

		auto* lastLabel = new QLabel(widget->contentWidget());
		lastLabel->setObjectName(QStringLiteral("labelHeaderButtonLastEvent_") + indexText);
		contentLayout->addWidget(lastLabel);
		lastEventLabels_.append(lastLabel);

		QList<QLabel*> labels;
		for (const ButtonSpec& spec : kButtons) {
			auto* label = new QLabel(widget->contentWidget());
			label->setObjectName(QStringLiteral("labelHeaderButtonEvent_") + indexText +
			                     QStringLiteral("_") + QLatin1String(spec.id));
			contentLayout->addWidget(label);
			labels.append(label);
		}
		contentLayout->addStretch(1);

		widgets_.append(widget);
		eventLabels_.append(labels);
		clickCounts_.append(QVector<int>(static_cast<int>(kButtons.size()), 0));
		lastClickedIndices_.append(-1);
	}
	pageLayout->addLayout(grid);
}

/**
 * @brief 连接四个控件的按钮点击：捕获控件下标，分派到对应控件的计数与标签。
 */
void HeaderButtonPage::connectSignals() {
	for (int w = 0; w < widgets_.size(); ++w) {
		connect(widgets_.at(w), &CarbonUIWidgetWithHdrBtn::buttonClicked, this,
		        [this, w](const QString& id) { handleButtonClicked(w, id); });
	}
}

/**
 * @brief 按控件下标与按钮 id 分派事件：更新该控件计数、标签并上抛状态栏消息。
 */
void HeaderButtonPage::handleButtonClicked(int widgetIndex, const QString& id) {
	if (widgetIndex < 0 || widgetIndex >= widgets_.size()) {
		return;
	}
	const int index = buttonIndex(id);
	if (index < 0 || index >= eventLabels_.at(widgetIndex).size()) {
		return;
	}
	clickCounts_[widgetIndex][index] += 1;
	lastClickedIndices_[widgetIndex] = index;
	updateEventLabels();

	const ButtonSpec& spec = kButtons[static_cast<std::size_t>(index)];
	emit statusMessage(langText("控件 %1：", "Widget %1: ").arg(widgetIndex + 1) +
	                   langText(spec.nameZh, spec.nameEn) +
	                   langText(" 按钮被点击", " button clicked"));
}

/**
 * @brief 刷新四个控件内容区的全部事件标签（供点击与语言切换共用）。
 */
void HeaderButtonPage::updateEventLabels() {
	for (int w = 0; w < widgets_.size(); ++w) {
		for (int i = 0; i < eventLabels_.at(w).size(); ++i) {
			const ButtonSpec& spec = kButtons[static_cast<std::size_t>(i)];
			eventLabels_[w].at(i)->setText(
			    langText(spec.nameZh, spec.nameEn) + QStringLiteral("：") +
			    langText(spec.eventZh, spec.eventEn) + QStringLiteral("（") +
			    QString::number(clickCounts_.at(w).at(i)) + langText(" 次）", " time(s)"));
		}

		QLabel* lastLabel = lastEventLabels_.at(w);
		const int lastIndex = lastClickedIndices_.at(w);
		if (lastIndex < 0) {
			lastLabel->setText(
			    langText("最近点击：尚未点击任何按钮", "Last click: no button clicked yet"));
			continue;
		}
		const ButtonSpec& last = kButtons[static_cast<std::size_t>(lastIndex)];
		lastLabel->setText(langText("最近点击：", "Last click: ") +
		                   langText(last.nameZh, last.nameEn));
	}
}

/**
 * @brief 按当前语言覆写页内全部文本（详见头文件）。
 */
void HeaderButtonPage::retranslatePage() {
	hintLabel_->setText(
	    langText("四个控件按 2x2 网格排列；点击任一标题栏按钮，事件显示在该控件内部标签中。",
		         "Four widgets are arranged in a 2x2 grid; clicking any header button shows its "
		         "event on labels inside that widget."));
	for (int w = 0; w < widgets_.size(); ++w) {
		widgets_.at(w)->setTitle(langText("对象属性", "Object properties") +
		                         QStringLiteral(" %1").arg(w + 1));
		for (const ButtonSpec& spec : kButtons) {
			widgets_.at(w)->setButtonToolTip(QLatin1String(spec.id),
			                                 langText(spec.nameZh, spec.nameEn));
		}
	}
	updateEventLabels();
}
