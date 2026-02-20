/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "MainWindow.h"
#include "CarbonUIClass.h"
#include <QtGui/QFontDatabase>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QLabel>
#include <QtCore/QDebug>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtGui/QDoubleValidator>
#include <QtCore/QLocale>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QWidget>
#include <QtGui/QMouseEvent>
#include "groupboxcollapser.h"
#include <QtWidgets/QMenuBar>
#include <QtGui/QIcon>
#include <QtCore/QSize>
#include <QtWidgets/QTreeWidget>




MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	CarbonUIClass::applyScrollAreaStyle(ui.scrollArea);

	// Style the menu bar to match the dark horizontal layout shown
	CarbonUIClass::setupMenuBarStyle(ui.menuBar);
	CarbonUIClass::setupTabWidgetStyle(ui.tabWidget);
	// Add vertical scroll area to each tab page so content can scroll when it overflows

	qDebug() << ui.tabWidget->styleSheet();

	if (ui.treeWidget) {
		CarbonUIClass::applyTreeWidgetStyle(ui.treeWidget);
	}
	
	CarbonUIClass::applyTooltipStyle();

	// Set toolbar icon size and add down icon
	if (ui.mainToolBar) {
		CarbonUIClass::setupToolBarStyle(ui.mainToolBar);
		QAction* downAction = ui.mainToolBar->addAction(QIcon(":/images/config.png"), QString());
		downAction->setToolTip("打开");

		QAction* downAction2 = ui.mainToolBar->addAction(QIcon(":/images/down.png"), QString());
		downAction2->setToolTip("Down");
	}

	CarbonUIClass::setupComboBoxStyle(ui.comboBox);    

	CarbonUIClass::applyRadioStyle(ui.radioButton_2, CarbonUIClass::Radio_FourRound, 64);
	CarbonUIClass::applyRadioStyle(ui.radioButton_3, CarbonUIClass::Radio_FourRound, 64);
	CarbonUIClass::applyRadioStyle(ui.radioButton_4, CarbonUIClass::Radio_FourRound, 64);

	CarbonUIClass::applyRadioStyle(ui.radioButton_5, CarbonUIClass::Radio_LeftRound,64);
	CarbonUIClass::applyRadioStyle(ui.radioButton_6, CarbonUIClass::Radio_Rectangle, 64);
	CarbonUIClass::applyRadioStyle(ui.radioButton_7, CarbonUIClass::Radio_RightRound, 64);

	// apply line edit style and behavior for hover/focus/select-all
	CarbonUIClass::applyLineEditStyle_numberInput(ui.lineEdit, 200);

	// apply custom checkbox style
	CarbonUIClass::applyCheckStyle(ui.checkBox_2);
	CarbonUIClass::applyCheckStyle(ui.checkBox_3);

	CarbonUIClass::applyButtonStyle(ui.pushButton, CarbonUIClass::Shape_RoundAll);
	CarbonUIClass::applyButtonStyle(ui.pushButton_5, CarbonUIClass::Shape_RoundAll);
	// style pushButton_7: rounded left corners, square right corners
	CarbonUIClass::applyButtonStyle(ui.pushButton_7, CarbonUIClass::Shape_LeftRound);
	CarbonUIClass::applyButtonStyle(ui.pushButton_9, CarbonUIClass::Shape_Rectangle);
	CarbonUIClass::applyButtonStyle(ui.pushButton_10, CarbonUIClass::Shape_RightRound);

	// tabBar scroller styling moved to styles/app.qss

	ui.tabWidget->setCurrentIndex(0);
	// Ensure groupBox_1 has only a bottom border (1px white) and its internal QLabel use white and 11pt
	if (ui.groupBox_1)
	{
// 		ui.groupBox_1->setStyleSheet(
// 			"QGroupBox { border: none; border-bottom: 1px solid #000000; } "
// 			"QLabel { color: #ffffff; font-size: 11pt; } "
// 		);
	}
	if (ui.groupBox_2)
	{
		//ui.groupBox_2->setStyleSheet("QGroupBox { border: none; border-bottom: 1px solid #000000; } QLabel { color: #ffffff; font-size: 11pt; }");
	}

	QGroupBoxCollapser::installIntoGroupBox(ui.groupBox_1);
	QGroupBoxCollapser::installIntoGroupBox(ui.groupBox_2);
}


MainWindow::~MainWindow()
{
}

