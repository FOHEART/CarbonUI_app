/**
 * @file  ListPage.h
 * @brief 列表与折叠测试页声明：树控件 + 可折叠分组框（CarbonUI 折叠器）。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_core
 *
 * @details
 * 主界面第 3 个 Tab 的页面控件。构造时完成控件装配、CarbonUI 树风格应用
 * 与折叠器安装（面板 A 默认展开、B 默认折叠）；树条目点击经
 * @ref GalleryPage::statusMessage 上抛到状态栏；树内容为动态数据，语言
 * 切换时在 @ref retranslatePage 重建。控件 objectName 与原 MainWindow.ui 一致。
 *
 * 线程安全：仅允许在 GUI 主线程使用。
 */

#ifndef CARBONUI_APP_LISTPAGE_H
#define CARBONUI_APP_LISTPAGE_H

#include "GalleryPage.h"

#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTreeWidget>

/**
 * @brief 列表与折叠测试页：树控件展示与分组框折叠交互。
 *
 * @details
 * 分区：树控件（表头加粗、缩进压平）→ 可折叠面板 A（默认展开）→
 * 可折叠面板 B（默认折叠）。
 */
class ListPage : public GalleryPage {
	Q_OBJECT

  public:
	/**
	 * @brief 构造列表页：装配控件、应用 CarbonUI 树风格、安装折叠器。
	 * @param[in] parent 父控件指针，可为 NULL；通常由主窗口持有。
	 */
	explicit ListPage(QWidget* parent = nullptr);

  protected:
	/**
	 * @brief 按当前语言覆写页内全部文本并重建树条目
	 *        （详见 GalleryPage::retranslatePage）。
	 */
	void retranslatePage() override;

  private:
	void buildUi();            ///< 构建控件树与布局（仅构造时调用）。
	void applyStyles();        ///< 应用 CarbonUI 树风格并安装折叠器（仅构造时调用）。
	void connectPageSignals(); ///< 连接树条目点击到状态栏消息（仅构造时调用）。

	QLabel* labelTreeHint_ = nullptr;        ///< 树控件说明文字。
	QTreeWidget* treeWidget_ = nullptr;      ///< 展示树（objectName: treeWidget）。
	QGroupBox* groupBoxCollapseA_ = nullptr; ///< 可折叠面板 A（objectName: groupBoxCollapseA）。
	QLabel* labelCollapseA1_ = nullptr;      ///< 面板 A 内容说明。
	QGroupBox* groupBoxCollapseB_ = nullptr; ///< 可折叠面板 B（objectName: groupBoxCollapseB）。
	QLabel* labelCollapseB1_ = nullptr;      ///< 面板 B 内容说明。
};

#endif // CARBONUI_APP_LISTPAGE_H
