/**
 * @file  ScrollPage.h
 * @brief 滚动测试页声明：CarbonUIQScrollArea 与 CarbonUIQScrollBar 的功能与外观观察。
 * @author FOHEART
 * @date  2026-11-16
 * @version 1.0.0
 * @ingroup carbonui_app_core
 *
 * @details
 * 主界面第 7 个 Tab 的页面控件（追加在各页之后，既有页签下标不受影响）。
 * 构造时完成控件装配与交互接线；两个演示对象的外观由 CarbonUI 子库控件自身
 * 负责（@c CarbonUIQScrollArea 构造即带无边框滚动区 + 滚动条委托刷新，
 * @c CarbonUIQScrollBar 构造即带 8px 命中范围 + 悬停加厚外观），本页不重复写
 * QSS。滚动条取值变化经 @ref GalleryPage::statusMessage 上抛到状态栏。
 *
 * 线程安全：仅允许在 GUI 主线程使用。
 */

#ifndef CARBONUI_APP_SCROLLPAGE_H
#define CARBONUI_APP_SCROLLPAGE_H

#include "GalleryPage.h"

#include "CarbonUIQScrollArea.h"
#include "CarbonUIQScrollBar.h"

#include <QtCore/QList>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollBar>

/**
 * @brief 滚动测试页：滚动区（双向滚动条）与独立滚动条（垂直 / 水平）的观察。
 *
 * @details
 * 分区：
 *  - 滚动区：CarbonUIQScrollArea 装载一块 520x360 的固定尺寸内容，内容超出
 *    视口，垂直与水平滚动条同时出现，可直接观察常态 4px、悬停 8px（命中范围
 *    恒为 8px）的加厚效果；
 *  - 滚动条：竖直条由 CarbonUIQScrollBar 构造函数创建（自带外观），水平条为
 *    原生 QScrollBar + CarbonUIQScrollBar::applyStyle()（演示给外部已有滚动条
 *    刷同一套外观的接口）；两条条的取值实时回显，并上抛状态栏消息。
 */
class ScrollPage : public GalleryPage {
	Q_OBJECT

  public:
	/**
	 * @brief 构造滚动页：装配控件并接入滚动条取值反馈。
	 * @param[in] parent 父控件指针，可为 NULL；通常由主窗口持有。
	 */
	explicit ScrollPage(QWidget* parent = nullptr);

  protected:
	/**
	 * @brief 按当前语言覆写页内全部文本并重建滚动区内容行
	 *        （详见 GalleryPage::retranslatePage）。
	 */
	void retranslatePage() override;

  private:
	void buildUi();            ///< 构建控件树与布局（仅构造时调用）。
	void connectPageSignals(); ///< 连接滚动条取值到回显与状态栏（仅构造时调用）。
	void refreshValueLabels(); ///< 按当前取值刷新两个数值标签（取值变化与重翻译共用）。

	QGroupBox* groupBoxScrollArea_ =
	    nullptr;                            ///< "滚动区"分组框（objectName: groupBoxScrollArea）。
	QLabel* labelScrollAreaHint_ = nullptr; ///< 滚动区说明文字（objectName: labelScrollAreaHint）。
	CarbonUIQScrollArea* scrollAreaDemo_ =
	    nullptr;                             ///< 演示用滚动区（objectName: scrollAreaDemo）。
	QList<QLabel*> contentRows_;             ///< 滚动区内容行，重翻译时重建文本。
	QGroupBox* groupBoxScrollBar_ = nullptr; ///< "滚动条"分组框（objectName: groupBoxScrollBar）。
	QLabel* labelScrollBarHint_ = nullptr;   ///< 滚动条说明文字（objectName: labelScrollBarHint）。
	QLabel* labelScrollBarVerticalCaption_ =
	    nullptr; ///< 竖直条行标题（objectName: labelScrollBarVerticalCaption）。
	QLabel* labelScrollBarHorizontalCaption_ =
	    nullptr; ///< 水平条行标题（objectName: labelScrollBarHorizontalCaption）。
	CarbonUIQScrollBar* scrollBarVertical_ =
	    nullptr;                                ///< 竖直滚动条（objectName: scrollBarVertical）。
	QScrollBar* scrollBarHorizontal_ = nullptr; ///< 水平滚动条（objectName: scrollBarHorizontal）。
	QLabel* labelScrollBarVerticalValue_ =
	    nullptr; ///< 竖直条取值回显（objectName: labelScrollBarVerticalValue）。
	QLabel* labelScrollBarHorizontalValue_ =
	    nullptr; ///< 水平条取值回显（objectName: labelScrollBarHorizontalValue）。
};

#endif // CARBONUI_APP_SCROLLPAGE_H
