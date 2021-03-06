#ifndef HASHTHREAD_H
#define HASHTHREAD_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include "keypairqueue.h"

class HashThread : public QThread
{
    Q_OBJECT
public:
    void run();
    HashThread(KeyPairQueue* keys);
    ~HashThread();
public slots:
    void hash();
    void stop();
    void kill();
    void changeQueue(KeyPairQueue* keys);
signals:
    void hashComputed();
    void matchFound(QString hash, QString salt, QString key);
    void done();
private:
    /* Functions */
    void tryKey(std::wstring key, QByteArray salt, QString goal);
    QString doHash(QByteArray *keyAndSalt);
    bool sanityCheck();
    bool testHashFunction();
    QByteArray* combineKeyAndSalt(std::wstring key, QByteArray salt);
    bool testCombineKeyAndSalt();

    /* Member Objects */
    KeyPairQueue* keys;
    bool keepAlive;
    bool active;
};

#endif // HASHTHREAD_H
