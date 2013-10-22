#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    menuBar()->setVisible(true);
    ui->mainToolBar->setVisible(false);
    ui->statusBar->setVisible(false);

    //setWindowFlags(Qt::FramelessWindowHint);

    glWidget = new GlWidget();
    setCentralWidget(glWidget);

    resize(1280, 800);
}

MainWindow::~MainWindow()
{
    delete ui;
}
