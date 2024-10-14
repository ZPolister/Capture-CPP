#include "CaptureApplication.h"
#include "qdebug.h"
#include <QFileDialog>
#include <QScreen>
#include <QMouseEvent>
#include <QBitmap>
#include <QComboBox>
#include "QTranslator"
#include <qt_windows.h>
#include <QSettings>
#include <QInputDialog>

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
    m_tray->setToolTip(tr("Dian=\nscreenshots:F1") + "F1");
	m_tray->show();
    QIcon messageIcon(":/image/main.ico");
    m_tray->showMessage(QStringLiteral(""), tr("Dian-capture\n"
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
	
    shortcut = new QxtGlobalShortcut(this);
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
    m_tray->setToolTip(tr("Dian-capture\nscreenshots:F1"));
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
    // 获取所有屏幕
    const QList<QScreen*> screens = QGuiApplication::screens();
    for (QScreen *screen : screens) {
        // 捕获每个屏幕的内容
        QPixmap fullPixmap = screen->grabWindow(0);

        ScreenView *screenView = new ScreenView(screen);
        screenView->setBackGroundPixmap(fullPixmap);

        // 设置 ScreenView 的位置
        QRect screenGeometry = screen->geometry();
        screenView->setGeometry(screenGeometry);


        // 显示 ScreenView
        screenView->show();
        views.append(screenView);
    }
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
        m_translator->load(":/translation/capture_en.qm");
		qApp->installTranslator(m_translator);
		ui.retranslateUi(this);
		languageTranslate();
	}
	else
	{
		m_translator = new QTranslator();
        m_translator->load(":/translation/capture_zh.qm");
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

void CaptureApplication::on_checkBox_stateChanged(int arg1)
{
    qDebug() << arg1;
    copyWithMd = arg1 == 2;
}


// 自启动设定
void CaptureApplication::setAutoRun(bool isStart)
{
    #define AUTO_RUN_KEY	"HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
    QString application_name = QApplication::applicationName();
    QSettings *settings = new QSettings(AUTO_RUN_KEY, QSettings::NativeFormat);
    if(isStart)
    {
        QString application_path = QApplication::applicationFilePath();
        settings->setValue(application_name, application_path.replace("/", "\\"));
    }
    else
    {
        settings->remove(application_name);
    }
}

void CaptureApplication::on_autoRuncheckBox_stateChanged(int arg1)
{
    this->setAutoRun(arg1 == 2);
}

void CaptureApplication::on_pushButton_hotKey_clicked() {

    bool ok;
    QString text = QInputDialog::getText(nullptr, QObject::tr("修改截图快捷键"),
                                         QObject::tr("输入快捷键（例如：Alt+F1 或 F1）:"),
                                         QLineEdit::Normal, "", &ok);

    if (ok && !text.isEmpty()) {
        // 用户输入的快捷键
        if (!shortcut->setShortcut(QKeySequence(text))) {
            m_tray->showMessage(QObject::tr("错误"), QObject::tr("快捷键已被占用：") + text, QSystemTrayIcon::Critical);
        } else {
            m_tray->showMessage(QObject::tr("成功"), QObject::tr("快捷键已修改：") + text, QSystemTrayIcon::Information);
            m_tray->setToolTip(tr("Dian=\nscreenshots:F1") + text);
        }
    }
}


bool copyWithMd = true;
QList<ScreenView*> views;



