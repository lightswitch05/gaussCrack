#ifndef HASHGENERATOR_H
#define HASHGENERATOR_H

#include <QObject>
#include <QString>
#include <QList>
#include "keypairqueue.h"
#include "hashthread.h"

class HashGenerator : public QObject
{
    Q_OBJECT
public:
    HashGenerator(KeyPairQueue* keys, int threadCount=10);
    void run();
    ~HashGenerator();
public slots:
    void start();
    void pause();
    void stop();
    void changeQueue(KeyPairQueue* keys);
    void computeProgress();
signals:
    void updatePercent(int);
    void targetFound(QString hash, QString salt, QString key);
private:
    /* Functions */
    void spawnThreads();

    /* Member Objects */
    int threadCount;
    int totalHashes;
    int hashsDone;
    KeyPairQueue* keys;
    QList<HashThread*>* threadPool;
};

#endif // HASHGENERATOR_H
