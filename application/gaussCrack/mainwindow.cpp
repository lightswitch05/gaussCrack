#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
  * Constructor for main window
  */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->hash = new HashGenerator(new KeyPairQueue());
    this->ui->setupUi(this);
    this->setupSignals();
}

/**
  * De-Constructor for main window
  */
MainWindow::~MainWindow()
{
    delete this->hash;
    delete this->ui;
}

void MainWindow::setupSignals()
{
    //Start Button
    this->connect(this->ui->startButton,
                  SIGNAL(clicked()),
                  this,
                  SLOT(onStartButtonClick()));
    //Stop Button
    this->connect(this->ui->stopButton,
                  SIGNAL(clicked()),
                  this,
                  SLOT(onStopButtonClick()));
    //Target Found
    this->connect(this->hash,
                  SIGNAL(targetFound(QString,QString,QString)),
                  this,
                  SLOT(targetFound(QString,QString,QString)));
    //Update Progress
    this->connect(this->hash,
                  SIGNAL(updatePercent(int)),
                  this->ui->progressBar,
                  SLOT(setValue(int)));
    //Update Progress
    this->connect(this->hash,
                  SIGNAL(done()),
                  this,
                  SLOT(hashesDone()));
}

/**
  * Slot for start/pause button
  */
void MainWindow::onStartButtonClick()
{
    if(this->ui->startButton->text() == tr("Start") || this->ui->startButton->text() == tr("Resume")) {
        this->ui->startButton->setText(tr("Pause"));
        this->ui->statusLabel->setText(tr("Running"));
        this->hash->start();
    }
    else {
        this->ui->startButton->setText(tr("Resume"));
        this->ui->statusLabel->setText(tr("Idle"));
        this->hash->stop();
    }
}

/**
  * Slot for stop button
  */
void MainWindow::onStopButtonClick()
{
    this->hash->stop();
    this->hash->changeQueue(new KeyPairQueue());
    this->ui->startButton->setText(tr("Start"));
    this->ui->statusLabel->setText(tr("Idle"));
    this->ui->progressBar->setValue(0);
}

void MainWindow::targetFound(QString hash, QString salt, QString key)
{

}

void MainWindow::hashesDone()
{
    this->ui->startButton->setText(tr("Start"));
    this->ui->statusLabel->setText(tr("Idle"));
    this->hash->changeQueue(new KeyPairQueue());
}
