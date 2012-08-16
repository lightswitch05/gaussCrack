#include "hashthread.h"


/**
  * Constructor
  */
HashThread::HashThread(KeyPairQueue *keys)
{
    this->keepAlive = true;
    this->active = false;
    this->keys = keys;
    if(!this->sanityCheck()){
        qFatal("SANITY CHECK FAILED!! Something is wrong with the hashing");
    }
}

/**
  * De-Constructor
  */
HashThread::~HashThread()
{
    this->keepAlive = false;
}

void HashThread::run()
{
    qDebug() << "Thread inside Main run loop";
    std::wstring key;
    forever{
        if(!this->keepAlive){
            qDebug() << "Thread Dying";
            return;
        }
        if(this->active){
            key = this->keys->dequeue();
            if(!key.empty()){
                key = L"C:\\Documents and Settings\\john\\Local Settings\\Application Data\\Google\\Chrome\\Application~dir1";
                //first sample
                this->tryKey(key, QByteArray::fromHex("97486CAA225FE877C035CC0373236D51"),
                                  QByteArray::fromHex("758EA09A147DCBCAD6BD558BE30774DE"));
                /*//second sample
                this->tryKey(key, QByteArray::fromHex("6EE3472C06A5C859BD1642D1D4F5BB3E"),
                                  QByteArray::fromHex("EB2F172398261ED94C8D05216650919B"));
                //third sample
                this->tryKey(key, QByteArray::fromHex("0EA501D12471CDCD0E9EAC6E485AF932"),
                                  QByteArray::fromHex("52DD4D6B792D84C422E6A08E4272ACB8"));
                //last sample
                this->tryKey(key, QByteArray::fromHex("C3234D515D52A58E8146FA8A6D93DF7D"),
                                  QByteArray::fromHex("53B3FAEA53CC1B90AA2C5FCF831EF9E2"));*/
            }
            else{
                this->active = false;
                emit this->done();
            }
        }
        else{
            qDebug() << "Sleeping";
            this->sleep(1);
        }
        QCoreApplication::processEvents();
    }
}

/**
  * 4. For each pair, append the first hard-coded 16-byte salt...
  */
void HashThread::tryKey(std::wstring key, QByteArray salt, QByteArray goal)
{
    QByteArray* keyAndSalt = new QByteArray((char*)key.c_str());
    //keyAndSalt->setRawData((char*)key.c_str(),key.length());
    keyAndSalt->append(salt);
    QByteArray hash = this->doHash(keyAndSalt);
    #ifdef DEBUG
        qWarning() << "Got Hash:" << QString(hash);
    #endif
    if(goal == hash){
        emit matchFound(QString(hash), QString(salt), QString::fromStdWString(key));
    }
    emit hashComputed();
}

/**
  * 4. ...and calculate MD5 hash.
  * 5. Calculate MD5 hash from the hash ( i.e. hash = md5(hash) ), 10000 times.
  */
QByteArray HashThread::doHash(QByteArray* keyAndSalt)
{
    QByteArray hash = QCryptographicHash::hash(*keyAndSalt, QCryptographicHash::Md5);
    for(int i=0; i<10000; i++){
        hash = QCryptographicHash::hash(hash, QCryptographicHash::Md5);
    }
    return hash.toHex();
}

void HashThread::hash()
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

bool HashThread::sanityCheck()
{
    QByteArray testData = QByteArray::fromHex("43003A005C0044006F00630075006D0065006E0074007300200061006E0064002000530065007400740069006E00670073005C006A006F0068006E005C004C006F00630061006C002000530065007400740069006E00670073005C004100700070006C00690063006100740069006F006E00200044006100740061005C0047006F006F0067006C0065005C004300680072006F006D0065005C004100700070006C00690063006100740069006F006E007E006400690072003100BB494E77F925EEC03B89FCEDC2224A211500");
    QByteArray result = HashThread::doHash(&testData);
    if(QString(result) == "00916031b3e9513044436ee42b6aa273") {
        return true;
    }
    return false;
}
