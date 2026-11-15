/**
 * @file  InputPage.h
 * @brief 输入测试页声明：数字输入框 + 普通输入框 + 下拉框。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_core
 *
 * @details
 * 主界面第 2 个 Tab 的页面控件。构造时完成控件装配与 CarbonUI 输入/展示类
 * 风格应用；交互事件（回车回显、下拉选中）经 @ref GalleryPage::statusMessage
 * 上抛到状态栏；下拉项为动态内容，语言切换时在 @ref retranslatePage 重建。
 * 控件 objectName 与原 MainWindow.ui 一致。
 *
 * 线程安全：仅允许在 GUI 主线程使用。
 */

#ifndef CARBONUI_APP_INPUTPAGE_H
#define CARBONUI_APP_INPUTPAGE_H

#include "GalleryPage.h"

#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

/**
 * @brief 输入测试页：输入框功能（校验/全选/回显）与下拉框选中反馈。
 *
 * @details
 * 分区：数字输入框（聚焦全选 + C 语言环境校验）→ 普通输入框 → 下拉框
 * （含长文本选项观察溢出）。
 */
class InputPage : public GalleryPage {
	Q_OBJECT

  public:
	/**
	 * @brief 构造输入页：装配控件、应用 CarbonUI 风格、接入交互反馈。
	 * @param[in] parent 父控件指针，可为 NULL；通常由主窗口持有。
	 */
	explicit InputPage(QWidget* parent = nullptr);

  protected:
	/**
	 * @brief 按当前语言覆写页内全部文本并重建下拉项
	 *        （详见 GalleryPage::retranslatePage）。
	 */
	void retranslatePage() override;

  private:
	void buildUi();            ///< 构建控件树与布局（仅构造时调用）。
	void applyStyles();        ///< 应用 CarbonUI 输入/展示类风格（仅构造时调用）。
	void connectPageSignals(); ///< 连接回车与下拉选中到状态栏消息（仅构造时调用）。

	QGroupBox* groupBoxNumber_ = nullptr;   ///< "数字输入框"分组框。
	QLabel* labelNumberHint_ = nullptr;     ///< 数字输入说明文字。
	QLineEdit* lineEditNumber_ = nullptr;   ///< 数字输入框（objectName: lineEditNumber）。
	QGroupBox* groupBoxLineEdit_ = nullptr; ///< "普通输入框"分组框。
	QLineEdit* lineEditFree_ = nullptr;     ///< 自由文本输入框（objectName: lineEditFree）。
	QGroupBox* groupBoxCombo_ = nullptr;    ///< "下拉框"分组框。
	QLabel* labelComboHint_ = nullptr;      ///< 下拉框说明文字。
	QComboBox* comboBox_ = nullptr;         ///< 下拉框（objectName: comboBox）。
};

#endif // CARBONUI_APP_INPUTPAGE_H
