#ifndef WORDREADER_H
#define WORDREADER_H

#include <QObject>

class LineParser;

struct WordReaderCallback
{
    virtual void started(QString const&) = 0;
    virtual bool addWord(QString const&) = 0;
    virtual void changeProgress(int) = 0;
};

class WordReader : public QObject
{
    Q_OBJECT

public:
    WordReader(WordReaderCallback* cb,
               int delay, QString const& filePath, QObject* parent = nullptr);

    bool read();

private:
    WordReaderCallback* mCallback;
    int mDelay = 0;
    QString mFilePath;
};

#endif // WORDREADER_H
