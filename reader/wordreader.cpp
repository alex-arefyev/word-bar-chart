#include "wordreader.h"
#include "lineparser.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QThread>
#include <chrono>

WordReader::WordReader(WordReaderCallback* cb, int delay, QString const& filePath, QObject* parent)
    : QObject(parent)
    , mCallback(cb)
    , mDelay(delay)
    , mFilePath(filePath)
{
    Q_ASSERT(mCallback);
}

bool WordReader::read()
{
    QFile file(mFilePath);
    bool ok = file.open(QIODevice::ReadOnly|QIODevice::Text);
    if (ok) {
        qint64 fileSize = file.size();
        mCallback->started(mFilePath);
        bool go = true;
        QString line;
        LineParser parser;
        QTextStream in(&file);
        while (go && !in.atEnd()) {
            line = in.readLine();
            qint64 pos = in.pos();
            int progress = qRound(((double)pos / fileSize) * 100);
            mCallback->changeProgress(progress);
            QStringList list = parser.parseLine(line);
            for (auto const& word: list) {
                go = mCallback->addWord(word.toLower());
                if (!go) {
                    break;
                }
                if (mDelay > 0) {
                    QThread::msleep(mDelay);
                }
            }
        }
    }
    return ok;
}
