/**
 * @file  MainWindow.h
 * @brief 应用主窗口声明：装配 CarbonUI 风格控件并持有 .ui 生成的界面。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_core
 *
 * @details
 * MainWindow 只做界面装配（样式应用、折叠器安装），不含业务算法；
 * 可测逻辑统一放在 core/ 下的纯逻辑模块。
 * ui 控件集以指针持有（前向声明避免在头文件包含生成的 ui_MainWindow.h，
 * 使 applib 的消费者无需看到 AUTOUIC 输出目录）。
 * 生命周期：栈上创建，析构时手工释放 ui 对象（控件本身随父子树释放）。
 * 线程安全：仅允许在 GUI 主线程构造与使用。
 */

#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QMenuBar>

namespace Ui
{
class MainWindow; ///< 由 MainWindow.ui 经 uic 生成的界面类（完整定义在 ui_MainWindow.h）。
}

/**
 * @brief 主窗口：加载 MainWindow.ui 并对 tab/tree/菜单栏/工具栏等控件应用 CarbonUI 风格。
 *
 * @details
 * 构造函数内完成全部装配工作（setupUi + CarbonUIClass 系列 apply/setup 调用 +
 * QGroupBoxCollapser 安装），构造完成后窗口即可显示使用。
 * 无共享状态，线程亲和性为 GUI 主线程。
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造主窗口：装配 .ui 界面并应用 CarbonUI 风格。
     * @param[in] parent 父窗口指针，可为 NULL；为 NULL 时成为独立顶层窗口。
     */
    MainWindow(QWidget *parent = nullptr);

    /** @brief 析构主窗口：释放 ui 对象，子控件随 Qt 父子树自动释放。 */
    ~MainWindow();

private:
    /// @name 界面成员
    /// @{
    Ui::MainWindow *ui; ///< 由 MainWindow.ui 生成的控件集合，构造时创建、析构时释放。
    /// @}
};
