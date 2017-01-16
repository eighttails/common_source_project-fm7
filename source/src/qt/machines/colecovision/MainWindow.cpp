/*
 * Common Source code Project:
 * Ui->Qt->MainWindow for Coleco Vision .
 * (C) 2015 K.Ohta <whatisthis.sowhat _at_ gmail.com>
 *   License : GPLv2
 *   History :
 * Jan 14, 2015 : Initial, many of constructors were moved to qt/gui/menu_main.cpp.
 */

#include <QVariant>
#include <QtGui>
#include "commonclasses.h"
#include "menuclasses.h"
#include "emu.h"
#include "qt_main.h"

//QT_BEGIN_NAMESPACE
	

void META_MainWindow::setupUI_Emu(void)
{
	int i;
}

void META_MainWindow::retranslateUi(void)
{
	retranslateControlMenu("",  false);
	retranslateCartMenu(0, 1);

	retranslateMachineMenu();
	retranslateSoundMenu();
	retranslateScreenMenu();
	retranslateEmulatorMenu();
	retranslateUI_Help();
   
	this->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
	actionAbout->setText(QApplication::translate("MainWindow", "About...", 0));

	menuEmulator->setTitle(QApplication::translate("MainWindow", "Emulator", 0));
	menuMachine->setTitle(QApplication::translate("MainWindow", "Machine", 0));
	
	menuHELP->setTitle(QApplication::translate("MainWindow", "HELP", 0));
	actionHelp_AboutQt->setText(QApplication::translate("MainWindow", "About Qt", 0));
#ifdef USE_DEBUGGER
	actionDebugger[0]->setVisible(true);
	actionDebugger[1]->setVisible(false);
	actionDebugger[2]->setVisible(false);
	actionDebugger[3]->setVisible(false);
#endif
} // retranslateUi



META_MainWindow::META_MainWindow(USING_FLAGS *p, QWidget *parent) : Ui_MainWindow(p, parent)
{
	setupUI_Emu();
	retranslateUi();
}


META_MainWindow::~META_MainWindow()
{
}

//QT_END_NAMESPACE


