/**
 * @file  appfont_test.cpp
 * @brief 纯逻辑层（appcore::loadApplicationFont）单元测试，框架为 GoogleTest。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_tests
 *
 * @details
 * 覆盖策略
 *  - 核心路径：真实字体文件加载（金标准 = 工程自带的 HarmonyOS Sans 字体）；
 *  - 异常场景：文件不存在、空路径，统一收敛为"返回空字符串"。
 *
 * 本层不创建窗口，通过离屏平台（QT_QPA_PLATFORM=offscreen）运行，
 * 字体文件路径由 CMake 编译定义 CARBONUI_APP_TEST_FONT 注入。
 */

#include "AppFont.h"

#include <gtest/gtest.h>
#include <QtWidgets/QApplication>

/** @brief 单元测试用字体路径，由 CMake 编译定义注入（仓库内真实 TTF 文件）。 */
#ifndef CARBONUI_APP_TEST_FONT
#define CARBONUI_APP_TEST_FONT "fonts/HarmonyOS_Sans_SC_Bold.ttf"
#endif

/// @cond DOC_HIDDEN
namespace {

/**
 * @brief 单元测试入口：先让 GoogleTest 吃掉 --gtest_* 参数，再创建 QApplication。
 * @details AppFont 依赖 QFontDatabase，Qt5 要求调用前已存在 QApplication 实例；
 *          测试经 ctest 设置 QT_QPA_PLATFORM=offscreen 离屏运行。
 * @param[in] argc 命令行参数个数，由运行时传入。
 * @param[in] argv 命令行参数数组，不可为 NULL。
 * @return RUN_ALL_TESTS() 的结果，0 表示全部用例通过。
 */
int runUnitTests(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);   // 先吃掉 gtest 参数
    QApplication app(argc, argv);             // 再交给 Qt，避免它误吞 --gtest_*
    return RUN_ALL_TESTS();
}

} // namespace
/// @endcond

/**
 * @brief 转发入口：保持 main 与 Doxygen 参数文档一一对应。
 * @param[in] argc 命令行参数个数，由运行时传入。
 * @param[in] argv 命令行参数数组，不可为 NULL。
 * @return 见 runUnitTests；0 表示全部用例通过。
 */
int main(int argc, char **argv)
{
    return runUnitTests(argc, argv);
}

/// @cond DOC_HIDDEN

/**
 * @test   AppFont.LoadsValidFontReturnsNonEmptyFamily
 * @brief  核心路径：加载工程自带的有效 TTF 字体时返回非空字体族名。
 * @details 目的：防止字体注册链路回归（addApplicationFont / 字体族名提取被改坏）。
 *          设计思路：用仓库内真实字体文件做金标准输入（路径由 CMake 注入），
 *          期望值只断言"非空族名"，避免把字体内部族名字符串硬编码进测试。
 * @expected loadApplicationFont(有效 ttf) 返回非空 QString。
 * @priority P0
 */
TEST(AppFont, LoadsValidFontReturnsNonEmptyFamily)
{
    // Arrange：有效字体路径来自编译定义
    const QString path = QStringLiteral(CARBONUI_APP_TEST_FONT);

    // Act
    const QString family = appcore::loadApplicationFont(path);

    // Assert
    EXPECT_FALSE(family.isEmpty());
}

/**
 * @test   AppFont.MissingFontFileReturnsEmptyFamily
 * @brief  异常场景：字体文件不存在时返回空字符串，不崩溃、不抛异常。
 * @details 目的：锁住"失败统一收敛为空串"的约定，防止实现改成抛异常或
 *          返回带垃圾值的族名污染 QApplication::setFont。
 *          设计思路：使用必然不存在的路径（含非法目录层级）构造失败分支。
 * @expected 返回值 isEmpty() == true。
 * @priority P0
 */
TEST(AppFont, MissingFontFileReturnsEmptyFamily)
{
    // Arrange：构造必然不存在的路径
    const QString path = QStringLiteral(":no/such/dir/missing_font.ttf");

    // Act
    const QString family = appcore::loadApplicationFont(path);

    // Assert
    EXPECT_TRUE(family.isEmpty());
}

/**
 * @test   AppFont.EmptyPathReturnsEmptyFamily
 * @brief  边界：空路径输入被直接短路，返回空字符串。
 * @details 目的：防止空串传入 QFontDatabase 触发运行期告警或未定义行为。
 *          设计思路：空路径是最小合法输入，单独成对（与非法路径对照）验证
 *          "空输入 -> 空输出"契约。
 * @expected 返回值 isEmpty() == true。
 * @priority P1
 */
TEST(AppFont, EmptyPathReturnsEmptyFamily)
{
    // Arrange：空路径
    const QString path = QString();

    // Act
    const QString family = appcore::loadApplicationFont(path);

    // Assert
    EXPECT_TRUE(family.isEmpty());
}

/// @endcond
