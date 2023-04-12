#include "readerthread.h"
#include "reader/wordcounter.h"

ReaderThread::ReaderThread(WordCounter* wc, QObject *parent)
    : QThread(parent)
    , mCounter(wc)
{
    mCounter->moveToThread(this);
}

void ReaderThread::run()
{
    mCounter->read();
}
