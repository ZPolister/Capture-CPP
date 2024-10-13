#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H
#include <QApplication>
#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QListWidget>
#include <QStringList>
#include <QPushButton>
#include <qtextEdit>
#include <QLabel>

class ResultWindow : public QDialog {
public:
    ResultWindow(const QString& imagePath, QWidget *parent = nullptr);
    ~ResultWindow();
    void request();
    void copyText();
private:
    QTextEdit *textEdit;
    QLabel *imageLabel;
    QString imagePath;
};

#endif // RESULTWINDOW_H
