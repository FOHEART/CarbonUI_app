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
 * 由 @ref retranslatePage 按当前语言完成。自绘复选框（指示灯居左形态）的
 * text() 被 CarbonUI 清空，文案由旁置 QLabel 承载；指示灯居右形态的文案
 * 由控件内部子标签承载，直接 setText 即可。控件 objectName 与原
 * MainWindow.ui 一致。
 *
 * 「复选框」分组内另有指示灯形态测试按钮：点击后销毁并重建本分组的两个
 * 自绘复选框，在指示灯居左/居右两种形态间切换（CarbonUI 的 applyCheckStyle
 * 非幂等，切换即重建；勾选状态予以保留）。
 *
 * 线程安全：仅允许在 GUI 主线程使用。
 */

#ifndef CARBONUI_APP_SELECTIONPAGE_H
#define CARBONUI_APP_SELECTIONPAGE_H

#include "GalleryPage.h"

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

/**
 * @brief 选择与开关测试页：单选按钮与复选框的功能、外形与长文本溢出观察。
 *
 * @details
 * 分区：单选按钮（独立 + 左/中/右三段式）→ 复选框（自绘指示灯 ×2 + 原生
 * 对照 + 形态测试按钮）→ 复选框（指示灯居右形态）→ 长文本单选（溢出观察）。
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

	/**
	 * @brief 事件过滤器：把旁置文案标签的鼠标点击转发为对应复选框的勾选切换，
	 *        使指示灯居左形态与居右形态的交互一致（点文字也能选中/取消）。
	 * @param[in] watched 被过滤对象（labelCheckA_/labelCheckB_）。
	 * @param[in] event 事件；仅处理 MouseButtonPress，其余放行。
	 * @return 标签点击已消费返回 true，其余交回基类处理。
	 */
	bool eventFilter(QObject* watched, QEvent* event) override;

  private:
	void buildUi();     ///< 构建控件树与布局（仅构造时调用）。
	void applyStyles(); ///< 应用 CarbonUI 选择类风格（仅构造时调用）。

	/**
	 * @brief 指示灯形态测试按钮：销毁重建本分组两个自绘复选框，在
	 *        指示灯居左/居右形态间切换（详见 SelectionPage.h 类说明）。
	 */
	void toggleIndicatorForm();

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
	QPushButton* buttonRightFormTest_ =
	    nullptr; ///< 指示灯形态测试按钮（objectName: buttonRightFormTest）。
	QBoxLayout* checkRowLayoutA_ = nullptr;    ///< 复选框 A 所在行布局（重建控件时定位插入点）。
	QBoxLayout* checkRowLayoutB_ = nullptr;    ///< 复选框 B 所在行布局（重建控件时定位插入点）。
	bool indicatorRightForm_ = false;          ///< 当前自绘复选框是否处于指示灯居右形态。
	QGroupBox* groupBoxChecksRight_ = nullptr; ///< "复选框（指示灯居右）"分组框。
	QCheckBox* checkIndicatorRightA_ =
	    nullptr; ///< 指示灯居右复选框 A（objectName: checkIndicatorRightA）。
	QCheckBox* checkIndicatorRightB_ =
	    nullptr; ///< 指示灯居右复选框 B（objectName: checkIndicatorRightB）。
	QLabel* labelCheckRightHint_ = nullptr;     ///< 指示灯居右说明文字。
	QGroupBox* groupBoxRadioLong_ = nullptr;    ///< 长文本单选分组框。
	QRadioButton* radioButtonLongCn_ = nullptr; ///< 长文本单选（中文样例）。
	QRadioButton* radioButtonLongEn_ = nullptr; ///< 长文本单选（英文样例）。
};

#endif // CARBONUI_APP_SELECTIONPAGE_H
