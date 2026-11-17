/**
 * @file  DialogPage.h
 * @brief 对话框测试页声明：按钮弹出 CarbonUI 深色 About 式「测试」对话框。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_core
 *
 * @details
 * 主界面第 5 个 Tab 的页面控件。构造时装配"对话框"分组（说明文字 + 四个打开
 * 按钮）并应用 CarbonUI 按钮风格；点击按钮经 @ref createTestDialog 构造
 * CarbonUI 子库的 @ref CarbonUIDialog（标题「测试」/ Test），以模态方式弹出。
 *
 * 四个按钮分别演示标题条窗口按钮的四种组合：「测试」对话框（只有关闭）、
 * 仅最小化、仅最大化、最小化 + 最大化。最小化与最大化都是 CarbonUIDialog 的
 * 真实功能（最小化到任务栏、最大化铺满可用区域并切换为还原字形）。
 *
 * @ref createTestDialog 公开给界面测试使用：测试不点按钮（exec 会阻塞），
 * 而是直接拿到对话框实例做结构断言与截图基线，保证"页面弹的窗"与"测的窗"
 * 是同一段构造代码。
 *
 * 线程安全：仅允许在 GUI 主线程使用。
 */

#ifndef CARBONUI_APP_DIALOGPAGE_H
#define CARBONUI_APP_DIALOGPAGE_H

#include "GalleryPage.h"

#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

class QDialog;
class QWidget;

/**
 * @brief 对话框测试页：CarbonUI 深色对话框的弹出演示。
 *
 * @details
 * 分区：「对话框」（说明文字 + 四个打开按钮）与「颜色选择」（打开按钮 +
 * 确认结果的颜色块与十六进制标签）。「测试」对话框内容为四条 "名称: 取值"
 * 信息行（应用修订号 / 应用构建时间 / 程序版本 / 许可证），与 CarbonUI
 * 产品线的 About 对话框一致；标题条窗口按钮按按钮选定显示最小化 / 最大化。
 */
class DialogPage : public GalleryPage {
	Q_OBJECT

  public:
	/**
	 * @brief 构造对话框页：装配控件、应用 CarbonUI 风格、接入点击反馈。
	 * @param[in] parent 父控件指针，可为 NULL；通常由主窗口持有。
	 */
	explicit DialogPage(QWidget* parent = nullptr);

	/**
	 * @brief 按当前语言构造「测试」对话框（不显示、不阻塞）。
	 *
	 * @details
	 * 页面按钮与实际测试共用本函数，保证截图基线与运行时看到的是同一个
	 * 对话框。调用方负责 show()/exec() 并 delete（或交给父对象）。
	 *
	 * @param[in] parent 对话框父窗口，**可为 NULL**；非 NULL 时对话框居中于它。
	 * @return 新建的对话框，父对象为 @p parent；取值一定非 NULL。
	 */
	QDialog* createTestDialog(QWidget* parent) const;

	/**
	 * @brief 按当前语言构造「测试」对话框，并分别开关最小化 / 最大化按钮。
	 *
	 * @details
	 * 两个开关直接映射到 @ref CarbonUIDialog::setMinimizeButtonVisible 与
	 * @ref CarbonUIDialog::setMaximizeButtonVisible，按钮都是真功能：最小化
	 * 走 `showMinimized()`，最大化在最大化与还原之间切换。参数全为 false 时
	 * 与单参数重载等价（保持既有截图基线与用例不变）。
	 *
	 * @param[in] parent   对话框父窗口，**可为 NULL**；非 NULL 时对话框居中于它。
	 * @param[in] minimize true 显示最小化按钮，false 隐藏。
	 * @param[in] maximize true 显示最大化按钮，false 隐藏。
	 * @return 新建的对话框，父对象为 @p parent；取值一定非 NULL。
	 */
	QDialog* createTestDialog(QWidget* parent, bool minimize, bool maximize) const;

	/**
	 * @brief 按当前语言构造「颜色选择」对话框（不显示、不阻塞）。
	 *
	 * @details
	 * 无边框深色对话框（面板自带底色，无白色系统标题栏），内嵌 CarbonUI 子库的
	 * @ref CarbonUIColorPickWidget（色相×饱和度选色区 + 明度滑条 + RGB/十六进制/
	 * 透明度输入 + 颜色预览块 + Ok/Cancel），与页面按钮弹出的窗是同一段构造代码。
	 * Ok / Cancel 分别驱动对话框 accept() / reject()。
	 *
	 * @param[in] parent 对话框父窗口，**可为 NULL**；非 NULL 时对话框居中于它。
	 * @return 新建的对话框，父对象为 @p parent；取值一定非 NULL。
	 */
	QDialog* createColorPickerDialog(QWidget* parent) const;

  protected:
	/**
	 * @brief 按当前语言覆写页内全部文本（详见 GalleryPage::retranslatePage）。
	 */
	void retranslatePage() override;

  private:
	void buildUi();     ///< 构建控件树与布局（仅构造时调用）。
	void applyStyles(); ///< 应用 CarbonUI 按钮风格（仅构造时调用）。

	/**
	 * @brief 以模态方式弹出「测试」对话框，返回后释放。
	 * @param[in] minimize true 显示最小化按钮。
	 * @param[in] maximize true 显示最大化按钮。
	 */
	void openDialog(bool minimize, bool maximize);

	/**
	 * @brief 以模态方式弹出「颜色选择」对话框，接受后向状态栏上报所选颜色。
	 */
	void openColorPicker();

	QGroupBox* groupBoxDialogs_ = nullptr; ///< "对话框"分组框（objectName: groupBoxDialogs）。
	QLabel* labelDialogHint_ = nullptr;    ///< 对话框说明文字（objectName: labelDialogHint）。
	QPushButton* pushButtonOpenDialog_ =
	    nullptr; ///< 打开无窗口按钮的对话框（objectName: pushButtonOpenDialog）。
	QPushButton* pushButtonOpenDialogMinimize_ =
	    nullptr; ///< 仅最小化（objectName: pushButtonOpenDialogMinimize）。
	QPushButton* pushButtonOpenDialogMaximize_ =
	    nullptr; ///< 仅最大化（objectName: pushButtonOpenDialogMaximize）。
	QPushButton* pushButtonOpenDialogBoth_ =
	    nullptr; ///< 最小化 + 最大化（objectName: pushButtonOpenDialogBoth）。
	QPushButton* pushButtonOpenColorPick_ =
	    nullptr; ///< 打开「颜色选择」对话框（objectName: pushButtonOpenColorPick）。
	QGroupBox* groupBoxColorPick_ =
	    nullptr;                         ///< "颜色选择"分组框（objectName: groupBoxColorPick）。
	QLabel* labelColorSwatch_ = nullptr; ///< 确认后的颜色块（objectName: labelColorSwatch）。
	QLabel* labelColorHex_ = nullptr;    ///< 确认后的十六进制文本（objectName: labelColorHex）。
};

#endif // CARBONUI_APP_DIALOGPAGE_H
