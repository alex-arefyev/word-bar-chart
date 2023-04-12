#include "wordbarchart.h"
#include "reader/wordcounter.h"
#include "reader/readerthread.h"
#include "models/ChartModel.h"
#include <QUrl>

WordBarChart::WordBarChart(int mode, int barNum, int delay, QObject* parent)
    : QObject(parent)
    , mMode(mode)
    , mDelay(delay)
    , mModel(new ChartModel(mode, barNum, this))
{}

void WordBarChart::load(QString const& fileUrl)
{
    if (mCounter) { return; }

    QUrl const url(fileUrl);
    if (!url.isLocalFile()) { return; }

    QString filePath = url.toLocalFile();
    WordCounter* wc = new WordCounter(mMode, mModel->size(), mDelay, filePath);
    ReaderThread* th = new ReaderThread(wc);

    connect(th, &QThread::started, this, [this]()
    {
        emit readingChanged(true);
    },
    Qt::QueuedConnection);

    connect(th, &QThread::finished,this,  [this, th]()
    {
        th->deleteLater();
        mCounter->deleteLater();
        mCounter.clear();
        emit readingChanged(false);
    },
    Qt::QueuedConnection);

    connect(wc, &WordCounter::ratingChanged, this, [this, wc]()
    {
        std::lock_guard<WordCounter> _(*wc);
        WordCounter::Items const& items = wc->items();
        mModel->sync(items);
    },
    Qt::QueuedConnection);

    connect(wc, &WordCounter::wordCountChanged,
            this, &WordBarChart::setWordCounter,
            Qt::QueuedConnection);

    connect(wc, &WordCounter::totalCountChanged,
            this, &WordBarChart::setTotalCounter,
            Qt::QueuedConnection);

    connect(wc, &WordCounter::progressChanged,
            this, &WordBarChart::setProgress,
            Qt::QueuedConnection);

    mModel->clearWords();
    setWordCounter(0);
    setTotalCounter(0);
    setProgress(0);
    setFilePath(filePath);

    mContinue = true;
    mCounter = wc;
    th->start();
}

void WordBarChart::stop()
{
    if (mCounter) {
        mCounter->stop();
    }
}

ChartModel* WordBarChart::model() const
{
    return mModel;
}

bool WordBarChart::animate() const
{
    return mDelay > 31;
}

bool WordBarChart::reading() const
{
    return mCounter;
}

int WordBarChart::wordCounter() const
{
    return mWordCounter;
}

int WordBarChart::totalCounter() const
{
    return mTotalCounter;
}

int WordBarChart::progress() const
{
    return mProgress;
}

QString WordBarChart::filePath() const
{
    return mFilePath;
}

void WordBarChart::setFilePath(QString const& filePath)
{
    mFilePath = filePath;
    emit started(mFilePath);
}

void WordBarChart::setWordCounter(int value)
{
    if (mWordCounter != value) {
        mWordCounter = value;
        emit wordCounterChanged(mWordCounter);
    }
}

void WordBarChart::setTotalCounter(int value)
{
    if (mTotalCounter != value) {
        mTotalCounter = value;
        emit totalCounterChanged(mTotalCounter);
    }
}

void WordBarChart::setProgress(int value)
{
    if (mProgress != value) {
        mProgress = value;
        emit progressChanged(mProgress);
    }
}
