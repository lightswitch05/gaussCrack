#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <keypairqueue.h>
#include <hashgenerator.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onStartButtonClick();
    void onStopButtonClick();
    void targetFound(QString hash, QString salt, QString key);
    void hashesDone();

private:
    /* Functions */
    void setupSignals();

    /* Member Objects */
    Ui::MainWindow *ui;
    HashGenerator* hash;
};

#endif // MAINWINDOW_H
