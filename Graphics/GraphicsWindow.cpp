#include "GraphicsWindow.hpp"
#include "ui_GraphicsWindow.h"
#include <QDebug>

GraphicsWindow::GraphicsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphicsWindow)
{
    ui->setupUi(this);

    ui->menubar->setVisible(false);
    ui->statusbar->setVisible(false);

    setWindowFlags(Qt::FramelessWindowHint);

    QGLFormat f;
    f.setVersion(3,2);
    f.setProfile(QGLFormat::CoreProfile);
    //f.setProfile(QGLFormat::CompatibilityProfile);
    //f.setProfile(QGLFormat::NoProfile);


    glWidget = new GLWidget(f);
    setCentralWidget(glWidget);

    resize(1280, 800);
}

GraphicsWindow::~GraphicsWindow()
{
    delete ui;
}

void GraphicsWindow::keyPressEvent(QKeyEvent *e)
{
    qDebug() << "GraphicsWindow KeyPress: " << e->text();
    switch(e->key()){
    case Qt::Key_Escape:
        close();
        break;
    default:
        glWidget->keyPressEvent(e);
        break;
    }
}

void GraphicsWindow::closeEvent(QCloseEvent *e)
{
    e->accept();
}
