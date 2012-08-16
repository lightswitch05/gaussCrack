#ifndef KEYPAIRQUEUE_H
#define KEYPAIRQUEUE_H

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QList>
#include <Windows.h>
#include <QDebug>

class KeyPairQueue
{
public:
    KeyPairQueue();
    std::wstring dequeue();
    int getSize();
    bool isEmpty();
    ~KeyPairQueue();
private:
    /* Functions */
    void enqueue(std::wstring);
    std::wstring getPath();
    QList<std::wstring>* appendFiles(QList<std::wstring>* paths);
    void buildQueue(QList<std::wstring>* pathAndFiles);
    QString getProgramFilesPath();
    QList<std::wstring> *splitPath(std::wstring path);

    /* Member Objects */
    QQueue<std::wstring>* queue;
    QMutex* lock;
};

#endif // KEYPAIRQUEUE_H
