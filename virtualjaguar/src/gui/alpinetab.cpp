//
// alpinetab.cpp: "Alpine" tab on the settings dialog
//
// Part of the Virtual Jaguar Project
// (C) 2011 Underground Software
// See the README and GPLv3 files for licensing and warranty information
//
// JLH = James Hammons <jlhamm@acm.org>
//
// WHO  WHEN        WHAT
// ---  ----------  ------------------------------------------------------------
// JLH  07/15/2011  Created this file

#include "alpinetab.h"


AlpineTab::AlpineTab(QWidget * parent/*= 0*/): QWidget(parent)
{
	QLabel * label1 = new QLabel("ROM to load:");
	QLabel * label2 = new QLabel("ABS to load:");
//	QLabel * label3 = new QLabel("EEPROMs:");
//	QLabel * label4 = new QLabel("Software:");

	edit1 = new QLineEdit("");
	edit2 = new QLineEdit("");
//	edit3 = new QLineEdit("");
//	edit4 = new QLineEdit("");
	edit1->setPlaceholderText("ROM to load when Virtual Jaguar loads");
	edit2->setPlaceholderText("ABS to load when Virtual Jaguar loads");
//	edit3->setPlaceholderText("EEPROM path");
//	edit4->setPlaceholderText("Software path");

	QVBoxLayout * layout1 = new QVBoxLayout;
	layout1->addWidget(label1);
	layout1->addWidget(label2);
//	layout1->addWidget(label3);
//	layout1->addWidget(label4);

	QVBoxLayout * layout2 = new QVBoxLayout;
	layout2->addWidget(edit1);
	layout2->addWidget(edit2);
//	layout2->addWidget(edit3);
//	layout2->addWidget(edit4);

	QHBoxLayout * layout3 = new QHBoxLayout;
	layout3->addLayout(layout1);
	layout3->addLayout(layout2);

	QVBoxLayout * layout4 = new QVBoxLayout;
	layout4->addLayout(layout3);

	// Checkboxes...
	writeROM         = new QCheckBox(tr("Allow writes to cartridge ROM"));
//	useDSP             = new QCheckBox(tr("Enable DSP"));
//	useHostAudio       = new QCheckBox(tr("Enable audio playback"));
//	useUnknownSoftware = new QCheckBox(tr("Allow unknown software in file chooser"));
// Currently, this is unused, so let's signal this to the user:
	writeROM->setDisabled(true);

	layout4->addWidget(writeROM);
//	layout4->addWidget(useDSP);
//	layout4->addWidget(useHostAudio);
//	layout4->addWidget(useUnknownSoftware);

	setLayout(layout4);
}

AlpineTab::~AlpineTab()
{
}
