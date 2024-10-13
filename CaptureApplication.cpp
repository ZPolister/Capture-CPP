#include "CaptureApplication.h"
#include "screenview.h"
#include <QFileDialog>
#include <QScreen>
#include <QMouseEvent>
#include <QBitmap>
#include <QComboBox>
#include "QTranslator"
#include "qxtglobalshortcut.h"
#include <qt_windows.h>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#define SHOWWINDOWTOP \
::SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);\
::SetWindowPos(HWND(this->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);\
this->show();\
this->activateWindow();


CaptureApplication::CaptureApplication(QWidget *parent)
	: QWidget(parent), m_translator(nullptr)
{
	ui.setupUi(this);
	_file_path = QStringLiteral(".");

    m_tray = new QSystemTrayIcon(QIcon(":/image/main.ico"), this);
    m_tray->setToolTip(tr("Dian=\nscreenshots:F1"));
	m_tray->show();
    QIcon messageIcon(":/image/main.ico");
    m_tray->showMessage(QStringLiteral(""), QStringLiteral("Dian-capture\n"
                                                           "截屏:F1\n"
                                                           "贴图:鼠标中键"));

	m_trayMenu = new QMenu(this);
	m_prefer = new QAction(tr("Preference"), m_trayMenu);
	m_trayMenu->addAction(m_prefer);
	m_quit = new QAction(tr("Quit"), m_trayMenu);
	m_trayMenu->addAction(m_quit);
	m_tray->setContextMenu(m_trayMenu);

	QStringList languages;
	languages << QStringLiteral("中文") << QStringLiteral("English");
    ui.comboBox_langue->addItems(languages);
	
	QxtGlobalShortcut* shortcut = new QxtGlobalShortcut(this);
	if(!shortcut->setShortcut(QKeySequence(QLatin1String("F1"))))
        m_tray->showMessage(tr("Error"), tr("register shorcut F1 failed"), QSystemTrayIcon::Critical);

	connect(shortcut, &QxtGlobalShortcut::activated, this, &CaptureApplication::screenShotCut);
	connect(ui.pushButton_open,     &QPushButton::clicked, this, &CaptureApplication::openImage);
	connect(ui.pushButton_shortcut, &QPushButton::clicked, this, &CaptureApplication::screenShotCut);
	connect(m_trayMenu, &QMenu::triggered, this, &CaptureApplication::trayMenuTrigged);
	connect(m_tray, &QSystemTrayIcon::activated, this, &CaptureApplication::trayActivated);
	connect(ui.comboBox_langue, &QComboBox::currentTextChanged, this, &CaptureApplication::switchLanguage);
}


void CaptureApplication::languageTranslate()
{
	m_prefer->setText(tr("Preference"));
	m_quit->setText(tr("Quit"));
	m_tray->setToolTip(tr("Snappit\nscreenshots:F1"));
}

CaptureApplication::~CaptureApplication()
{
	for(int i = 0; i < _arr_imgView.size(); i++)
	{
		ImageView *imgView = _arr_imgView[i];
		if(imgView != NULL)
		{
			imgView->close();
			delete imgView;
			imgView = NULL;
		}
	}
}

void CaptureApplication::openImage()
{
	
	QString img_path = QFileDialog::getOpenFileName(this, tr("Choose Image"), _file_path, "Image files (*.bmp *.jpg *.png);;All files (*.*)");
	if(!img_path.isEmpty())
	{
		QFileInfo file(img_path);
		_file_path = file.absolutePath();
		ImageView *imgView = new ImageView();
		imgView->setImage(img_path);
		imgView->show();
		_arr_imgView.push_back(imgView);
	}
}

void CaptureApplication::screenShotCut()
{
	QScreen *screen = QGuiApplication::primaryScreen();
	QPixmap fullPixmap = screen->grabWindow(0);
	ScreenView *screenView = new ScreenView();
	screenView->setBackGroundPixmap(fullPixmap);  //传递全屏背景图片
	screenView->show();
}


void CaptureApplication::trayMenuTrigged(QAction* action)
{
	if (action == m_prefer)
	{
		SHOWWINDOWTOP;
	}
	else if (action == m_quit)
	{
		qApp->exit();
	}
}

void CaptureApplication::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
	case QSystemTrayIcon::Trigger:
		SHOWWINDOWTOP;
		break;
	default:
		break;
	}
}

void CaptureApplication::switchLanguage(const QString &text)
{
	if (text == QLatin1String("English"))
	{
		m_translator = new QTranslator();
		m_translator->load(":/translation/qtt_en.qm");
		qApp->installTranslator(m_translator);
		ui.retranslateUi(this);
		languageTranslate();
	}
	else
	{
		m_translator = new QTranslator();
		m_translator->load(":/translation/qtt_zh.qm");
		qApp->installTranslator(m_translator);
		ui.retranslateUi(this);
		languageTranslate();
	}
}

void CaptureApplication::hideEvent(QHideEvent *event)
{
	if (m_tray->isVisible())
	{
		this->hide();
		event->ignore();
	}
}

void CaptureApplication::closeEvent(QCloseEvent *event)
{
	if (m_tray->isVisible())
	{
		this->hide(); //隐藏窗口
		event->ignore(); //忽略事件
	}
}
