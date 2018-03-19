#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->openGLWidget->resize(800,600);
    connect(ui->openGLWidget,SIGNAL(showStatusBarMessage(QString,int)),this,SLOT(showStatusBarMessage(QString,int)));
    connect(ui->lineEditDT,SIGNAL(editingFinished()),this,SLOT(updateLorenzLineData()));
    connect(ui->sliderIterations,SIGNAL(valueChanged(int)),this,SLOT(updateLorenzLineData()));
    connect(ui->chkHSVLines,SIGNAL(toggled(bool)),this,SLOT(updateLorenzLineProperties()));
    connect(ui->chkRGBLines,SIGNAL(toggled(bool)),this,SLOT(updateLorenzLineProperties()));
    ui->sliderIterations->setValue(CLorenzLine::iIterationSteps/100);
    ui->labelIterations->setNum(ui->sliderIterations->value()*100);
    ui->lineEditDT->setText(QString::number(CLorenzLine::dIterationDT));
    ui->chkHSVLines->setChecked(CLorenzLine::bShowHSVLine);
    ui->chkRGBLines->setChecked(CLorenzLine::bShowRGBLines);
    ui->lineEditDT->setValidator(new QDoubleValidator(0.0,1.0,5));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showStatusBarMessage(const QString &message, int iMessageTimeMS)
{
    this->statusBar()->showMessage(message,iMessageTimeMS);
}

void MainWindow::updateLorenzLineData()
{
    ui->labelIterations->setNum(ui->sliderIterations->value()*100);
    CLorenzLine::iIterationSteps=ui->sliderIterations->value()*100;
    bool bOk=false;
    double newDT=ui->lineEditDT->text().toDouble(&bOk);
    if (bOk)
        CLorenzLine::dIterationDT=newDT;
    ui->openGLWidget->updateLorenzLine();

}

void MainWindow::updateLorenzLineProperties()
{
    CLorenzLine::bShowHSVLine=ui->chkHSVLines->isChecked();
    CLorenzLine::bShowRGBLines=ui->chkRGBLines->isChecked();
}


