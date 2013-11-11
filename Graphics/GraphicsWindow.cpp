#include "GraphicsWindow.hpp"
#include "ui_GraphicsWindow.h"
#include <QDebug>


void GraphicsWindow::registerEventManager(graphics::Camera *c){
    cam = c;
}

GraphicsWindow::GraphicsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphicsWindow)
{
    ui->setupUi(this);

    ui->menubar->setVisible(true);
    ui->statusbar->setVisible(false);

    setWindowFlags(Qt::FramelessWindowHint);

    QGLFormat f;
    f.setVersion(3,2);
    f.setProfile(QGLFormat::CoreProfile);

    glWidget = new GraphicsWidget(f);
    setCentralWidget(glWidget);
    registerEventManager(getRenderer()->cam);
    resize(1280, 800);
}

void GraphicsWindow::repaint(void){
    glWidget->repaint();
}

Renderer* GraphicsWindow::getRenderer(void){
    return glWidget->getRenderer();
}

GraphicsWindow::~GraphicsWindow(){
    delete ui;
}

void GraphicsWindow::closeEvent(QCloseEvent *e){
    e->accept();
}

void GraphicsWindow::keyReleaseEvent(QKeyEvent *e){

    if(cam == NULL){
        return ;
    }

    qDebug() << "GLWidget KeyRelease: " << e->text();
    switch(e->key()){
    default:
        cam->keyReleaseEvent(e);
        break;
    }
}

void GraphicsWindow::keyPressEvent(QKeyEvent *e)
{

    if(cam == NULL){
        return ;
    }

    qDebug() << "GLWidget KeyPress: " << e->text();
    switch(e->key()){

    case Qt::Key_Escape:
        close();
        break;
    default:
        cam->keyPressEvent(e);
        break;
    }
}
