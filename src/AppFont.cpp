/**
 * @file  AppFont.cpp
 * @brief 实现应用字体加载接口（见 AppFont.h 的接口文档）。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_core
 *
 * @details
 * 实现约定：所有失败分支统一收敛到"返回空字符串"，调用方只须判空，
 * 不需要区分失败原因；原因通过 qDebug 输出便于现场排查。
 */

#include "AppFont.h"

#include <QtGui/QFontDatabase>
#include <QtCore/QDebug>

namespace appcore
{

QString loadApplicationFont(const QString &fontPath)
{
    // 空路径直接短路，避免把空串传进 QFontDatabase 产生无意义告警
    if (fontPath.isEmpty()) {
        return QString();
    }

    const int id = QFontDatabase::addApplicationFont(fontPath);
    if (id == -1) {
        // 文件不存在或字体数据非法：QFontDatabase 约定返回 -1
        qDebug() << "Failed to load font from" << fontPath;
        return QString();
    }

    const QString family = QFontDatabase::applicationFontFamilies(id).value(0);
    if (family.isEmpty()) {
        // 注册成功但拿不到族名：视为失败，保持与加载失败一致的语义
        qDebug() << "Loaded font but no family returned for" << fontPath;
        return QString();
    }

    return family;
}

} // namespace appcore
