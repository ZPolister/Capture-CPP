#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H
#include <QApplication>
#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QListWidget>
#include <QStringList>
#include <QPushButton>

class ResultWindow : public QDialog {
public:
    ResultWindow(const QString& text, QWidget *parent = nullptr);
};

#endif // RESULTWINDOW_H
