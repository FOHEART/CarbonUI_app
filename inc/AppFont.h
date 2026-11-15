/**
 * @file  AppFont.h
 * @brief 应用字体加载：把 TTF/OTF 字体文件注册进 Qt 字体库并返回字体族名。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_core
 *
 * @details
 * 本模块只封装"字体文件 -> 应用字体族名"这一条纯逻辑链路，供 main.cpp
 * 与单元测试共用。无全局状态、无平台 API 依赖，仅依赖 QtCore/QtGui 的
 * QFontDatabase，可在离屏（QT_QPA_PLATFORM=offscreen）环境下测试。
 *
 * 线程安全：QFontDatabase 在 Qt5 中要求调用前已存在 QApplication 实例；
 * 本模块不做加锁，约定只在主线程调用。
 */

#pragma once

#include <QtCore/QString>

namespace appcore
{

/**
 * @brief 加载字体文件并注册为 Qt 应用字体。
 *
 * @details
 * 内部走 QFontDatabase::addApplicationFont；注册成功后返回首个字体族名，
 * 该族名可直接交给 QApplication::setFont。失败路径（文件不存在、字体
 * 数据非法、注册成功但无族名）统一返回空字符串，不抛异常。
 * 副作用：成功时字体已进入应用字体库，重复加载同一文件由 Qt 去重。
 *
 * @param[in] fontPath 字体文件路径（相对工作目录或绝对路径），
 *                     编码随 QString；不可为 NULL（QString 值语义不存在空指针），
 *                     允许空串。
 *
 * @return 字体族名，取值为非空 UTF-16 字符串；加载失败、无族名或
 *         @p fontPath 为空串时返回空字符串。
 */
QString loadApplicationFont(const QString &fontPath);

} // namespace appcore
