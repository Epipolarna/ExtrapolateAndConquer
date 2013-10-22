#ifndef GLMAINWINDOW_H
#define GLMAINWINDOW_H

#include <QMainWindow>
#include "glwidget.h"

namespace Ui {
class glMainWindow;
}

class glMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit glMainWindow(QWidget *parent = 0);
    ~glMainWindow();

private:
    Ui::glMainWindow *ui;
    GLWidget* glWidget;
};

#endif // GLMAINWINDOW_H
