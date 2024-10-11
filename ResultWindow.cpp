#include "ResultWindow.h"

ResultWindow::ResultWindow(QWidget* parent) : QWidget(parent) {
        // 设置窗口标题
        setWindowTitle("识别结果");

        // 创建垂直布局
        layout = new QVBoxLayout(this);

        // 添加按钮以关闭窗口
        closeButton = new QPushButton("关闭", this);
        connect(closeButton, &QPushButton::clicked, this, &QWidget::close);

        // 将 QListWidget 和按钮添加到布局
        layout->addWidget(listWidget);
        layout->addWidget(closeButton);
}

void ResultWindow::setText(const QStringList& list) {

    // 将 QStringList 的内容添加到 QListWidget
    listWidget->clear();
    listWidget->addItems(list);

}
