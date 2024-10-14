#ifndef CAPTURE_APPLICATION_H
#define CAPTURE_APPLICATION_H

#include <QWidget>
#include "ui_CaptureApplication.h"
#include "imageview.h"
#include <QSystemTrayIcon>
#include "qxtglobalshortcut.h"
#include "screenview.h"
#include "QMenu"

class CaptureApplication : public QWidget
{
	Q_OBJECT

public:
	CaptureApplication(QWidget *parent = nullptr);
	~CaptureApplication();

public:
	void openImage();
	void screenShotCut();
	void trayMenuTrigged(QAction* action);
	void trayActivated(QSystemTrayIcon::ActivationReason reason);
	void switchLanguage(const QString &text);

private:
	void languageTranslate();
    void setAutoRun(bool isStart);

protected:
	void hideEvent(QHideEvent *event)     Q_DECL_OVERRIDE;
	void closeEvent(QCloseEvent *event)	  Q_DECL_OVERRIDE;

private slots:
    void on_checkBox_stateChanged(int arg1);

    void on_autoRuncheckBox_stateChanged(int arg1);

    void on_pushButton_hotKey_clicked();

private:
    Ui::CaptureApplication ui;
	std::vector<ImageView *> _arr_imgView;
	QString _file_path;
	QSystemTrayIcon *m_tray;
	QMenu *m_trayMenu;
	QAction *m_prefer;
	QAction *m_quit;
	QTranslator *m_translator;
    QxtGlobalShortcut* shortcut;
};

extern bool copyWithMd;
extern QList<ScreenView*> views;

#endif // CAPTURE_APPLICATION_H
