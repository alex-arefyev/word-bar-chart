#ifndef WORDBARCHART_H
#define WORDBARCHART_H

#include <QPointer>
#include <QObject>
#include <QMap>
#include <atomic>

class ChartModel;
class WordCounter;

class WordBarChart
        : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(WordBarChart)

    Q_PROPERTY(bool animate READ animate NOTIFY animateChanged)
    Q_PROPERTY(bool reading READ reading NOTIFY readingChanged)
    Q_PROPERTY(int wordCounter READ wordCounter NOTIFY wordCounterChanged)
    Q_PROPERTY(int totalCounter READ totalCounter NOTIFY totalCounterChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString filePath READ filePath NOTIFY started)

public:
    WordBarChart(int mode, int barNum, int delay, QObject* parent = nullptr);

    Q_INVOKABLE void load(QString const& fileUrl);
    Q_INVOKABLE void stop();

    ChartModel* model() const;

    bool animate() const;
    bool reading() const;
    int wordCounter() const;
    int totalCounter() const;
    int progress() const;
    QString filePath() const;

signals:
    void animateChanged(bool);
    void readingChanged(bool);
    void wordCounterChanged(int);
    void totalCounterChanged(int);
    void progressChanged(int);
    void started(QString);

private slots:
    void setWordCounter(int value);
    void setTotalCounter(int value);
    void setProgress(int value);

private:
    int const mMode;
    int const mDelay;
    std::atomic_bool mContinue{false};
    ChartModel* mModel;
    QPointer<WordCounter> mCounter;
    int mWordCounter = 0;
    int mTotalCounter = 0;
    int mProgress = 0;
    QString mFilePath;

    void setFilePath(QString const& filePath);
};

#endif // WORDBARCHART_H
