#include "keypairqueue.h"

#define DEBUG true
#include <QDebug>

/**
  * Constructor
  */
KeyPairQueue::KeyPairQueue()
{
    this->lock = new QMutex();
    this->queue = new QQueue<std::wstring>();
    std::wstring path = this->getPath();
    QList<std::wstring>* pathAndFiles = this->splitPath(path);
    pathAndFiles = this->appendFiles(pathAndFiles);
    this->buildQueue(pathAndFiles);
}

/**
  * De-Constructor
  */
KeyPairQueue::~KeyPairQueue()
{
    delete this->queue;
    delete this->lock;
}

/**
  * 1. Make a list of all entries from GetEnvironmentPathW(“Path”), split by separator “;”
  *
  * Gets the environment var PATH.
  * Returns null if nothing found.
  */
std::wstring KeyPairQueue::getPath()
{
    DWORD bufferSize = 65535;
    std::wstring buff;
    buff.resize(bufferSize);
    bufferSize = GetEnvironmentVariable(L"PATH", &buff[0], bufferSize);
    if (!bufferSize){
        qWarning() << "No Path Found";
        return NULL;
    }
    buff.resize(bufferSize);
    #ifdef DEBUG
        qWarning() << "Got Path:" << QString::fromStdWString(buff);
    #endif
    return buff;
}

QList<std::wstring>* KeyPairQueue::splitPath(std::wstring path)
{
    uint start=0;
    QList<std::wstring>* result = new QList<std::wstring>();
    if(!path.empty()) {
        for(uint i=0; i<path.length(); i++)
        {
            if(i != start && path.at(i) == ';') {
                #ifdef DEBUG
                    qWarning() << "Split:" << QString::fromStdWString(path.substr(start, i-start));
                #endif
                result->append( path.substr(start, i-start) );
                start=i+1;
            }
        }
    }
    return result;
}


/**
  * 2. Append the list with all entries returned by FindFirstFileW / FindNextFileW by mask
  *
  * Adds to the list all program names found by FindFirstFileW / FindNextFileW
  */
QList<std::wstring>* KeyPairQueue::appendFiles(QList<std::wstring>* paths)
{
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    qWarning() << "Starting:" << paths->size();

    //Get the path to %PROGRAMFILES%
    QString searchDir = this->getProgramFilesPath() + "\\*";
    WCHAR* searchDirW = new WCHAR[searchDir.length()+1];
    searchDirW[searchDir.length()] = NULL;
    int size = searchDir.toWCharArray(searchDirW);
    #ifdef DEBUG
        qWarning() << "size used:" << size;
        qWarning() << "Searching Path:" << QString::fromWCharArray(searchDirW);
    #endif

    //find files
    hFind = FindFirstFileW(searchDirW, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        qWarning() << "FindFirstFileW failed:" << GetLastError();
    }
    else {
        if(FindFileData.cFileName[0] > 0x007A) {
            paths->append(FindFileData.cFileName);
            #ifdef DEBUG
                qWarning() << "Added File:" << QString::fromWCharArray(FindFileData.cFileName);
            #endif
        }
        while( FindNextFileW(hFind, &FindFileData) ) {
            if(FindFileData.cFileName[0] > 0x007A) {
                paths->append(FindFileData.cFileName);
                #ifdef DEBUG
                    qWarning() << "Added File:" << QString::fromWCharArray(FindFileData.cFileName);
                #endif
            }
        }
        FindClose(hFind);
    }
    qWarning() << "ending:" << paths->size();

    return paths;
}

/**
  * Gets the %PROGRAMFILES% environment var
  */
QString KeyPairQueue::getProgramFilesPath()
{
    DWORD bufferSize = 65535;
    std::wstring buff;
    buff.resize(bufferSize);
    bufferSize = GetEnvironmentVariable(L"PROGRAMFILES", &buff[0], bufferSize);
    if (!bufferSize){
        qWarning() << "No %PROGRAMFILES% Found!";
        return QString();
    }
    buff.resize(bufferSize);
    #ifdef DEBUG
        qWarning() << "Got %PROGRAMFILES%:" << QString::fromStdWString(buff);
    #endif
    return QString::fromStdWString(buff);;
}

/** 3. Make all possible pairs from the entries of the resulting list.
  *
  * Further Explanation:
  * The algorithm requires us to calculate a Cartesian square
  * of the combined list of strings from Path and %PROGRAMFILES%.
  *
  * Creates the Cartesian square of the given list.
  * Adds the results to the queue.
  *
  * Example:   A = [{1},{2},{3}]
  *          A^2 = [{1,1},{1,2},{1,3}
  *                 {2,1},{2,2},{2,3}
  *                 {3,1},{3,2},{3,3}]
  */
void KeyPairQueue::buildQueue(QList<std::wstring> *pathAndFiles)
{
    std::wstring temp;
    qWarning() << "buildQueue size:" << pathAndFiles->size();
    for(int column=0; column < pathAndFiles->size(); column++) {
        for(int row=0; row < pathAndFiles->size(); row++) {
            temp.clear();
            temp.append(pathAndFiles->at(column));
            temp.append(pathAndFiles->at(row));
            this->enqueue( temp );
        }
    }
}

std::wstring KeyPairQueue::dequeue()
{
    std::wstring result;
    this->lock->lock();
    if(!this->queue->isEmpty()){
        result = this->queue->dequeue();
    }
    else{
        result = std::wstring();
    }
    this->lock->unlock();
    return result;
}

int KeyPairQueue::getSize(){
    int size;
    this->lock->lock();
    size = this->queue->size();
    this->lock->unlock();
    return size;
}

void KeyPairQueue::enqueue(std::wstring keyPair)
{
    //this->lock->lock(); //since its private function, doesn't need lock
    this->queue->enqueue(keyPair);
    //this->lock->unlock();
    #ifdef DEBUG
        //qWarning() << "Queued KeyPair:" << keyPair;
    #endif
}
