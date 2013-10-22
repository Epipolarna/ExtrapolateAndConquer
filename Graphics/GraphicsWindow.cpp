#include "GraphicsWindow.hpp"
#include "ui_GraphicsWindow.h"

GraphicsWindow::GraphicsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphicsWindow)
{
    ui->setupUi(this);

    ui->menubar->setVisible(false);
    ui->statusbar->setVisible(false);

    //setWindowFlags(Qt::FramelessWindowHint);

    glWidget = new GLWidget();
    setCentralWidget(glWidget);

    resize(1280, 800);
}

GraphicsWindow::~GraphicsWindow()
{
    delete ui;
}
