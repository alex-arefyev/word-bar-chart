#ifndef WORDCOUNTER_H
#define WORDCOUNTER_H

#include "reader/wordreader.h"
#include <QObject>
#include <QMap>
#include <mutex>

class WordReader;

class WordCounter
        : public QObject
        , public WordReaderCallback
{
    Q_OBJECT

public:
    struct Item
    {
        int index; // untouchable
        int count;
        int rating;
        QString word;
    };

    typedef std::vector<Item> Items;
    typedef std::vector<const Item> CItems;

public:
    explicit WordCounter(int mode, int barNum, int delay, QString const& filePath, QObject *parent = nullptr);

    bool read();
    void stop();

    inline void lock() { mItemsLock.lock(); }
    inline void unlock() { mItemsLock.unlock(); }

    const Items& items() const;
    int maxCount() const;

signals:
    void ratingChanged();
    void maxCountChanged(int);
    void filePathChanged(QString);
    void wordCountChanged(int);
    void totalCountChanged(int);
    void progressChanged(int);

public slots:

private:
    int mMode = 0;
    WordReader* mReader;
    bool mNotify = false;
    int mWordCounter = 0;
    int mTotalCounter = 0;
    int mProgress = 0;
    std::atomic_bool mContinue{false};

    QMap<QString, int> mWordCountMap;
    int mMaxWordCount = 1;

    Items mItems;
    std::mutex mItemsLock;

    std::function<bool(QString const&)> mFnAddWord;

    void started(QString const&) override;
    bool addWord(QString const& word) override;
    void changeProgress(int progress) override;

    Items::reverse_iterator _findByWord(QString const& word);
    Items::iterator _findMin();
    void _liftItemUp(Items::reverse_iterator const& baseItr);
    std::tuple<Items::iterator, Items::iterator, int>
    _reorderAlphabetically(Items::iterator const& baseItr);
    std::tuple<Items::iterator, Items::iterator, int>
    _reorderUp(Items::iterator const& baseItr);
    std::tuple<Items::iterator, Items::iterator, int>
    _reorderDown(Items::iterator const& baseItr);
    void _updateItemsRatingFrom(Items::iterator const& baseItr,
                                Items::iterator const& endItr, int rating);
    void _setMaxWordCount(int value);
    void _resetMaxWordCount();

    bool _addWordSortByCount(QString const& word);
    bool _addWordSortAlphabetically(QString const& word);
};

#endif // WORDCOUNTER_H
