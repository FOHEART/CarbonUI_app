/**
 * @file  DialogPage.h
 * @brief 对话框测试页声明：按钮弹出 CarbonUI 深色 About 式「测试」对话框。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_core
 *
 * @details
 * 主界面第 5 个 Tab 的页面控件。构造时装配"对话框"分组（说明文字 + 打开按钮）
 * 并应用 CarbonUI 按钮风格；点击按钮经 @ref createTestDialog 构造 CarbonUI
 * 子库的 @ref CarbonUIDialog（标题「测试」/ Test），以模态方式弹出。
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
 * 分区：对话框（说明文字 + "打开「测试」对话框"按钮）。对话框内容为四条
 * "名称: 取值" 信息行（应用修订号 / 应用构建时间 / 程序版本 / 许可证），
 * 与 CarbonUI 产品线的 About 对话框一致。
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

  protected:
	/**
	 * @brief 按当前语言覆写页内全部文本（详见 GalleryPage::retranslatePage）。
	 */
	void retranslatePage() override;

  private:
	void buildUi();     ///< 构建控件树与布局（仅构造时调用）。
	void applyStyles(); ///< 应用 CarbonUI 按钮风格（仅构造时调用）。

	QGroupBox* groupBoxDialogs_ = nullptr; ///< "对话框"分组框（objectName: groupBoxDialogs）。
	QLabel* labelDialogHint_ = nullptr;    ///< 对话框说明文字（objectName: labelDialogHint）。
	QPushButton* pushButtonOpenDialog_ =
	    nullptr; ///< 打开对话框按钮（objectName: pushButtonOpenDialog）。
};

#endif // CARBONUI_APP_DIALOGPAGE_H
