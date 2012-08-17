#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
  * Constructor for main window
  */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);
    this->hash = new HashGenerator(new KeyPairQueue(),this->ui->threadSpinBox->value());
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

/**
  * Link all the slots and signals between the UI and the HashGenerator
  */
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
    //Change Thread Count
    this->connect(this->ui->threadSpinBox,
                  SIGNAL(valueChanged(int)),
                  this->hash,
                  SLOT(setThreadNumber(int)));
}

/**
  * Slot for UI start/pause button
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
  * Slot for UI stop button
  */
void MainWindow::onStopButtonClick()
{
    this->hash->stop();
    this->hash->changeQueue(new KeyPairQueue());
    this->ui->startButton->setText(tr("Start"));
    this->ui->statusLabel->setText(tr("Idle"));
    this->ui->progressBar->setValue(0);
}

/**
  * Slot called when HashGenerator reports a matching hash
  */
void MainWindow::targetFound(QString hash, QString salt, QString key)
{
    qWarning() << "MATCH HAS BEEN FOUND! hash:" <<  hash << " salt:" << salt << " key:" << key;
    //Target found! Pause search
    this->ui->startButton->setText(tr("Resume"));
    this->ui->statusLabel->setText(tr("Idle"));
    this->hash->stop();

    //update UI
    QPalette plt;
    plt.setColor(QPalette::WindowText, Qt::darkRed);
    this->ui->targetFoundLabel->setPalette(plt);
    this->ui->targetFoundLabel->setText(tr("You Are The Target!") + "\nKeypair:"+key+ "\nSalt:"+salt+"\nHash:" +hash);
}

/**
  * Slot called when HashGenerator has run all the hashs
  */
void MainWindow::hashesDone()
{
    //update UI
    QPalette plt;
    plt.setColor(QPalette::WindowText, Qt::darkGreen);
    this->ui->targetFoundLabel->setPalette(plt);
    this->ui->targetFoundLabel->setText(tr("System Passed. You are NOT the target."));
    this->ui->startButton->setText(tr("Start"));
    this->ui->statusLabel->setText(tr("Idle"));

    this->hash->changeQueue(new KeyPairQueue());
}
