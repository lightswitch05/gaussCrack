#ifndef KEYPAIRQUEUE_H
#define KEYPAIRQUEUE_H

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QStringList>
#include <Windows.h>

class KeyPairQueue
{
public:
    KeyPairQueue();
    QString dequeue();
    int getSize();
    bool isEmpty();
    ~KeyPairQueue();
private:
    /* Functions */
    void enqueue(QString);
    QStringList getPath();
    QStringList appendFiles(QStringList paths);
    void buildQueue(QStringList pathAndFiles);
    QString getProgramFilesPath();

    /* Member Objects */
    QQueue<QString>* queue;
    QMutex* lock;
};

#endif // KEYPAIRQUEUE_H
