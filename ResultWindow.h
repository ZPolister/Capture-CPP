#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QStringList>
#include <QPushButton>

class ResultWindow : public QWidget {
public:
    ResultWindow(QWidget*);
    void setText(const QStringList&);
    QVBoxLayout* layout;
    QListWidget* listWidget;
    QPushButton* closeButton;
};

#endif // RESULTWINDOW_H
