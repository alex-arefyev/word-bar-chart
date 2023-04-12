#ifndef READERTHREAD_H
#define READERTHREAD_H

#include <QThread>

class WordCounter;

class ReaderThread : public QThread
{
    Q_OBJECT
public:
    ReaderThread(WordCounter* wc, QObject* parent = nullptr);

    void run() override;

signals:

public slots:

private:
    WordCounter* mCounter;
};

#endif // READERTHREAD_H
