/**
 * @file  GalleryPage.h
 * @brief 控件测试台页面基类声明：语言状态 + 双语文案 + 状态栏消息信号的公共设施。
 * @author FOHEART
 * @date  2026-11-15
 * @version 1.0.0
 * @ingroup carbonui_app_core
 *
 * @details
 * 主界面的每个分类测试 Tab 对应一个 @ref GalleryPage 子类（每类一个独立的
 * .h/.cpp 文件对）。子类在构造函数内完成控件装配、CarbonUI 风格应用与页面内
 * 信号连接；文本覆写统一走 @ref retranslateUi 入口。
 *
 * 基类约定：
 *  - 语言枚举 ::GalleryPage::Language 是全工程的界面语言唯一来源，
 *    MainWindow 经别名复用；
 *  - 子类交互产生的状态栏消息经 @ref statusMessage 信号上抛，由主窗口统一
 *    接入状态栏（页面本身不持有状态栏引用）；
 *  - 子类只重写 @ref retranslatePage 覆写文本，不感知语言切换的调度细节。
 *
 * 线程安全：仅允许在 GUI 主线程使用。
 */

#ifndef CARBONUI_APP_GALLERYPAGE_H
#define CARBONUI_APP_GALLERYPAGE_H

#include <QtWidgets/QWidget>

/**
 * @brief 分类测试页公共基类：语言状态、双语文案与状态栏消息信号。
 *
 * @details
 * 抽象类，不能直接实例化。子类构造完成后即具备完整控件树与 CarbonUI 风格；
 * 首次文本覆写由主窗口在装配末尾调用 @ref retranslateUi 完成。
 */
class GalleryPage : public QWidget {
	Q_OBJECT

  public:
	/**
	 * @brief 界面语言枚举（全工程唯一来源，MainWindow 以别名复用）。
	 *
	 * @details 当前支持中英文双语；取值由主窗口经 @ref retranslateUi 下发。
	 */
	enum class Language : quint8 {
		Chinese, ///< 简体中文。
		English  ///< English。
	};

	/**
	 * @brief 构造页面基类部分。
	 * @param[in] parent 父控件指针，可为 NULL；为 NULL 时成为独立顶层控件。
	 */
	explicit GalleryPage(QWidget* parent = nullptr);

	/**
	 * @brief 更新页面语言并触发 @ref retranslatePage 文本覆写。
	 * @param[in] lang 目标语言，取值为 ::GalleryPage::Language 枚举值。
	 */
	void retranslateUi(Language lang);

	/**
	 * @brief 查询页面当前语言。
	 * @return 当前语言枚举值。
	 */
	Language language() const;

  signals:
	/**
	 * @brief 页面交互产生的状态栏临时消息。
	 * @param[in] message 已按当前语言拼装好的完整文案。
	 */
	void statusMessage(const QString& message);

  protected:
	/**
	 * @brief 按当前语言 @ref lang_ 覆写页面内全部静态文本（子类必须实现）。
	 * @details 调用时机：@ref retranslateUi 入口内部；动态内容（下拉项、
	 *          树条目）也应在此重建。
	 */
	virtual void retranslatePage() = 0;

	/**
	 * @brief 按当前语言取双语字符串。
	 * @param[in] zh 中文文案（UTF-8 字面量）。
	 * @param[in] en 英文文案（UTF-8 字面量）。
	 * @return 当前语言对应的文案。
	 */
	QString langText(const char* zh, const char* en) const;

	/// 长文本测试样例（中文）：各页共用的溢出观察素材。
	static QString longTextZh();

	/// 长文本测试样例（英文）：各页共用的溢出观察素材。
	static QString longTextEn();

	Language lang_ = Language::Chinese; ///< 当前页面语言，由 @ref retranslateUi 维护。
};

#endif // CARBONUI_APP_GALLERYPAGE_H
