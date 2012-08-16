#include "hashgenerator.h"

#define DEBUG true
#ifdef DEBUG
    #include <QDebug>
#endif

/**
  * Constructor
  */
HashGenerator::HashGenerator(KeyPairQueue *keys, int threadCount)
{
    this->hashsDone = 0;
    this->threadsActive = 0;
    this->threadCount = threadCount;
    this->keys = keys;
    this->totalHashes = keys->getSize() * 4;
    this->threadPool = new QList<HashThread*>();
    this->spawnThreads();
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

void HashGenerator::spawnThreads()
{
    HashThread* temp;
    while(this->threadPool->size() < this->threadCount){
        #ifdef DEBUG
            qWarning() << "Spawning a thread";
        #endif
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
        this->threadPool->append( temp );
        temp->start();
    }
    #ifdef DEBUG
        qWarning() << "Spawned " << this->threadPool->size() << " Threads";
    #endif
}

/**
  * Starts all the threads
  */
void HashGenerator::start()
{
    for(int i=0; i < this->threadPool->size(); i++){
        #ifdef DEBUG
            qWarning() << "starting thread";
        #endif
        this->threadsActive++;
        this->threadPool->at(i)->hash();
    }
}

/**
  * Stops all the threads
  */
void HashGenerator::stop()
{
    for(int i=0; i < this->threadPool->size(); i++){
        #ifdef DEBUG
            qWarning() << "stopping thread";
        #endif
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
    #ifdef DEBUG
        qWarning() << "Hash Progress:" << result;
    #endif
    emit this->updatePercent(result);
}

/**
  * Slot called by each thread when its queue runs out
  */
void HashGenerator::threadDone(){
    this->threadsActive--;
    #ifdef DEBUG
        qWarning() << "Now only have " << this->threadsActive << " active threads";
    #endif
    if(this->threadsActive == 0){
        emit this->done();
    }
}
