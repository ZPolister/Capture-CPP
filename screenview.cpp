#include "screenview.h"
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QMenu>
#include <Qpainter>
#include <Qboxlayout>
#include <QPushButton>
#include <Qscreen>
#include <QClipboard>
#include <QDateTime>
#include <QFileDialog>
#include <QFontMetrics>
#include <QLabel>
#include <QColorDialog>
#include <QSplitter>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QProgressDialog>
#include <QTimer>
#include "CaptureApplication.h"
#include "ResultWindow.h"
#include "imageview.h"

static const QString s_normalStyle = QStringLiteral(
    "QPushButton { background-color: rgb(255, 255, 255);  margin: 5px;border-radius:10px;}"
    "QPushButton:hover { background-color: #F5F5F5;margin: 5px; color: rgb(0, 0, 0);border: 1px solid #DCDCDC;border-radius:10px; }"
    "QPushButton:pressed { background-color: #F5F5F5;margin: 5px; color: rgb(255, 255, 255);border: 1px solid #DCDCDC;border-radius:10px; }"
    );
static const QString s_pressStyle  = QStringLiteral("margin: 5px;border: 1px solid #DCDCDC;background-color:#F5F5F5;border-radius:10px;");
static const QString s_brushStyle = QStringLiteral(    "QPushButton { background-color: rgb(255, 255, 255);  margin: 5px;border-radius:10px;}"
    "QPushButton:hover { background-color: #F5F5F5;margin: 5px; color: rgb(0, 0, 0);border: 1px solid #DCDCDC;border-radius:10px; }"
    "QPushButton:pressed { background-color: #F5F5F5;margin: 5px; color: rgb(255, 255, 255);border: 1px solid #DCDCDC;border-radius:10px; }");
static const QString toolbar_Style = QStringLiteral("background-color:rgb(255, 255, 255); border-radius:10px;");
const QPen penScreenShotBound = QPen(Qt::gray, 5, Qt::SolidLine);

ScreenView::ScreenView(QWidget *parent)
	: QWidget(parent)
{
	init();	
	initLabel();
	initColorBar();
	initToolBar();
	setMouseTracking(true);       //鼠标移动捕捉
	raise();                      //软置顶
	showFullScreen();             //画布全屏显示
}

ScreenView::ScreenView(QScreen* screen, QWidget *parent)
    : QWidget(parent)
{
    init();
    initLabel();
    initColorBar();
    initToolBar();
    setMouseTracking(true);       //鼠标移动捕捉
    raise();                      //软置顶
    showFullScreen();

    this->screen = screen;
}
ScreenView::~ScreenView()
{
    views.removeAll(this);
}



void ScreenView::setBackGroundPixmap(QPixmap &px)
{
	_fullPixmap = px;
}

QRectF ScreenView::getRectF(QPointF p1, QPointF p2)
{
	qreal x1, y1, x2, y2;
	if (p1.x() < p2.x())
	{
		x1 = p1.x();
		x2 = p2.x();
	}
	else
	{
		x1 = p2.x();
		x2 = p1.x();
	}

	if (p1.y() < p2.y())
	{
		y1 = p1.y();
		y2 = p2.y();
	}
	else
	{
		y1 = p2.y();
		y2 = p1.y();
	}

	QPointF ps(x1, y1);
	QPointF pe(x2, y2);
	QRectF rect(ps, pe);
	return rect;
}

void ScreenView::setTopLeftAndButtomRight(QPointF &ptTL, QPointF &ptBR)
{
	qreal x1, y1, x2, y2;
	if (ptTL.x() < ptBR.x())
	{
		x1 = ptTL.x();
		x2 = ptBR.x();
	}
	else
	{
		x1 = ptBR.x();
		x2 = ptTL.x();
	}

	if (ptTL.y() < ptBR.y())
	{
		y1 = ptTL.y();
		y2 = ptBR.y();
	}
	else
	{
		y1 = ptBR.y();
		y2 = ptTL.y();
	}

	ptTL.setX(x1);
	ptTL.setY(y1);
	ptBR.setX(x2);
	ptBR.setY(y2);
}

CursorLocation ScreenView::caputerCursor(QRectF rect, qreal x, qreal y)
{
	QPointF ptTL = rect.topLeft();
	QPointF ptBR = rect.bottomRight();
	QPointF ptTR = rect.topRight();
	QPointF ptBL = rect.bottomLeft();

	if (x < ptTL.x() + MARGIN)
	{//left
		if (y < ptTL.y() + MARGIN)
		{
			return CursorLocation::TOPLEFT;
		}
		else if (y > ptBL.y() - MARGIN)
		{
			return CursorLocation::BUTTOMLEFT;
		}
		else
		{
			return CursorLocation::LEFT;
		}
	}
	else if (x > ptTR.x() - MARGIN)
	{//right
		if (y < ptTR.y() + MARGIN)
		{
			return CursorLocation::TOPRIGHT;
		}
		else if (y > ptBR.y() - MARGIN)
		{
			return CursorLocation::BUTTOMRIGHT;
		}
		else
		{
			return CursorLocation::RIGHT;
		}
	}
	else
	{//mid
		if (y < ptTL.y() + MARGIN)
		{
			return CursorLocation::TOP;
		}
		else if (y > ptBL.y() - MARGIN)
		{
			return CursorLocation::BUTTOM;
		}
		else
		{
			return CursorLocation::MIDDLE;
		}
	}
}

void ScreenView::copyImage()
{
	screenCapture(_shortArea);
	_clipboard->setPixmap(_shotPixmap);
	this->close();
}

void ScreenView::saveImage()
{
	screenCapture(_shortArea);
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString current_date = current_date_time.toString("yyyyMMdd_hhmmss");
	QString save_file = "ScreenShot_" + current_date + ".jpg";

	QString file_name = QFileDialog::getSaveFileName(this, 
													 QStringLiteral("保存图片"), 
													 "./" + save_file, 
													 "JPEG File (*.jpg);;BMP File (*.bmp);;PNG File (*.png)");
	if (file_name.isEmpty())
		return;

	_shotPixmap.save(file_name);

	this->close();
}

void ScreenView::drawLine()
{
	if (DrawEditFlag::DRAWLINE != _draw_edit_flag)
	{
		_draw_edit_flag = DrawEditFlag::DRAWLINE;
		_btn_drawLine->setStyleSheet(s_pressStyle);
		_btn_drawRect->setStyleSheet(s_normalStyle);
		_btn_drawEllipse->setStyleSheet(s_normalStyle);
		_btn_drawText->setStyleSheet(s_normalStyle);
		showColorBar();
	}
	else
	{
		_draw_edit_flag = DrawEditFlag::DRAWDRAG;
		_btn_drawLine->setStyleSheet(s_normalStyle);
		hideColorBar();
	}
}

void ScreenView::uploadPicGo() {
    screenCapture(_shortArea);
    _clipboard->setPixmap(_shotPixmap);

    QNetworkAccessManager netManager;

    QString url = "http://localhost:36677/upload";
    QNetworkRequest request = QNetworkRequest(QUrl(url));
    QByteArray responseData;
    QEventLoop eventLoop;
    connect(&netManager , SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    QNetworkReply* initReply = netManager.post(request, QString("").toUtf8());
    eventLoop.exec();
    responseData = initReply->readAll();
    // qDebug() << responseData;

    // 解析json
    QString imageUrl;
    QJsonParseError json_error;
    QJsonDocument doucment = QJsonDocument::fromJson(responseData, &json_error);
    if (json_error.error == QJsonParseError::NoError) {
        if (doucment.isObject()) {
            const QJsonObject object_data = doucment.object();
            qDebug() << object_data;
            if (object_data.contains("success") && object_data.value("success").toBool())
            {
                QJsonArray jsonArray = object_data.value("result").toArray();
                imageUrl = jsonArray.at(0).toString();
            }
            else
            {
                qDebug() << "上传失败";
            }
        }
    } else {
        qDebug() << "failed upload";
    }

    // 打开了复制为md格式，构造md
    if (copyWithMd) {
        imageUrl = "![](" + imageUrl + ")";
    }
    _clipboard->setText(imageUrl);
    qDebug() << imageUrl;

    this->close();

}

void ScreenView::drawRect()
{
	if (DrawEditFlag::DRAWRECT != _draw_edit_flag)
	{
		_draw_edit_flag = DrawEditFlag::DRAWRECT;
		_btn_drawRect->setStyleSheet(s_pressStyle);
		_btn_drawLine->setStyleSheet(s_normalStyle);
		_btn_drawEllipse->setStyleSheet(s_normalStyle);
		_btn_drawText->setStyleSheet(s_normalStyle);
		showColorBar();
	}
	else
	{
		_draw_edit_flag = DrawEditFlag::DRAWDRAG;
		_btn_drawRect->setStyleSheet(s_normalStyle);
		hideColorBar();
	}
}

void ScreenView::drawEllipse()
{
	if (DrawEditFlag::DRAWCIRCLE != _draw_edit_flag)
	{
		_draw_edit_flag = DrawEditFlag::DRAWCIRCLE;
		_btn_drawEllipse->setStyleSheet(s_pressStyle);
		_btn_drawLine->setStyleSheet(s_normalStyle);
		_btn_drawRect->setStyleSheet(s_normalStyle);
		_btn_drawText->setStyleSheet(s_normalStyle);
		showColorBar();
	}
	else
	{
		_draw_edit_flag = DrawEditFlag::DRAWDRAG;
		_btn_drawEllipse->setStyleSheet(s_normalStyle);
		hideColorBar();
	}
}

void ScreenView::drawTextStatus()
{
	if (DrawEditFlag::DRAWTEXT != _draw_edit_flag)
	{
		_draw_edit_flag = DrawEditFlag::DRAWTEXT;
		_btn_drawText->setStyleSheet(s_pressStyle);
		_btn_drawLine->setStyleSheet(s_normalStyle);
		_btn_drawRect->setStyleSheet(s_normalStyle);
		_btn_drawEllipse->setStyleSheet(s_normalStyle);
		showColorBar();
	}
	else
	{
		_draw_edit_flag = DrawEditFlag::DRAWDRAG;
		_btn_drawText->setStyleSheet(s_normalStyle);
		hideColorBar();
	}
}

void ScreenView::colorItemChanged(const QColor &color)
{
	Q_ASSERT(_cur_coloritem);
	_cur_coloritem->setColor(color);
	_pen_color = color;
	if(_bIsBrushed)
		_brush.setColor(_pen_color);
}

void ScreenView::colorSelection()
{
	QColorDialog dia(this);
	dia.setWindowTitle(QStringLiteral("选择颜色"));
	dia.setCurrentColor(_cur_coloritem->getColor());
	if (dia.exec() == QColorDialog::Accepted)
	{
		QColor color = dia.selectedColor();
		_cur_coloritem->setColor(color);
		_pen_color = color;
	}
}

void ScreenView::pointSizeChanged(int point_size)
{
	_point_size = point_size;
}

void ScreenView::btnBrushClicked()
{
	if(!_bIsBrushed)
	{
		_brush.setStyle(Qt::SolidPattern);
		_brush.setColor(_pen_color);
		_btn_brush->setStyleSheet(s_pressStyle);
		_bIsBrushed = true;
	}
	else
	{
		_brush.setStyle(Qt::NoBrush);
		_btn_brush->setStyleSheet(s_brushStyle);
		_bIsBrushed = false;
	}
}

void ScreenView::init()
{
	_ptS.rx() = -10;
	_ptS.ry() = -10;
	_ptE.rx() = -10;
	_ptE.ry() = -10;
	_bIsDrawLineEnd = false;
	_bIsBrushed = false;

	_line_list.clear();
	_rect_list.clear();
	_ellipse_list.clear();
	_text_list.clear();

	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	setCursor(Qt::ArrowCursor);
	_draw_flag = DrawStatus::WAITDRAW;
	_curlocation = CursorLocation::INVALID;
	_draw_edit_flag = DrawEditFlag::DRAWDRAG;
	_pen_color = QColor(Qt::black);
	_point_size = 2;

	QRect screenGeometry = QApplication::desktop()->screenGeometry(this);
	_screen_width = screenGeometry.width();
	_screen_height = screenGeometry.height();
	_clipboard = QApplication::clipboard();   //获取系统剪贴板指针

}
// 初始化颜色条
void ScreenView::initColorBar()
{
    _colorbar = new QWidget(this); // 创建颜色条的主窗口

    // 创建第一组颜色项
    ColorItem *coloritem1_1 = new ColorItem(QColor(0, 0, 0));
    ColorItem *coloritem1_2 = new ColorItem(QColor(127, 127, 127));
    ColorItem *coloritem1_3 = new ColorItem(QColor(136, 0, 21));
    ColorItem *coloritem1_4 = new ColorItem(QColor(237, 28, 36));
    ColorItem *coloritem1_5 = new ColorItem(QColor(255, 127, 39));
    ColorItem *coloritem1_6 = new ColorItem(QColor(255, 242, 0));
    ColorItem *coloritem1_7 = new ColorItem(QColor(34, 177, 76));
    ColorItem *coloritem1_8 = new ColorItem(QColor(0, 162, 232));
    ColorItem *coloritem1_9 = new ColorItem(QColor(63, 72, 204));
    ColorItem *coloritem1_10 = new ColorItem(QColor(163, 73, 164));
    QHBoxLayout *hBoxLayout1 = new QHBoxLayout(); // 创建第一个水平布局

    // 添加颜色项到布局
    hBoxLayout1->addWidget(coloritem1_1);
    hBoxLayout1->addWidget(coloritem1_2);
    hBoxLayout1->addWidget(coloritem1_3);
    hBoxLayout1->addWidget(coloritem1_4);
    hBoxLayout1->addWidget(coloritem1_5);
    hBoxLayout1->addWidget(coloritem1_6);
    hBoxLayout1->addWidget(coloritem1_7);
    hBoxLayout1->addWidget(coloritem1_8);
    hBoxLayout1->addWidget(coloritem1_9);
    hBoxLayout1->addWidget(coloritem1_10);
    hBoxLayout1->setMargin(2); // 设置边距
    hBoxLayout1->setSpacing(2); // 设置间距

    // 连接颜色项的点击信号到处理函数
    connect(coloritem1_1,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem1_2,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem1_3,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem1_4,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem1_5,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem1_6,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem1_7,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem1_8,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem1_9,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem1_10, SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));

    // 创建第二组颜色项
    ColorItem *coloritem2_1 = new ColorItem(QColor(255, 255, 255));
    ColorItem *coloritem2_2 = new ColorItem(QColor(195, 195, 195));
    ColorItem *coloritem2_3 = new ColorItem(QColor(185, 122, 87));
    ColorItem *coloritem2_4 = new ColorItem(QColor(255, 174, 201));
    ColorItem *coloritem2_5 = new ColorItem(QColor(255, 201, 14));
    ColorItem *coloritem2_6 = new ColorItem(QColor(239, 228, 176));
    ColorItem *coloritem2_7 = new ColorItem(QColor(181, 230, 29));
    ColorItem *coloritem2_8 = new ColorItem(QColor(153, 217, 234));
    ColorItem *coloritem2_9 = new ColorItem(QColor(112, 146, 190));
    ColorItem *coloritem2_10 = new ColorItem(QColor(200, 191, 231));
    QHBoxLayout *hBoxLayout2 = new QHBoxLayout(); // 创建第二个水平布局

    // 添加第二组颜色项到布局
    hBoxLayout2->addWidget(coloritem2_1);
    hBoxLayout2->addWidget(coloritem2_2);
    hBoxLayout2->addWidget(coloritem2_3);
    hBoxLayout2->addWidget(coloritem2_4);
    hBoxLayout2->addWidget(coloritem2_5);
    hBoxLayout2->addWidget(coloritem2_6);
    hBoxLayout2->addWidget(coloritem2_7);
    hBoxLayout2->addWidget(coloritem2_8);
    hBoxLayout2->addWidget(coloritem2_9);
    hBoxLayout2->addWidget(coloritem2_10);
    hBoxLayout2->setMargin(2); // 设置边距
    hBoxLayout2->setSpacing(2); // 设置间距

    // 连接第二组颜色项的点击信号到处理函数
    connect(coloritem2_1,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem2_2,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem2_3,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem2_4,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem2_5,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem2_6,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem2_7,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem2_8,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem2_9,  SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));
    connect(coloritem2_10, SIGNAL(itemClicked(const QColor &)), this, SLOT(colorItemChanged(const QColor &)));

    // 创建主垂直布局，包含两个水平布局
    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setMargin(1); // 设置边距
    vBoxLayout->setSpacing(0); // 设置间距
    vBoxLayout->addLayout(hBoxLayout1); // 添加第一组颜色布局
    vBoxLayout->addLayout(hBoxLayout2); // 添加第二组颜色布局

    // 创建画刷按钮
    _btn_brush = new QPushButton();
    _btn_brush->setFixedSize(45, 45); // 设置固定大小
    _btn_brush->setIcon(QPixmap(":/image/brush.png")); // 设置图标
    _btn_brush->setIconSize(QSize(25, 25));
    _btn_brush->setToolTip(QStringLiteral("填充")); // 设置工具提示
    _btn_brush->setStyleSheet(s_brushStyle); // 设置样式

    // 创建当前颜色项
    _cur_coloritem = new ColorItem(QColor(0, 0, 0), 2 * ITEM_LENGTH + 2);
    PointSizeWidget *pointSizeWidget = new PointSizeWidget(); // 创建点大小选择控件
    QHBoxLayout *hBoxLayout = new QHBoxLayout(); // 创建水平布局
    hBoxLayout->addWidget(pointSizeWidget); // 添加点大小控件
    hBoxLayout->addWidget(_btn_brush); // 添加画刷按钮
    hBoxLayout->addWidget(_cur_coloritem); // 添加当前颜色项
    hBoxLayout->setMargin(1); // 设置边距
    hBoxLayout->setSpacing(2); // 设置间距

    // 连接信号和槽
    connect(_cur_coloritem, SIGNAL(clicked()), this, SLOT(colorSelection())); // 颜色项点击
    connect(pointSizeWidget, SIGNAL(wheelscrolled(int)), this, SLOT(pointSizeChanged(int))); // 点大小滚轮事件
    connect(_btn_brush, SIGNAL(clicked()), this, SLOT(btnBrushClicked())); // 画刷按钮点击

    // 创建主布局，包含颜色条和点大小选择控件
    QHBoxLayout *mainBoxLayout = new QHBoxLayout();
    mainBoxLayout->addLayout(hBoxLayout); // 添加点大小和颜色控件
    mainBoxLayout->addLayout(vBoxLayout); // 添加颜色布局
    mainBoxLayout->setMargin(2); // 设置边距
    mainBoxLayout->setSpacing(2); // 设置间距

    _colorbar->setStyleSheet("background-color: #F5F5F5;border-radius:10px;"); // 设置背景颜色
    _colorbar->setCursor(Qt::ArrowCursor); // 设置光标样式
    _colorbar->setLayout(mainBoxLayout); // 设置主布局
    _colorbar->setVisible(true); // 显示颜色条
    _colorbar->setVisible(false);
}



void ScreenView::showColorBar()
{
	Q_ASSERT(_colorbar);
	qreal x = _shortArea.topLeft().x();
	qreal y = _shortArea.bottomRight().y() + _toolbar->height() + 6;
	_colorbar->move(x, y);
	_colorbar->setVisible(true);
}

void ScreenView::hideColorBar()
{
	_colorbar->setVisible(false);
}

void ScreenView::initLabel()
{
	_label = new QLabel(this);
	_label->setStyleSheet("background-color: rgba(0, 0, 0, 50%);color: white;");
	_label->setAlignment(Qt::AlignHCenter);

	QFont font;
	font.setFamily(QStringLiteral("Microsoft YaHei"));
	_label->setFont(font);
}

void ScreenView::showLabel()
{
	Q_ASSERT(_label);
	qreal x = _shortArea.topLeft().x();
	qreal y = _shortArea.topLeft().y() - _label->height() - 2;
	qreal w = _shortArea.width();
	qreal h = _shortArea.height();
	QString tip = QStringLiteral("%1 × %2").arg(w).arg(h);
	_label->setText(tip);
	_label->adjustSize();
	_label->move(x, y);
	_label->setVisible(true);
}

void ScreenView::hideLabel()
{
	_label->setVisible(false);
}

void ScreenView::initToolBar()
{
	_toolbar = new QWidget(this);

	QHBoxLayout *mainToolBarLayout = new QHBoxLayout();
	_btn_copy = new QPushButton();
	_btn_save = new QPushButton();
	_btn_drawLine = new QPushButton();
	_btn_drawRect = new QPushButton();
	_btn_drawEllipse = new QPushButton();
	_btn_drawText = new QPushButton();
    _btn_uploadPicGo = new QPushButton();
    _btn_ocr = new QPushButton();
	
	_btn_copy->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
	_btn_save->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
	_btn_drawLine->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
	_btn_drawRect->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
	_btn_drawEllipse->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
	_btn_drawText->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    _btn_uploadPicGo->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    _btn_ocr->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);

    _btn_copy->setIcon(QPixmap(":/image/copy.png"));
    _btn_copy->setIconSize(QSize(32, 32));

    _btn_save->setIcon(QPixmap(":/image/save.png"));
    _btn_save->setIconSize(QSize(32, 32));

    _btn_drawLine->setIcon(QPixmap(":/image/drawline.png"));
    _btn_drawLine->setIconSize(QSize(32, 32));

    _btn_drawRect->setIcon(QPixmap(":/image/drawrect.png"));
    _btn_drawRect->setIconSize(QSize(32, 32));

    _btn_drawEllipse->setIcon(QPixmap(":/image/drawellipse.png"));
    _btn_drawEllipse->setIconSize(QSize(32, 32));

    _btn_drawText->setIcon(QPixmap(":/image/drawtext.png"));
    _btn_drawText->setIconSize(QSize(32, 32));

    _btn_uploadPicGo->setIcon(QPixmap(":/image/mail.png"));
    _btn_uploadPicGo->setIconSize(QSize(32, 32));

    _btn_ocr->setIcon(QPixmap(":/image/title.png"));
    _btn_ocr->setIconSize(QSize(32, 32));

	
    _btn_copy->setToolTip(QStringLiteral("复制到剪贴板(Enter)"));
    _btn_save->setToolTip(QStringLiteral("保存到文件(S)"));
    _btn_drawLine->setToolTip(QStringLiteral("绘制线段(L)"));
    _btn_drawRect->setToolTip(QStringLiteral("绘制矩形(P)"));
    _btn_drawEllipse->setToolTip(QStringLiteral("绘制椭圆(M)"));
    _btn_drawText->setToolTip(QStringLiteral("添加文本(T)"));
    _btn_uploadPicGo->setToolTip(QStringLiteral("上传到PicGo(U)"));
    _btn_ocr->setToolTip(QStringLiteral("提取文字(O)"));

    _toolbar->setStyleSheet(toolbar_Style);
	_btn_copy->setStyleSheet(s_normalStyle);
	_btn_save->setStyleSheet(s_normalStyle);
	_btn_drawLine->setStyleSheet(s_normalStyle);
	_btn_drawRect->setStyleSheet(s_normalStyle);
	_btn_drawEllipse->setStyleSheet(s_normalStyle);
	_btn_drawText->setStyleSheet(s_normalStyle);
    _btn_uploadPicGo->setStyleSheet(s_normalStyle);
    _btn_ocr->setStyleSheet(s_normalStyle);
	
	mainToolBarLayout->addWidget(_btn_drawLine);
	mainToolBarLayout->addWidget(_btn_drawRect);
	mainToolBarLayout->addWidget(_btn_drawEllipse);
	mainToolBarLayout->addWidget(_btn_drawText);
    mainToolBarLayout->addWidget(_btn_ocr);
	mainToolBarLayout->addWidget(_btn_save);
    mainToolBarLayout->addWidget(_btn_uploadPicGo);
	mainToolBarLayout->addWidget(_btn_copy);


	mainToolBarLayout->setContentsMargins(0, 0, 0, 0);
	mainToolBarLayout->setSpacing(0);
	
	connect(_btn_copy, SIGNAL(clicked()), this, SLOT(copyImage()));
	connect(_btn_save, SIGNAL(clicked()), this, SLOT(saveImage()));
	connect(_btn_drawLine, SIGNAL(clicked()), this, SLOT(drawLine()));
	connect(_btn_drawRect, SIGNAL(clicked()), this, SLOT(drawRect()));
	connect(_btn_drawEllipse, SIGNAL(clicked()), this, SLOT(drawEllipse()));
	connect(_btn_drawText, SIGNAL(clicked()), this, SLOT(drawTextStatus()));
    connect(_btn_uploadPicGo, SIGNAL(clicked()), this, SLOT(uploadPicGo()));
    connect(_btn_ocr, SIGNAL(clicked()), this, SLOT(ocrText()));

	_toolbar->setCursor(Qt::ArrowCursor);
	_toolbar->setLayout(mainToolBarLayout);
	_toolbar->setVisible(true);
	_toolbar->setVisible(false);
}

void ScreenView::showToolBar()
{
	Q_ASSERT(_toolbar);
	qreal x = _shortArea.topLeft().x();
	qreal y = _shortArea.bottomRight().y()+4;
	_toolbar->move(x, y);
	_toolbar->setVisible(true);

}

void ScreenView::hideToolBar()
{
	Q_ASSERT(_toolbar);
	_toolbar->setVisible(false);
}

void ScreenView::screenCapture(const QRectF &rect)
{
	QScreen *screen = QGuiApplication::primaryScreen();
	_shotPixmap = screen->grabWindow(0, rect.x(), rect.y(), rect.width(), rect.height());
}

void ScreenView::adjustShotScreen(QMouseEvent *event)
{
	QPointF pt_tl = _shortArea.topLeft();
	QPointF pt_br = _shortArea.bottomRight();
	qreal coord_x = event->x();
	qreal coord_y = event->y();
	qreal dx = coord_x - _ptDrag.x();
	qreal dy = coord_y - _ptDrag.y();

	CursorLocation curLocation = _curlocation;

	switch (curLocation)
	{
	case TOPLEFT:
		setCursor(Qt::SizeFDiagCursor);
		pt_tl.setX(coord_x);
		pt_tl.setY(coord_y);
		break;
	case TOP:
		setCursor(Qt::SizeVerCursor);
		pt_tl.setY(coord_y);
		break;
	case TOPRIGHT:
		setCursor(Qt::SizeBDiagCursor);
		pt_br.setX(coord_x);
		pt_tl.setY(coord_y);
		break;
	case LEFT:
		setCursor(Qt::SizeHorCursor);
		pt_tl.setX(coord_x);
		break;
	case MIDDLE:
		switch (_draw_edit_flag)
		{
		case DRAWLINE:
			_ptE.setX(coord_x);
			_ptE.setY(coord_y);
			_line_list.last().setP2(_ptE.toPoint());
			break;
		case DRAWRECT:
			_ptE.setX(coord_x);
			_ptE.setY(coord_y);
			_rect_list.last().setBottomRight(_ptE.toPoint());
			break;
		case DRAWCIRCLE:
			_ptE.setX(coord_x);
			_ptE.setY(coord_y);
			_ellipse_list.last().setBottomRight(_ptE.toPoint());
			break;
		case DRAWDRAG:
			setCursor(Qt::SizeAllCursor);
			_ptDrag.rx() = coord_x;
			_ptDrag.ry() = coord_y;
			if ((pt_tl.x() + dx > 0) && (pt_br.x() + dx < _screen_width))
			{
				pt_tl.rx() += dx;
				pt_br.rx() += dx;
			}
			if ((pt_tl.y() + dy > 0) && (pt_br.y() + dy < _screen_height))
			{
				pt_tl.ry() += dy;
				pt_br.ry() += dy;
			}
			break;
		default:
			break;
		}
		break;
	case RIGHT:
		setCursor(Qt::SizeHorCursor);
		pt_br.setX(coord_x);
		break;
	case BUTTOMLEFT:
		setCursor(Qt::SizeBDiagCursor);
		pt_tl.setX(coord_x);
		pt_br.setY(coord_y);
		break;
	case BUTTOM:
		setCursor(Qt::SizeVerCursor);
		pt_br.setY(coord_y);
		break;
	case BUTTOMRIGHT:
		setCursor(Qt::SizeFDiagCursor);
		pt_br.setX(coord_x);
		pt_br.setY(coord_y);
		break;
	default:
		break;
	}

	if (pt_tl.x() > pt_br.x() || pt_tl.y() > pt_br.y())
	{//如果矩形范围左上大于右下则进行调整
		if (pt_tl.x() > pt_br.x() && pt_tl.y() > pt_br.y())
		{
			switch (_curlocation)
			{
			case TOPLEFT:
				_curlocation = BUTTOMRIGHT;
				break;
			case TOPRIGHT:
				_curlocation = BUTTOMLEFT;
				break;
			case BUTTOMLEFT:
				_curlocation = TOPRIGHT;
				break;
			case BUTTOMRIGHT:
				_curlocation = TOPLEFT;
				break;
			default:
				break;
			}
		}
		else if (pt_tl.x() > pt_br.x())
		{
			switch (_curlocation)
			{
			case TOPLEFT:
				_curlocation = TOPRIGHT;
				break;
			case TOPRIGHT:
				_curlocation = TOPLEFT;
				break;
			case LEFT:
				_curlocation = RIGHT;
				break;
			case RIGHT:
				_curlocation = LEFT;
				break;
			case BUTTOMLEFT:
				_curlocation = BUTTOMRIGHT;
				break;
			case BUTTOMRIGHT:
				_curlocation = BUTTOMLEFT;
				break;
			default:
				break;
			}
		}
		else
		{
			switch (_curlocation)
			{
			case TOPLEFT:
				_curlocation = BUTTOMLEFT;
				break;
			case TOP:
				_curlocation = BUTTOM;
				break;
			case TOPRIGHT:
				_curlocation = BUTTOMRIGHT;
				break;
			case BUTTOMLEFT:
				_curlocation = TOPLEFT;
				break;
			case BUTTOM:
				_curlocation = TOP;
				break;
			case BUTTOMRIGHT:
				_curlocation = TOPRIGHT;
				break;
			default:
				break;
			}
		}
		setTopLeftAndButtomRight(pt_tl, pt_br);
	}

	_shortArea.setTopLeft(pt_tl);
	_shortArea.setBottomRight(pt_br);
}

void ScreenView::ocrText() {
    screenCapture(_shortArea);
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyyMMdd_hhmmss");
    QString imageName = ".temp_" + current_date + ".jpg";
    _shotPixmap.save(imageName);

    QDir currentDir = QDir::current();
    QString imagePath = currentDir.absoluteFilePath(imageName);

    ResultWindow *resultWindow = new ResultWindow(imagePath, this);
    resultWindow->show();

    this->hide();
    resultWindow->request();
    resultWindow->exec();
    delete resultWindow;
    this->close();

}

void ScreenView::drawText(QMouseEvent *event)
{
	int x = event->x();
	int y = event->y();
	TextPaint *curText = new TextPaint(this);
	int size = _text_list.length();
	for (int i = 0; i < size; i++)
	{
		_text_list[i]->clearFocus();
	}

	QPalette palette;
	palette.setColor(QPalette::Text, _pen_color);
	curText->setPalette(palette);
	curText->setStyleSheet("background:transparent;border-width:0;border-style:outset");

	curText->setGeometry(x, y, 30, 30);
	curText->setVisible(true);
	curText->setFocus();
	_text_list.append(curText);
	QWidget::mousePressEvent(event);
	update();
}

void ScreenView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() & Qt::LeftButton)
	{
		if (DrawStatus::WAITDRAW == _draw_flag)
		{
			_ptS.rx() = event->x();
			_ptS.ry() = event->y();
			_ptE.rx() = event->x();
			_ptE.ry() = event->y();
			_shortArea = getRectF(_ptS, _ptE);
			_draw_flag = DrawStatus::DRAWINIG;

            // 其他屏幕不截图了，放开
            // 获取鼠标位置
            QPoint mousePos = event->globalPos();
            QList<ScreenView*> unusedViewList;
            for (auto view : views) {
                // 检查鼠标位置是否在屏幕的矩形区域内
                if (!view->screen->geometry().contains(mousePos)) {
                    unusedViewList.append(view);
                }
            }

            for (auto view: unusedViewList) {
                view->close();
                views.removeAll(view);
            }
		}
		else if (DrawStatus::DRAWEND == _draw_flag)
		{
			_ptDrag.rx() = event->x();
			_ptDrag.ry() = event->y();

			QPointF pt_tl = _shortArea.topLeft();
			QPointF pt_br = _shortArea.bottomRight();
			
			qreal coord_x = event->x();
			qreal coord_y = event->y();
			qreal dx = coord_x - _ptDrag.x();
			qreal dy = coord_y - _ptDrag.y();

			CursorLocation curLocation = _curlocation;
			Qt::CursorShape curshape = cursor().shape();
			if (Qt::ForbiddenCursor == curshape)
				return;

			switch (curLocation)
			{
			case TOPLEFT:
				setCursor(Qt::SizeFDiagCursor);
				pt_tl.setX(coord_x);
				pt_tl.setY(coord_y);
				break;
			case TOP:
				setCursor(Qt::SizeVerCursor);
				pt_tl.setY(coord_y);
				break;
			case TOPRIGHT:
				setCursor(Qt::SizeBDiagCursor);
				pt_br.setX(coord_x);
				pt_tl.setY(coord_y);
				break;
			case LEFT:
				setCursor(Qt::SizeHorCursor);
				pt_tl.setX(coord_x);
				break;
			case MIDDLE:
				switch (_draw_edit_flag)
				{
				case DRAWLINE:
					_bIsDrawLineEnd = false;
					_ptS.rx() = event->x();
					_ptS.ry() = event->y();
					_ptE.rx() = event->x();
					_ptE.ry() = event->y();
					_line_list.append(LinePaint(QLine(_ptS.toPoint(), _ptE.toPoint()), QPen(_pen_color, _point_size, Qt::SolidLine)));
					break;
				case DRAWRECT:
					_ptS.rx() = event->x();
					_ptS.ry() = event->y();
					_ptE.rx() = event->x();
					_ptE.ry() = event->y();
					_rect_list.append(RectPaint(_ptS.toPoint(), _ptE.toPoint(), QPen(_pen_color, _point_size, Qt::SolidLine), _brush));
					break;
				case DRAWCIRCLE:
					_ptS.rx() = event->x();
					_ptS.ry() = event->y();
					_ptE.rx() = event->x();
					_ptE.ry() = event->y();
					_ellipse_list.append(EllipsePaint(_ptS.toPoint(), _ptE.toPoint(), QPen(_pen_color, _point_size, Qt::SolidLine), _brush));
					break;
				case DRAWTEXT:
					drawText(event);
					break;
				case DRAWDRAG:
					setCursor(Qt::SizeAllCursor);
					_ptDrag.rx() = coord_x;
					_ptDrag.ry() = coord_y;
					if ((pt_tl.x() + dx > 0) && (pt_br.x() + dx < _screen_width))
					{
						pt_tl.rx() += dx;
						pt_br.rx() += dx;
					}
					if ((pt_tl.y() + dy > 0) && (pt_br.y() + dy < _screen_height))
					{
						pt_tl.ry() += dy;
						pt_br.ry() += dy;
					}
					break;
				default:
					break;
				}
				break;
			case RIGHT:
				setCursor(Qt::SizeHorCursor);
				pt_br.setX(coord_x);
				break;
			case BUTTOMLEFT:
				setCursor(Qt::SizeBDiagCursor);
				pt_tl.setX(coord_x);
				pt_br.setY(coord_y);
				break;
			case BUTTOM:
				setCursor(Qt::SizeVerCursor);
				pt_br.setY(coord_y);
				break;
			case BUTTOMRIGHT:
				setCursor(Qt::SizeFDiagCursor);
				pt_br.setX(coord_x);
				pt_br.setY(coord_y);
				break;
			default:
				break;
			}

			_shortArea.setTopLeft(pt_tl);
			_shortArea.setBottomRight(pt_br);
		}
	}
	else if (event->button() & Qt::RightButton)
	{
		if (DrawEditFlag::DRAWLINE == _draw_edit_flag)
		{
			if (_line_list.size() != 0 && !_bIsDrawLineEnd)
			{
				_line_list.removeLast();
				_ptS.setX(-10);
				_ptS.setY(-10);
				_bIsDrawLineEnd = true;
			}
        } else {
            QPoint mousePos = event->globalPos();
            QList<ScreenView*> unusedViewList;
            for (auto view : views) {
                if (!view->screen->geometry().contains(mousePos)) {
                    unusedViewList.append(view);
                }
            }

            for (auto view: unusedViewList) {
                view->close();
                views.removeAll(view);
            }

            this->close();
        }
	}
	else if (event->button() &Qt::MiddleButton)
	{
		screenCapture(_shortArea);
		ImageView *imageview = new ImageView();
		imageview->setImage(_shotPixmap);
		imageview->setImageGeometry(_shortArea.toRect());
		imageview->show();
		close();
	}
}

void ScreenView::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		if (DrawStatus::DRAWINIG == _draw_flag)
		{
			_ptE.setX(event->x());
			_ptE.setY(event->y());
			_shortArea = getRectF(_ptS, _ptE);
		}
		else if (DrawStatus::DRAWEND == _draw_flag)
		{
			if (Qt::ForbiddenCursor == cursor().shape())
				return;
			adjustShotScreen(event);
		}

		update();
	}
	else
	{
		if (DrawStatus::DRAWEND == _draw_flag)
		{
			CursorLocation curLocation = caputerCursor(_shortArea, event->x(), event->y());
			_curlocation = curLocation;

			switch (curLocation)
			{
			case TOPLEFT:
				DrawEditFlag::DRAWDRAG == _draw_edit_flag ? setCursor(Qt::SizeFDiagCursor) : setCursor(Qt::ForbiddenCursor);
				break;
			case TOP:
				DrawEditFlag::DRAWDRAG == _draw_edit_flag ? setCursor(Qt::SizeVerCursor) : setCursor(Qt::ForbiddenCursor);
				break;
			case TOPRIGHT:
				DrawEditFlag::DRAWDRAG == _draw_edit_flag ? setCursor(Qt::SizeBDiagCursor) : setCursor(Qt::ForbiddenCursor);
				break;
			case LEFT:
				DrawEditFlag::DRAWDRAG == _draw_edit_flag ? setCursor(Qt::SizeHorCursor) : setCursor(Qt::ForbiddenCursor);
				break;
			case MIDDLE:
				DrawEditFlag::DRAWDRAG == _draw_edit_flag ? setCursor(Qt::SizeAllCursor) : setCursor(Qt::ArrowCursor);
				if (DrawEditFlag::DRAWLINE == _draw_edit_flag)
				{
					_ptE.setX(event->x());
					_ptE.setY(event->y());
					if (!_bIsDrawLineEnd && _ptS.x() > -1 && _ptS.y() > -1 && _line_list.size()>0)
						_line_list.last().setP2(_ptE.toPoint());
				}
				update();
				break;
			case RIGHT:
				DrawEditFlag::DRAWDRAG == _draw_edit_flag ? setCursor(Qt::SizeHorCursor) : setCursor(Qt::ForbiddenCursor);
				break;
			case BUTTOMLEFT:
				DrawEditFlag::DRAWDRAG == _draw_edit_flag ? setCursor(Qt::SizeBDiagCursor) : setCursor(Qt::ForbiddenCursor);
				break;
			case BUTTOM:
				DrawEditFlag::DRAWDRAG == _draw_edit_flag ? setCursor(Qt::SizeVerCursor) : setCursor(Qt::ForbiddenCursor);
				break;
			case BUTTOMRIGHT:
				DrawEditFlag::DRAWDRAG == _draw_edit_flag ? setCursor(Qt::SizeFDiagCursor) : setCursor(Qt::ForbiddenCursor);
				break;
			default:
				break;
			}
		}
	}
}

void ScreenView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() & Qt::LeftButton)
	{
		if (DrawStatus::DRAWINIG == _draw_flag)
		{
			_ptE.setX(event->x());
			_ptE.setY(event->y());
			_shortArea = getRectF(_ptS, _ptE);

			_draw_flag = DrawStatus::DRAWEND;
			showToolBar();
		}
		else if (DrawStatus::DRAWEND == _draw_flag)
		{
			switch (_draw_edit_flag)
			{
			case DRAWLINE:
// 				if (qAbs(_ptS.x() - _ptE.x()) < 1.0e-3 && qAbs(_ptS.y() - _ptE.y()) < 1.0e-3)
// 					_line_list.removeLast();
				break;
			case DRAWRECT:
				if (qAbs(_ptS.x() - _ptE.x()) < 1.0e-3 && qAbs(_ptS.y() - _ptE.y()) < 1.0e-3)
					_rect_list.removeLast();
				break;
			case DRAWCIRCLE:
				if (qAbs(_ptS.x() - _ptE.x()) < 1.0e-3 && qAbs(_ptS.y() - _ptE.y()) < 1.0e-3)
					_ellipse_list.removeLast();
				break;
			case DRAWDRAG:
				break;
			default:
				break;
			}
		}
		update();
	}
}

void ScreenView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        QPoint mousePos = QCursor::pos();
        QList<ScreenView*> unusedViewList;
        for (auto view : views) {
            if (!view->screen->geometry().contains(mousePos)) {
                unusedViewList.append(view);
            }
        }

        for (auto view: unusedViewList) {
            view->close();
            views.removeAll(view);
        }

        this->close();
    }
    else if (event->key() == Qt::Key_Return)
        this->copyImage();
    else if (event->key() == Qt::Key_S)
        this->saveImage();
    else if (event->key() == Qt::Key_U)
        this->uploadPicGo();
    else if (event->key() == Qt::Key_O)
        this->ocrText();
    else if (event->key() == Qt::Key_P)
        this->drawRect();
    else if (event->key() == Qt::Key_L)
        this->drawLine();
    else if (event->key() == Qt::Key_M)
        this->drawEllipse();
    else
        QWidget::keyPressEvent(event);
}

void ScreenView::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true); //设置渲染提示为消除锯齿

	QPixmap tempmask(_screen_width, _screen_width);
	tempmask.fill((QColor(0, 0, 0, 160)));
	painter.drawPixmap(0, 0, _fullPixmap);     //先绘制全屏原图背景
	painter.drawPixmap(0, 0, tempmask);        //然后绘制半透明背景，用来降低亮度

	switch (_draw_flag)
	{
	case WAITDRAW:
		break;
	case DRAWINIG:
	{
		painter.setPen(penScreenShotBound);
		painter.drawRect(_shortArea);
		painter.drawPixmap(_shortArea, _fullPixmap, _shortArea);
		showLabel();
		break;
	}
	case DRAWEND:
	{
		painter.setPen(penScreenShotBound);            //设置画笔形式
		painter.drawRect(_shortArea);                                //然后绘制矩形框
		painter.drawPixmap(_shortArea, _fullPixmap, _shortArea);     //然后将矩形框中的半透明图像替换成原图
		showToolBar();
		showLabel();
		break;
	}
	default:
		break;
	}

	int size = _line_list.length();
	for (int i = 0; i < size; i++)
	{
		painter.setPen(_line_list[i].getPen());
		painter.drawLine(_line_list[i]);
	}

	size = _rect_list.length();
	for (int i = 0; i < size; i++)
	{
		painter.setPen(_rect_list[i].getPen());
		painter.setBrush(_rect_list[i].getBrush());
		painter.drawRect(_rect_list[i]);
	}

	size = _ellipse_list.length();
	for (int i = 0; i < size; i++)
	{
		painter.setPen(_ellipse_list[i].getPen());
		painter.setBrush(_ellipse_list[i].getBrush());
		painter.drawEllipse(_ellipse_list[i]);
	}

	QWidget::paintEvent(event);
}
