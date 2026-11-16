/**
 * @file  MainWindow.h
 * @brief 应用主窗口声明：CarbonUI 控件分类测试台（多 Tab + 菜单栏 + 状态栏 + 中英文切换）。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_core
 *
 * @details
 * MainWindow 负责三件事：
 *  1. 界面装配：setupUi（菜单/工具栏/状态栏/空 TabWidget 骨架）+ 创建五个
 *     页面控件（ButtonPage / SelectionPage / InputPage / ListPage /
 *     TextOverflowPage，各自独立 .h/.cpp）并加入 Tab + 容器级 CarbonUI 风格；
 *  2. 语言管理：@ref setLanguage 切换中英文并触发 @ref retranslateUi 全量
 *     重翻译（窗口标题、Tab 标题、菜单、状态栏），并下发给各页面
 *     （页面内部文本由页面自己覆写）；
 *  3. 交互反馈：接收各页面的 @ref GalleryPage::statusMessage 消息并显示
 *     到状态栏。
 *
 * .ui 文件（ui/MainWindow.ui）只含窗口骨架（菜单栏、工具栏、状态栏与空的
 * QTabWidget），可在 Qt Designer 中自由编辑；各 Tab 页内容在对应页面类中
 * 用代码装配。Designer 里的默认文本仅作预览用途，retranslateUi 会统一覆写。
 * 生命周期：栈上创建，析构时手工释放 ui 对象（控件本身随父子树释放）。
 * 线程安全：仅允许在 GUI 主线程构造与使用。
 */

#pragma once

#include "GalleryPage.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>

namespace Ui {
class MainWindow; ///< 由 MainWindow.ui 经 uic 生成的界面类（完整定义在 ui_MainWindow.h）。
}
class QDialog;          ///< 「关于」/「关于 Qt」对话框的基类（完整定义见 QtWidgets）。
class ButtonPage;       ///< 按钮测试页（独立 .h/.cpp）。
class SelectionPage;    ///< 选择与开关测试页（独立 .h/.cpp）。
class InputPage;        ///< 输入测试页（独立 .h/.cpp）。
class ListPage;         ///< 列表与折叠测试页（独立 .h/.cpp）。
class TextOverflowPage; ///< 文本溢出测试页（独立 .h/.cpp）。
class DialogPage;       ///< 对话框测试页（独立 .h/.cpp）。

/**
 * @brief 主窗口：CarbonUI 控件分类测试台。
 *
 * @details
 * 构造完成后窗口即可显示使用：五个页面已装配并应用 CarbonUI 风格、默认
 * 语言为中文、页面交互事件已接入状态栏。语言切换入口在菜单栏"语言"菜单
 * （中文 / English）。无共享状态，线程亲和性为 GUI 主线程。
 */
class MainWindow : public QMainWindow {
	Q_OBJECT

  public:
	/// @brief 界面语言枚举：复用页面基类的全工程唯一来源。
	using Language = GalleryPage::Language;

	/**
	 * @brief 构造主窗口：装配 .ui 骨架、创建页面、应用 CarbonUI 风格、接入状态栏反馈。
	 * @param[in] parent 父窗口指针，可为 NULL；为 NULL 时成为独立顶层窗口。
	 */
	MainWindow(QWidget* parent = nullptr);

	/** @brief 析构主窗口：释放 ui 对象，子控件随 Qt 父子树自动释放。 */
	~MainWindow() override;

	/**
	 * @brief 切换界面语言并全量重翻译（含向各页面下发语言）。
	 *
	 * @details
	 * 重复设置当前语言为幂等操作（直接返回）；切换后发出 @ref languageChanged。
	 * @param[in] lang 目标语言，取值必须为 ::MainWindow::Language 枚举值。
	 */
	void setLanguage(Language lang);

	/**
	 * @brief 查询当前界面语言。
	 * @return 当前语言，取值为 ::MainWindow::Language 枚举值。
	 */
	Language currentLanguage() const;

	/**
	 * @brief 按当前语言构造「关于」对话框（不显示、不阻塞）。
	 *
	 * @details
	 * 内容为五行 "名称: 取值"：程序版本 / CarbonUI 版本 / Qt 版本 / 构建时间 /
	 * 许可证。与界面测试共用本函数，保证"菜单弹的窗"与"测的窗"是同一段构造
	 * 代码。调用方负责 show()/exec() 并 delete（或交给父对象）。
	 *
	 * @param[in] parent 对话框父窗口，**可为 NULL**；非 NULL 时对话框居中于它。
	 * @return 新建的 CarbonUI 深色对话框，父对象为 @p parent；取值一定非 NULL。
	 */
	QDialog* createAboutDialog(QWidget* parent) const;

	/**
	 * @brief 按当前语言构造「关于 Qt」对话框（不显示、不阻塞）。
	 *
	 * @details
	 * 内容为四行 "名称: 取值"：Qt 版本 / 编译期版本 / 平台 / 许可，同样使用
	 * CarbonUI 深色对话框，避免系统浅色 QMessageBox 与整体风格割裂。
	 *
	 * @param[in] parent 对话框父窗口，**可为 NULL**；非 NULL 时对话框居中于它。
	 * @return 新建的 CarbonUI 深色对话框，父对象为 @p parent；取值一定非 NULL。
	 */
	QDialog* createAboutQtDialog(QWidget* parent) const;

  signals:
	/**
	 * @brief 语言切换完成信号。
	 * @param[in] lang 切换后的新语言。
	 */
	void languageChanged(MainWindow::Language lang);

  private:
	/// @name 内部装配（仅构造与语言切换路径调用）
	/// @{
	void createPages();       ///< 创建五个页面控件并加入 TabWidget（仅构造时调用）。
	void applyCarbonStyles(); ///< 应用容器级 CarbonUI 风格与工具栏动作（仅构造时调用一次）。
	void connectSignals();    ///< 建立菜单动作与页面状态消息的信号连接。
	void retranslateUi();     ///< 按当前语言覆写骨架文本并下发语言到各页面。
	QString langText(const char* zh, const char* en) const; ///< 按当前语言取双语字符串。

	/**
	 * @brief 弹出「关于」对话框（帮助菜单），返回后释放。
	 */
	void showAbout();

	/**
	 * @brief 弹出「关于 Qt」对话框（帮助菜单），返回后释放。
	 */
	void showAboutQt();
	/// @}

	/// @name 界面成员
	/// @{
	Ui::MainWindow* ui;                            ///< 由 MainWindow.ui 生成的骨架控件集合。
	Language lang_ = Language::Chinese;            ///< 当前界面语言，默认中文。
	QLabel* statusLang_ = nullptr;                 ///< 状态栏常驻语言指示标签。
	QLabel* statusVersion_ = nullptr;              ///< 状态栏常驻版本号标签。
	ButtonPage* buttonPage_ = nullptr;             ///< 第 0 页：按钮测试页。
	SelectionPage* selectionPage_ = nullptr;       ///< 第 1 页：选择与开关测试页。
	InputPage* inputPage_ = nullptr;               ///< 第 2 页：输入测试页。
	ListPage* listPage_ = nullptr;                 ///< 第 3 页：列表与折叠测试页。
	TextOverflowPage* textOverflowPage_ = nullptr; ///< 第 4 页：文本溢出测试页。
	DialogPage* dialogPage_ = nullptr;             ///< 第 5 页：对话框测试页。
	                                               /// @}
};
