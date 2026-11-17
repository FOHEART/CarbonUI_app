/**
 * @file  HeaderButtonPage.h
 * @brief 带按钮标题栏控件测试页：四个控件按 2x2 网格排列，各自按钮事件回显到内部标签。
 * @author FOHEART
 * @date  2026-11-16
 * @version 1.0.0
 * @ingroup carbonui_app_core
 *
 * @details
 * 主界面新增 Tab 的页面控件。页面内创建四个 @ref CarbonUIWidgetWithHdrBtn，以
 * 2 行 2 列网格布局；每个控件标题栏放置复制 / 同步 / 列表 / 显示 / 斜体 /
 * 下划线六个图标按钮，按钮之间由控件自身插入深色分割线；点击后按按钮 id 分派
 * 不同事件文案，并写入该控件内容区的六个事件标签与一个"最近点击"标签。
 * 文本覆写由 @ref retranslatePage 按当前语言完成。
 *
 * 线程安全：仅允许在 GUI 主线程使用。
 */

#ifndef CARBONUI_APP_HEADERBUTTONPAGE_H
#define CARBONUI_APP_HEADERBUTTONPAGE_H

#include "GalleryPage.h"

#include <QtCore/QList>
#include <QtCore/QVector>

class QLabel;
class CarbonUIWidgetWithHdrBtn;

/**
 * @brief 带按钮标题栏控件测试页（四个控件，2x2 网格）。
 */
class HeaderButtonPage : public GalleryPage {
	Q_OBJECT

  public:
	/**
	 * @brief 构造测试页：装配控件、连接按钮事件。
	 * @param[in] parent 父控件指针，可为 nullptr；通常由主窗口持有。
	 */
	explicit HeaderButtonPage(QWidget* parent = nullptr);

  protected:
	/** @brief 按当前语言覆写页内全部文本（详见 GalleryPage::retranslatePage）。 */
	void retranslatePage() override;

  private:
	void buildUi();        ///< 构建控件树与 2x2 网格（仅构造时调用）。
	void connectSignals(); ///< 连接四个控件的标题栏按钮点击（仅构造时调用）。
	void handleButtonClicked(int widgetIndex,
	                         const QString& id); ///< 按控件下标与按钮 id 分派事件。
	void updateEventLabels();                    ///< 按各控件点击次数刷新内容区事件标签。

	QVector<CarbonUIWidgetWithHdrBtn*> widgets_; ///< 四个被测控件，按行优先顺序存放。
	QLabel* hintLabel_ = nullptr;                ///< 页面顶部说明。
	QVector<QLabel*> lastEventLabels_;           ///< 每个控件内容区的"最近点击"标签。
	QVector<QList<QLabel*>> eventLabels_;        ///< 每个控件内与六个按钮一一对应的事件标签。
	QVector<QVector<int>> clickCounts_;          ///< 每个控件各按钮的点击次数。
	QVector<int> lastClickedIndices_;            ///< 每个控件最近点击下标，-1 表示尚未点击。
};

#endif // CARBONUI_APP_HEADERBUTTONPAGE_H
