//
// cpubrowser.h: Jaguar CPU browser
//
// by James Hammons
// (C) 2012 Underground Software
//

#ifndef __CPUBROWSER_H__
#define __CPUBROWSER_H__

#include <QtGui>
#include <stdint.h>

class CPUBrowserWindow: public QWidget
{
	Q_OBJECT

	public:
		CPUBrowserWindow(QWidget * parent = 0);


	public slots:
//		void DefineAllKeys(void);
		void RefreshContents(void);
		void HandleBPM(bool);
		void HandleBPMAddress(const QString &);

	protected:
		void keyPressEvent(QKeyEvent *);

	private:
		QVBoxLayout * layout;
//		QTextBrowser * text;
		QLabel * text;
		QPushButton * refresh;
		QCheckBox * bpm;
		QLineEdit * bpmAddress;

//		int32_t memBase;
};

#endif	// __CPUBROWSER_H__
