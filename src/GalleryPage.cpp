/**
 * @file  GalleryPage.cpp
 * @brief 控件测试台页面基类实现（接口文档见 GalleryPage.h）。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 *
 * @details
 * 基类只承载公共设施：语言状态维护、双语文案换算与长文本样例。控件装配、
 * 风格应用与文本覆写全部由各页面子类自行完成，基类不做任何界面假设。
 *
 * 本源码基于 Mozilla Public License 2.0 发布：
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "GalleryPage.h"

namespace {

/// @brief 长文本测试样例（中文），用于固定尺寸控件的溢出观察。
const char* const kLongTextZh =
    "这是一段非常长的中文测试文本，用于验证 CarbonUI 控件在中英文切换时是否会超出控件边界";

/// @brief 长文本测试样例（英文），用于固定尺寸控件的溢出观察。
const char* const kLongTextEn = "This is a very long English text used to verify whether the "
                                "widget content overflows its bounds";

} // namespace

/**
 * @brief 构造页面基类部分：仅初始化语言状态（详见 GalleryPage.h）。
 * @param[in] parent 父控件指针，可为 NULL；为 NULL 时成为独立顶层控件。
 */
GalleryPage::GalleryPage(QWidget* parent) : QWidget(parent) {}

/**
 * @brief 更新页面语言并触发子类文本覆写（详见 GalleryPage.h）。
 * @param[in] lang 目标语言，取值为 ::GalleryPage::Language 枚举值。
 */
void GalleryPage::retranslateUi(Language lang) {
	lang_ = lang;
	retranslatePage();
}

/**
 * @brief 查询页面当前语言（详见 GalleryPage.h）。
 * @return 当前语言枚举值。
 */
GalleryPage::Language GalleryPage::language() const {
	return lang_;
}

/**
 * @brief 按当前语言取双语字符串（详见 GalleryPage.h）。
 * @param[in] zh 中文文案（UTF-8 字面量）。
 * @param[in] en 英文文案（UTF-8 字面量）。
 * @return 当前语言对应的文案。
 */
QString GalleryPage::langText(const char* zh, const char* en) const {
	return lang_ == Language::Chinese ? QString::fromUtf8(zh) : QString::fromUtf8(en);
}

/**
 * @brief 取长文本中文样例（详见 GalleryPage.h）。
 * @return 中文长文本（各页共用，内容恒定）。
 */
QString GalleryPage::longTextZh() {
	return QString::fromUtf8(kLongTextZh);
}

/**
 * @brief 取长文本英文样例（详见 GalleryPage.h）。
 * @return 英文长文本（各页共用，内容恒定）。
 */
QString GalleryPage::longTextEn() {
	return QString::fromUtf8(kLongTextEn);
}
