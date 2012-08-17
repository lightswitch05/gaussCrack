#include "hashthread.h"


/**
  * Constructor
  */
HashThread::HashThread(KeyPairQueue *keys)
{
    this->keepAlive = true;
    this->active = false;
    this->keys = keys;
    //Run basic tests to make sure hashs and conversion are working correctly
    //crash if everything doesn't check out
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

/**
  * Main thread execution
  */
void HashThread::run()
{
    std::wstring key;
    forever{
        if(!this->keepAlive){
            qDebug() << "Thread Dying";
            return;
        }
        if(this->active){
            key = this->keys->dequeue();
            if(!key.empty()){
                //first sample
                this->tryKey(key, QByteArray::fromHex("97486CAA225FE877C035CC0373236D51"),
                                  QString("758EA09A147DCBCAD6BD558BE30774DE"));
                //second sample
                this->tryKey(key, QByteArray::fromHex("6EE3472C06A5C859BD1642D1D4F5BB3E"),
                                  QString("EB2F172398261ED94C8D05216650919B"));
                //third sample
                this->tryKey(key, QByteArray::fromHex("0EA501D12471CDCD0E9EAC6E485AF932"),
                                  QString("52DD4D6B792D84C422E6A08E4272ACB8"));
                //last sample
                this->tryKey(key, QByteArray::fromHex("C3234D515D52A58E8146FA8A6D93DF7D"),
                                  QString("53B3FAEA53CC1B90AA2C5FCF831EF9E2"));
            }
            else{
                this->active = false;
                emit this->done();
            }
        }
        else{
            qDebug() << "Thread Sleeping";
            this->sleep(2);
        }
        QCoreApplication::processEvents(); // handle slots, signals, and anything that needs to be done
    }
}

/**
  * 4. For each pair, append the first hard-coded 16-byte salt...
  */
void HashThread::tryKey(std::wstring key, QByteArray salt, QString goal)
{
    //combine key and salt
    QByteArray* keyAndSalt = this->combineKeyAndSalt(key, salt);
    QString hash = this->doHash(keyAndSalt);
    qDebug() << "Got Hash:" << QString(hash);
    //test if goal was found
    if(QString::compare(hash, goal, Qt::CaseInsensitive) == 0){
        emit matchFound(hash, QString(salt.toHex()), QString::fromStdWString(key));
    }
    emit hashComputed();
}

/**
  * 4. ...and calculate MD5 hash.
  * 5. Calculate MD5 hash from the hash ( i.e. hash = md5(hash) ), 10000 times.
  */
QString HashThread::doHash(QByteArray* keyAndSalt)
{
    QByteArray hash = QCryptographicHash::hash(*keyAndSalt, QCryptographicHash::Md5);
    for(int i=0; i<10000; i++){
        hash = QCryptographicHash::hash(hash, QCryptographicHash::Md5);
    }
    return QString(hash.toHex());
}

/**
  * 4. For each pair, append the first hard-coded 16-byte salt and calculate MD5 hash.
  *
  * Combines the key and salt into one QByteArray
  */
QByteArray* HashThread::combineKeyAndSalt(std::wstring key, QByteArray salt)
{  
    //Convert 16 bit wchar_t[] to char[] - doubles length
    char tempArray[key.length()*2];
    const wchar_t* keyArray = key.data();
    for(uint i=0; i<key.length(); i++){
        tempArray[i*2] = keyArray[i] & 0xff;
        tempArray[i*2+1]=(keyArray[i] >> 8);
    }
    //Convert to QByteArray and append salt
    QByteArray* result = new QByteArray(tempArray, key.length()*2);
    result->append(salt);
    return result;
}

/**
  * Begins processing of hashs.
  */
void HashThread::hash()
{
    this->active = true;
}

/**
  * Stops processing of hashs, thread stays runnning.
  */
void HashThread::stop()
{
    this->active = false;
}

/**
  * This will end the thread.
  */
void HashThread::kill()
{
    this->active = false;
    this->keepAlive = false;
}

/**
  * Called when the user re-starts the scan. Sets the new keypair queue.
  */
void HashThread::changeQueue(KeyPairQueue* keys)
{
    this->keys = keys;
}

/**
  * Runs basic tests to make sure thread and hashes are working correctly
  */
bool HashThread::sanityCheck()
{
    if(!this->testHashFunction()){
        return false;
    }
    if(!this->testCombineKeyAndSalt()){
        return false;
    }
    return true;
}

/**
  * This should be called at the creation of a hash thread. It will create a byte array from a wstring and
  * a salt and does the hash. This is to catch any descrepancies between w_chars across different computers.
  */
bool HashThread::testCombineKeyAndSalt()
{
    std::wstring key = L"C:\\Documents and Settings\\john\\Local Settings\\Application Data\\Google\\Chrome\\Application~dir1";
    QByteArray salt = QByteArray::fromHex("97486CAA225FE877C035CC0373236D51");
    QByteArray* keyAndSalt = combineKeyAndSalt(key, salt);
    QString result = HashThread::doHash(keyAndSalt);
    qDebug() << "testCombineKeyAndSalt got hash:" << result;
    if(QString::compare(result, "76405ce7f4e75e352c1cd4d9aeb6be41", Qt::CaseInsensitive) == 0){
        qDebug("testCombineKeyAndSalt Test Passed");
        return true;
    }
    qCritical("testCombineKeyAndSalt Test Failed");
    return false;
}

/**
  * This should be called at the creation of a hash thread. It forms a byte array of test data
  * and runs it through the md5 hash function. If the result is wrong, it will end the program. Doing this
  * should catch any discrepancies between MD5 functions.
  */
bool HashThread::testHashFunction()
{
    QByteArray testData = QByteArray::fromHex("43003A005C0044006F00630075006D0065006E0074007300200061006E0064002000530065007400740069006E00670073005C006A006F0068006E005C004C006F00630061006C002000530065007400740069006E00670073005C004100700070006C00690063006100740069006F006E00200044006100740061005C0047006F006F0067006C0065005C004300680072006F006D0065005C004100700070006C00690063006100740069006F006E007E00640069007200310097486CAA225FE877C035CC0373236D51");
    QString result = HashThread::doHash(&testData);
    qDebug() << "testCombineKeyAndSalt got hash:" << result;
    if(QString::compare(result, "76405ce7f4e75e352c1cd4d9aeb6be41", Qt::CaseInsensitive) == 0){
        qDebug("testHashFunction Test Passed");
        return true;
    }
    qCritical("testHashFunction Test Failed");
    return false;
}
