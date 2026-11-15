/**
 * @file  TextOverflowPage.h
 * @brief 文本溢出测试页声明：固定尺寸控件 + 长中英文文本的截断/省略/滚动观察。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_core
 *
 * @details
 * 主界面第 4 个 Tab 的页面控件。构造时完成控件装配与 CarbonUI 风格应用；
 * 文本覆写由 @ref retranslatePage 按当前语言完成。页面内所有观察对象均为
 * 固定尺寸（min == max），专门用于切换语言后逐一检查文本是否超出控件
 * 边界。控件 objectName 与原 MainWindow.ui 一致。
 *
 * 线程安全：仅允许在 GUI 主线程使用。
 */

#ifndef CARBONUI_APP_TEXTOVERFLOWPAGE_H
#define CARBONUI_APP_TEXTOVERFLOWPAGE_H

#include "GalleryPage.h"

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>

/**
 * @brief 文本溢出测试页：长中英文文本在各控件固定尺寸内的表现观察。
 *
 * @details
 * 分区（每行"类别说明 + 观察对象"）：按钮（中/英文）→ 单选 → 复选框
 * （自绘指示灯 + 长标签）→ 输入框 → 下拉框 → 标签（自动换行 / 不换行）。
 */
class TextOverflowPage : public GalleryPage {
	Q_OBJECT

  public:
	/**
	 * @brief 构造溢出页：装配控件、应用 CarbonUI 风格。
	 * @param[in] parent 父控件指针，可为 NULL；通常由主窗口持有。
	 */
	explicit TextOverflowPage(QWidget* parent = nullptr);

  protected:
	/**
	 * @brief 按当前语言覆写页内全部文本并重建下拉项
	 *        （详见 GalleryPage::retranslatePage）。
	 */
	void retranslatePage() override;

  private:
	void buildUi();     ///< 构建控件树与布局（仅构造时调用）。
	void applyStyles(); ///< 应用 CarbonUI 风格（仅构造时调用）。

	/// @name 创建单行观察条目的辅助
	/// @{
	/// 创建"标题标签（最小宽 140） + 弹簧"行布局，返回行布局供调用方插入观察对象。
	QHBoxLayout* addOverflowRow(QVBoxLayout* pageLayout, const char* captionName,
	                            QLabel** captionLabel);
	/// @}

	QScrollArea* scrollArea_ = nullptr;      ///< 页面滚动容器（objectName: scrollAreaOverflow）。
	QLabel* labelOvTitle_ = nullptr;         ///< 页面说明标题。
	QLabel* labelOvBtnCn_ = nullptr;         ///< 按钮中文样例行标题。
	QPushButton* pushButtonOvCn_ = nullptr;  ///< 固定尺寸按钮（中文文本）。
	QLabel* labelOvBtnEn_ = nullptr;         ///< 按钮英文样例行标题。
	QPushButton* pushButtonOvEn_ = nullptr;  ///< 固定尺寸按钮（英文文本）。
	QLabel* labelOvRadio_ = nullptr;         ///< 单选行标题。
	QRadioButton* radioButtonOv_ = nullptr;  ///< 固定尺寸单选。
	QLabel* labelOvCheckCaption_ = nullptr;  ///< 复选框行标题。
	QCheckBox* checkIndicatorOv_ = nullptr;  ///< 自绘指示灯复选框（objectName: checkIndicatorOv）。
	QLabel* labelOvCheck_ = nullptr;         ///< 复选框长标签（固定宽 120）。
	QLabel* labelOvLineEdit_ = nullptr;      ///< 输入框行标题。
	QLineEdit* lineEditOv_ = nullptr;        ///< 固定尺寸输入框（宽 120）。
	QLabel* labelOvCombo_ = nullptr;         ///< 下拉框行标题。
	QComboBox* comboBoxOv_ = nullptr;        ///< 固定尺寸下拉框（宽 110）。
	QLabel* labelOvWrapCaption_ = nullptr;   ///< 换行标签行标题。
	QLabel* labelOvWrap_ = nullptr;          ///< 自动换行标签（固定宽 160）。
	QLabel* labelOvNoWrapCaption_ = nullptr; ///< 不换行标签行标题。
	QLabel* labelOvNoWrap_ = nullptr;        ///< 不换行标签（固定宽 160）。
};

#endif // CARBONUI_APP_TEXTOVERFLOWPAGE_H
