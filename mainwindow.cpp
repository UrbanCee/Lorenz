#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->openGLWidget->resize(800,600);
    connect(ui->openGLWidget,SIGNAL(showStatusBarMessage(QString,int)),this,SLOT(showStatusBarMessage(QString,int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showStatusBarMessage(const QString &message, int iMessageTimeMS)
{
    this->statusBar()->showMessage(message,iMessageTimeMS);
}
