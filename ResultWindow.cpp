#include "ResultWindow.h"
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkReply>
#include <QFile>
#include <QLabel>
#include <QPixmap>
#include <QHBoxLayout>
#include <QClipboard>

ResultWindow::ResultWindow(const QString& imagePath, QWidget* parent) : QDialog(parent) {


    // 设置窗口标题
    setWindowTitle(tr("识别结果"));
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setWindowFlag(Qt::WindowMinimizeButtonHint, true);

    this->imagePath = imagePath;

    // 创建图片显示区域
    imageLabel = new QLabel(this);
    // imageLabel->setScaledContents(true); // 使图片适应标签大小

    // 创建文本显示区域
    textEdit = new QTextEdit(this);
    QFont font("微软雅黑", 11);
    this->setFont(font);
    textEdit->setText(tr("正在识别..."));
    textEdit->setReadOnly(true);
    textEdit->setFixedWidth(350);
    textEdit->setFixedHeight(700);


    // 创建完成按钮
    QPushButton *okButton = new QPushButton(tr("完成"), this);
    connect(okButton, &QPushButton::clicked, this, &ResultWindow::accept);
    // 创建复制按钮
    QPushButton *copyButton = new QPushButton(tr("复制"), this);
    connect(copyButton, &QPushButton::clicked, this, &ResultWindow::copyText);


    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(textEdit);
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(copyButton);
    buttonLayout->addWidget(okButton);

    rightLayout->addLayout(buttonLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(imageLabel);   // 左侧添加图片显示区域
    mainLayout->addLayout(rightLayout);  // 右侧添加文本编辑和按钮

    setLayout(mainLayout);
}

ResultWindow::~ResultWindow(){
    if (QFile::exists(imagePath)) {
        // 删除临时文件
        if (QFile::remove(imagePath)) {
            qDebug() << "删除文件成功:" << imagePath;
        } else {
            qDebug() << "删除失败:" << imagePath;
        }
    }
}


void ResultWindow::request() {
    // 显示图片
    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        imageLabel->setPixmap(pixmap);

        // 根据图片大小调整窗口大小, 感觉怪怪的
        imageLabel->adjustSize();
        this->adjustSize();
    } else {
        qDebug() << "无法加载图片:" << imagePath;
        imageLabel->setText(tr("无法加载图片"));
        this->adjustSize();
    }

    QNetworkAccessManager netManager;

    QString url = "http://localhost:36680/check";
    QNetworkRequest request = QNetworkRequest(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["path"] = imagePath;
    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson();

    QByteArray responseData;
    QEventLoop eventLoop;
    connect(&netManager, SIGNAL(finished(QNetworkReply * )), &eventLoop, SLOT(quit()));

    QNetworkReply *initReply = netManager.post(request, jsonData);
    eventLoop.exec();
    responseData = initReply->readAll();

    // 解析json
    QJsonParseError json_error;
    QJsonDocument doucment = QJsonDocument::fromJson(responseData, &json_error);
    if (json_error.error == QJsonParseError::NoError) {
        if (doucment.isObject()) {
            const QJsonObject object_data = doucment.object();

            qDebug() << object_data;
            if (object_data.contains("success") && object_data.value("success").toBool()) {
                QStringList stringList;
                QJsonArray jsonArray = object_data.value("result").toArray();
                for (const QJsonValue &value: jsonArray) {
                    if (value.isString()) {
                        stringList.append(value.toString());
                    }
                }
                qDebug() << "转换完成";

                this->textEdit->setText(stringList.join('\n'));
            }
        }
    } else {
        this->textEdit->setText(tr("识别失败，请检查本地OCR服务是否正常"));
    }
}

void ResultWindow::copyText() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(textEdit->toPlainText());
}
