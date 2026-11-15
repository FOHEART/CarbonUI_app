/**
 * @file  SelectionPage.h
 * @brief 选择与开关测试页声明：自绘单选（独立/分段/长文本）+ 自绘指示灯复选框。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_core
 *
 * @details
 * 主界面第 1 个 Tab 的页面控件。构造时完成控件装配、CarbonUI 选择类风格
 * 应用（自绘圆角单选 + 自绘指示灯复选框，均非幂等，仅此一次）；文本覆写
 * 由 @ref retranslatePage 按当前语言完成。自绘复选框的 text() 被 CarbonUI
 * 清空，文案由旁置 QLabel 承载。控件 objectName 与原 MainWindow.ui 一致。
 *
 * 线程安全：仅允许在 GUI 主线程使用。
 */

#ifndef CARBONUI_APP_SELECTIONPAGE_H
#define CARBONUI_APP_SELECTIONPAGE_H

#include "GalleryPage.h"

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>

/**
 * @brief 选择与开关测试页：单选按钮与复选框的功能、外形与长文本溢出观察。
 *
 * @details
 * 分区：单选按钮（独立 + 左/中/右三段式）→ 复选框（自绘指示灯 ×2 + 原生
 * 对照）→ 长文本单选（溢出观察）。
 */
class SelectionPage : public GalleryPage {
	Q_OBJECT

  public:
	/**
	 * @brief 构造选择页：装配控件、应用 CarbonUI 选择类风格。
	 * @param[in] parent 父控件指针，可为 NULL；通常由主窗口持有。
	 */
	explicit SelectionPage(QWidget* parent = nullptr);

  protected:
	/**
	 * @brief 按当前语言覆写页内全部文本（详见 GalleryPage::retranslatePage）。
	 */
	void retranslatePage() override;

  private:
	void buildUi();     ///< 构建控件树与布局（仅构造时调用）。
	void applyStyles(); ///< 应用 CarbonUI 选择类风格（仅构造时调用）。

	QGroupBox* groupBoxRadios_ = nullptr;     ///< "单选按钮"分组框。
	QRadioButton* radioButtonSolo_ = nullptr; ///< 独立单选（objectName: radioButtonSolo）。
	QRadioButton* radioButtonSegL_ = nullptr; ///< 分段单选左段（objectName: radioButtonSegL）。
	QRadioButton* radioButtonSegM_ = nullptr; ///< 分段单选中段（objectName: radioButtonSegM）。
	QRadioButton* radioButtonSegR_ = nullptr; ///< 分段单选右段（objectName: radioButtonSegR）。
	QLabel* labelRadioHint_ = nullptr;        ///< 单选说明文字。
	QGroupBox* groupBoxChecks_ = nullptr;     ///< "复选框"分组框。
	QCheckBox* checkIndicatorA_ = nullptr; ///< 自绘指示灯复选框 A（objectName: checkIndicatorA）。
	QLabel* labelCheckA_ = nullptr;        ///< 复选框 A 旁置文案标签。
	QCheckBox* checkIndicatorB_ = nullptr; ///< 自绘指示灯复选框 B（objectName: checkIndicatorB）。
	QLabel* labelCheckB_ = nullptr;        ///< 复选框 B 旁置文案标签。
	QCheckBox* checkBoxNative_ = nullptr;  ///< 原生复选框对照（objectName: checkBoxNative）。
	QLabel* labelCheckHint_ = nullptr;     ///< 复选框说明文字。
	QGroupBox* groupBoxRadioLong_ = nullptr;    ///< 长文本单选分组框。
	QRadioButton* radioButtonLongCn_ = nullptr; ///< 长文本单选（中文样例）。
	QRadioButton* radioButtonLongEn_ = nullptr; ///< 长文本单选（英文样例）。
};

#endif // CARBONUI_APP_SELECTIONPAGE_H
