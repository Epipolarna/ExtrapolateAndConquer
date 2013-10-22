#include "glMainWindow.h"
#include "ui_glMainWindow.h"

glMainWindow::glMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::glMainWindow)
{
    ui->setupUi(this);

    menuBar()->setVisible(true);
    //ui->mainToolBar->setVisible(false);
    //ui->statusBar->setVisible(false);

    //setWindowFlags(Qt::FramelessWindowHint);

    glWidget = new GLWidget();
    setCentralWidget(glWidget);

    resize(1280, 800);
}

glMainWindow::~glMainWindow()
{
    delete ui;
}
