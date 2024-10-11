#include "ResultWindow.h"
#include <QTextEdit>

ResultWindow::ResultWindow(const QString& text, QWidget* parent) : QDialog(parent) {
        // 设置窗口标题
        setWindowTitle("识别结果");

        this->setFixedSize(800, 600);

        QTextEdit *textEdit = new QTextEdit(this);
        textEdit->setText(text);
        textEdit->setReadOnly(true);

        QPushButton *okButton = new QPushButton("完成", this);
        connect(okButton, &QPushButton::clicked, this, &ResultWindow::accept);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(textEdit);
        layout->addWidget(okButton);

        setLayout(layout);
}
