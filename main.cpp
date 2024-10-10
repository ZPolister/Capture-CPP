#include "CaptureApplication.h"
#include <QtWidgets/QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CaptureApplication w;
	w.switchLanguage("zh");
	QApplication::setQuitOnLastWindowClosed(false);
	return QApplication::exec();
}
