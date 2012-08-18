#include "keypairqueue.h"

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
        qCritical() << "No PATH Found";
        return NULL;
    }
    buff.resize(bufferSize);
    qDebug() << "Got Path var:" << QString::fromStdWString(buff);
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
                qDebug() << "Split:" << QString::fromStdWString(path.substr(start, i-start));
                result->append( path.substr(start, i-start) );
                start=i+1;
            }
        }
    }
    //Not sure what the virus does if the path has ';;' in it
    //So go ahead and append the empty string just in case
    result->append(L"");
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

    qDebug("appendFiles Starting with %i paths", paths->size());

    //Get the path to %PROGRAMFILES%
    QString searchDir = this->getProgramFilesPath() + "\\*";
    WCHAR* searchDirW = new WCHAR[searchDir.length()+1];
    searchDirW[searchDir.length()] = NULL;
    int size = searchDir.toWCharArray(searchDirW);
    qDebug() << "size used:" << size;
    qDebug() << "Searching Path:" << QString::fromWCharArray(searchDirW);

    //find files
    hFind = FindFirstFileW(searchDirW, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        qCritical("FindFirstFileW failed: %lu", GetLastError());
    }
    else {
        if(FindFileData.cFileName[0] > 0x007A) {
            paths->append(FindFileData.cFileName);
            qDebug() << "Added File:" << QString::fromWCharArray(FindFileData.cFileName);
        }
        while( FindNextFileW(hFind, &FindFileData) ) {
            if(FindFileData.cFileName[0] > 0x007A) {
                paths->append(FindFileData.cFileName);
                qDebug() << "Added File:" << QString::fromWCharArray(FindFileData.cFileName);
            }
        }
        FindClose(hFind);
    }
    qDebug("appendFiles Finished with %i paths", paths->size());
    delete searchDirW;

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
        qCritical("No PROGRAMFILES var Found!");
        return QString();
    }
    buff.resize(bufferSize);
    qDebug() << "Got PROGRAMFILES:" << QString::fromStdWString(buff);
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
    qDebug() << "buildQueue start size:" << pathAndFiles->size();
    for(int column=0; column < pathAndFiles->size(); column++) {
        for(int row=0; row < pathAndFiles->size(); row++) {
            temp.clear();
            temp.append(pathAndFiles->at(column));
            temp.append(pathAndFiles->at(row));

            //Don't use the case where both are empty string.
            //Causes queue issues and won't be the solution anyways.
            if(!temp.empty()) {
                this->enqueue( temp );
            }
        }
    }
    delete pathAndFiles;
    qDebug() << "buildQueue end size:" << this->getSize();
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
    //since its private function, doesn't need a lock
    this->queue->enqueue(keyPair);
    qDebug("Queued KeyPair: %ls", keyPair.c_str());
}
