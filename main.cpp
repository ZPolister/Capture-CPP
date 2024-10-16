#include "CaptureApplication.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include "localconfig.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    CaptureApplication app;
    app.switchLanguage(localConfig.language);
    app.showLaunchMessage();
    QApplication::setQuitOnLastWindowClosed(false);
  	return QApplication::exec();
}
