#include "hashthread.h"

#define DEBUG true
#ifdef DEBUG
    #include <QDebug>
#endif

/**
  * Constructor
  */
HashThread::HashThread(KeyPairQueue *keys)
{
    this->keepAlive = true;
    this->active = true;
    this->keys = keys;
}

/**
  * De-Constructor
  */
HashThread::~HashThread()
{
}

void HashThread::run()
{
    #ifdef DEBUG
        qWarning() << "Thread inside Main run loop";
    #endif
    QString key;
    forever{
        if(!this->keepAlive){
            #ifdef DEBUG
                qWarning() << "Thread Dying";
            #endif
            return;
        }
        if(this->active){
            key = this->keys->dequeue();
            key = "C:\\Documents and Settings\\john\\Local Settings\\Application Data\\Google\\Chrome\\Application~.dir1";
            //first sample
            this->tryKey(key.utf16(), QByteArray::fromHex("97486CAA225FE877C035CC0373236D51"),
                              QByteArray::fromHex("758EA09A147DCBCAD6BD558BE30774DE"));
            //second sample
            this->tryKey(key.toAscii(), QByteArray::fromHex("6EE3472C06A5C859BD1642D1D4F5BB3E"),
                              QByteArray::fromHex("EB2F172398261ED94C8D05216650919B"));
            //third sample
            this->tryKey(key.toAscii(), QByteArray::fromHex("0EA501D12471CDCD0E9EAC6E485AF932"),
                              QByteArray::fromHex("52DD4D6B792D84C422E6A08E4272ACB8"));
            //last sample
            this->tryKey(key.toAscii(), QByteArray::fromHex("C3234D515D52A58E8146FA8A6D93DF7D"),
                              QByteArray::fromHex("53B3FAEA53CC1B90AA2C5FCF831EF9E2"));
        }
        else{
            #ifdef DEBUG
                qWarning() << "Sleeping:";
            #endif
            this->sleep(1);
        }
        QCoreApplication::processEvents();
    }
}

/**
  * 4. For each pair, append the first hard-coded 16-byte salt...
  */
void HashThread::tryKey(QByteArray key, QByteArray salt, QByteArray goal)
{
    #ifdef DEBUG
        qWarning() << "Using Key:" << QString(key);
        qWarning() << "Using Salt:" << QString(salt);
    #endif
    QByteArray hash = this->doHash(key + salt);
    #ifdef DEBUG
        qWarning() << "Got Hash:" << QString(hash);
    #endif
    if(goal == hash){
        emit matchFound(QString(hash), QString(salt), QString(key));
    }
    emit hashComputed();
}

/**
  * 4. ...and calculate MD5 hash.
  * 5. Calculate MD5 hash from the hash ( i.e. hash = md5(hash) ), 10000 times.
  */
QByteArray HashThread::doHash(QByteArray keyAndSalt)
{
    QByteArray hash = QCryptographicHash::hash(keyAndSalt, QCryptographicHash::Md5);
    for(int i=0; i<1000; i++){
        hash = QCryptographicHash::hash(hash, QCryptographicHash::Md5);
    }
    return hash.toHex();
}

void HashThread::start()
{
    this->active = true;
}

void HashThread::stop()
{
    this->active = false;
}

void HashThread::kill()
{
    this->active = false;
    this->keepAlive = false;
}

void HashThread::changeQueue(KeyPairQueue* keys)
{
    this->keys = keys;
}
