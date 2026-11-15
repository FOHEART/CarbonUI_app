/**
 * @file  ButtonPage.h
 * @brief 按钮测试页声明：独立按钮 + 分段按钮组 + 长文本按钮（溢出观察）。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_core
 *
 * @details
 * 主界面第 0 个 Tab 的页面控件。构造时完成控件装配、CarbonUI 按钮风格应用
 * 与点击信号接线（点击经 @ref GalleryPage::statusMessage 上抛到状态栏）；
 * 文本覆写由 @ref retranslatePage 按当前语言完成。控件 objectName 与原
 * MainWindow.ui 中的命名保持一致，测试与脚本无需感知拆分。
 *
 * 线程安全：仅允许在 GUI 主线程使用。
 */

#ifndef CARBONUI_APP_BUTTONPAGE_H
#define CARBONUI_APP_BUTTONPAGE_H

#include "GalleryPage.h"

#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>

/**
 * @brief 按钮测试页：CarbonUI 按钮外形分类展示与点击反馈。
 *
 * @details
 * 分区：按钮样式（主色按钮 + 左/中/右三段式按钮组）与长文本按钮
 * （观察文本是否溢出按钮边界）。
 */
class ButtonPage : public GalleryPage {
	Q_OBJECT

  public:
	/**
	 * @brief 构造按钮页：装配控件、应用 CarbonUI 风格、接入点击反馈。
	 * @param[in] parent 父控件指针，可为 NULL；通常由主窗口持有。
	 */
	explicit ButtonPage(QWidget* parent = nullptr);

  protected:
	/**
	 * @brief 按当前语言覆写页内全部文本（详见 GalleryPage::retranslatePage）。
	 */
	void retranslatePage() override;

  private:
	void buildUi();            ///< 构建控件树与布局（仅构造时调用）。
	void applyStyles();        ///< 应用 CarbonUI 按钮风格（仅构造时调用）。
	void connectPageSignals(); ///< 连接按钮点击到状态栏消息（仅构造时调用）。

	QScrollArea* scrollArea_ = nullptr;       ///< 页面滚动容器（objectName: scrollArea）。
	QGroupBox* groupBoxButtons_ = nullptr;    ///< "按钮样式"分组框。
	QPushButton* pushButton_ = nullptr;       ///< 主色按钮（objectName: pushButton）。
	QPushButton* pushButtonSegL_ = nullptr;   ///< 分段按钮左段（objectName: pushButton_7）。
	QPushButton* pushButtonSegM_ = nullptr;   ///< 分段按钮中段（objectName: pushButton_9）。
	QPushButton* pushButtonSegR_ = nullptr;   ///< 分段按钮右段（objectName: pushButton_10）。
	QLabel* labelSegButtonsHint_ = nullptr;   ///< 分段按钮说明文字。
	QGroupBox* groupBoxButtonLong_ = nullptr; ///< 长文本按钮分组框。
	QPushButton* pushButtonLongCn_ = nullptr; ///< 长文本按钮（中文样例）。
	QPushButton* pushButtonLongEn_ = nullptr; ///< 长文本按钮（英文样例）。
};

#endif // CARBONUI_APP_BUTTONPAGE_H
