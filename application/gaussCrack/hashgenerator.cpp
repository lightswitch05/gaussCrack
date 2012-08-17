#include "hashgenerator.h"

/**
  * Constructor
  */
HashGenerator::HashGenerator(KeyPairQueue *keys, int threadCount)
{
    this->hashsDone = 0;
    this->threadsActive = 0;
    this->threadCount = 0;
    this->keys = keys;
    this->hashActive = false;
    this->totalHashes = keys->getSize() * 4;
    this->threadPool = new QList<HashThread*>();
    while(this->threadCount < threadCount){
        this->spawnThread();
    }
}

/**
  * De-Constructor
  */
HashGenerator::~HashGenerator()
{
    for(int i=0; i < this->threadPool->size(); i++){
        this->threadPool->at(i)->stop();
        this->threadPool->at(i)->kill();
        delete this->threadPool->at(i);
    }
    delete this->keys;
}

void HashGenerator::spawnThread()
{
    HashThread* temp;
    qDebug() << "Spawning a thread";
    temp = new HashThread(keys);
    //Progress
    this->connect(temp,
                  SIGNAL(hashComputed()),
                  this,
                  SLOT(computeProgress()));
    //Target Found
    this->connect(temp,
                  SIGNAL(matchFound(QString,QString,QString)),
                  this,
                  SIGNAL(targetFound(QString,QString,QString)));
     //Thread Done
    this->connect(temp,
                  SIGNAL(done()),
                  this,
                  SLOT(threadDone()));
    this->threadCount++;
    this->threadPool->append( temp );
    temp->start();
    if(this->hashActive){
        this->threadsActive++;
        temp->hash();
    }
}

/**
  * Starts all the threads
  */
void HashGenerator::start()
{
    this->hashActive = true;
    for(int i=0; i < this->threadPool->size(); i++){
        qDebug() << "starting thread";
        this->threadsActive++;
        this->threadPool->at(i)->hash();
    }
}

/**
  * Stops all the threads
  */
void HashGenerator::stop()
{
    this->hashActive = false;
    for(int i=0; i < this->threadPool->size(); i++){
        qDebug() << "stopping thread";
        this->threadPool->at(i)->stop();
        this->threadsActive--;
    }
}

/**
  * Pauses all the threads
  */
void HashGenerator::pause()
{
    this->stop();
}

/**
  * Updates the queue. Used when re-starting.
  */
void HashGenerator::changeQueue(KeyPairQueue* keys)
{
    for(int i=0; i < this->threadPool->size(); i++){
        this->threadPool->at(i)->changeQueue(keys);
    }
    delete(this->keys);
    this->keys = keys;
    this->totalHashes = keys->getSize() * 4;
    this->hashsDone = 0;
}

/**
  * Keeps up with the amount the hashs to compute and
  * the number already computed.
  *
  * emits % progress for progress bar
  */
void HashGenerator::computeProgress(){
    this->hashsDone++;
    double percent = (double)this->hashsDone / (double)this->totalHashes;
    int result = percent * 100.00;
    qDebug() << "Hash Progress:" << result;
    emit this->updatePercent(result);
}

/**
  * Slot called by each thread when its queue runs out
  */
void HashGenerator::threadDone(){
    this->threadsActive--;
    qDebug() << "Now only have " << this->threadsActive << " active threads";
    if(this->threadsActive == 0){
        this->hashActive = false;
        emit this->done();
    }
}

/**
  * User has changed the number of threads to use
  */
void HashGenerator::setThreadNumber(int newAmount)
{
    //adding threads
    while(newAmount > this->threadCount){
        this->spawnThread();
    }

    //removing threads
    while(newAmount < this->threadCount && threadPool->size() > 0){
        qDebug() << "Dropping Threads to " << this->threadCount-1;
        this->threadPool->first()->stop();
        this->threadPool->first()->kill();
        this->threadPool->removeFirst();
        this->threadsActive--;
        this->threadCount--;
    }
}
