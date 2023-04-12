#include "wordcounter.h"
#include "wordreader.h"

WordCounter::WordCounter(int mode, int barNum, int delay, const QString& filePath, QObject *parent)
    : QObject(parent)
    , mMode(mode)
    , mReader(new WordReader(this, delay, filePath, this))
    , mNotify(delay > 0)
{
    Q_ASSERT(barNum > 1);
    mFnAddWord = std::bind(&WordCounter::_addWordSortAlphabetically, this, std::placeholders::_1);
    mItems.reserve(barNum);
    for (int i = 0; i < barNum; ++i) {
        mItems.emplace_back(Item{i, 0, i, QString()});
    }
}

bool WordCounter::read()
{
    mContinue = true;
    if (mReader->read()) {
        if (!mNotify) {
            emit ratingChanged();
            emit wordCountChanged(mWordCounter);
            emit totalCountChanged(mTotalCounter);
        }
        return true;
    }
    return false;
}

void WordCounter::stop()
{
    mContinue = false;
}

WordCounter::Items const& WordCounter::items() const
{
    return mItems;
}

WordCounter::Items::reverse_iterator WordCounter::_findByWord(QString const& word)
{
    return std::find_if(mItems.rbegin(), mItems.rend(), [word](Item const& item)
    {
        return item.word == word;
    });
}

WordCounter::Items::iterator WordCounter::_findMin()
{
    auto itr = std::min_element(mItems.begin(), mItems.end(),
                                  [](Item const& item1, Item const& item2)
    {
        return item1.count < item2.count;
    });
    return itr;
    //return r_itr == mItems.rend() ? i : r_itr;
}

void WordCounter::_liftItemUp(WordCounter::Items::reverse_iterator const& baseItr)
{
    Items::reverse_iterator r_itr = baseItr;
    Items::reverse_iterator r_itr_next = baseItr;
    ++r_itr_next;
    if (r_itr_next == mItems.rend()) { // the item is the topmost, just update max word count
        _setMaxWordCount(qMax(mMaxWordCount, (*r_itr).count));
    } else {
        // lift the item till its count is less then an upper item's one
        for (; r_itr_next != mItems.rend(); ++r_itr_next, ++r_itr) {
            if ((*r_itr).count > (*r_itr_next).count) {
                std::swap(*r_itr_next, *r_itr);
            } else {
                break;
            }
        }
        Items::iterator itr(r_itr_next.base());
        int rating = r_itr_next == mItems.rend() ? 0 : ((*r_itr_next).rating + 1);
        _setMaxWordCount(qMax(mMaxWordCount, (*itr).count));
        _updateItemsRatingFrom(itr, baseItr.base(), rating);
    }
}

static bool compareWords(QString const& s1, QString const& s2)
{
    if (s1.isEmpty()) return false;
    if (s2.isEmpty()) return true;
    return s1 < s2;
}

std::tuple<WordCounter::Items::iterator, WordCounter::Items::iterator, int>
WordCounter::_reorderAlphabetically(const WordCounter::Items::iterator& baseItr)
{
    if (baseItr == mItems.begin()) {
        return {mItems.end(), mItems.end(), -1};
    }

    Items::iterator itr_prev(baseItr - 1);
    bool up = !compareWords((*itr_prev).word, (*baseItr).word);
    if (up) {
        return _reorderUp(baseItr);
    } else {
        return _reorderDown(baseItr);
    }
}

std::tuple<WordCounter::Items::iterator, WordCounter::Items::iterator, int>
WordCounter::_reorderUp(Items::iterator const& baseItr)
{
    if (baseItr == mItems.end()) {
        return {mItems.end(), mItems.end(), -1};
    }

    Items& v = mItems;
    Items::iterator itr = baseItr;
    Items::iterator itr_prev = itr - 1;

    for (; itr != v.begin();) {

        if (!compareWords((*itr_prev).word, (*itr).word)) {

            std::swap(*itr_prev, *itr);

            itr = itr_prev;
            itr_prev = itr - 1;

        } else {
            break;
        }
    }
    int rating = itr == mItems.begin() ? 0 : (*(itr_prev)).rating + 1;
    return {itr, baseItr + 1, rating};
}

std::tuple<WordCounter::Items::iterator, WordCounter::Items::iterator, int>
WordCounter::_reorderDown(Items::iterator const& baseItr)
{
    if (std::distance(baseItr, mItems.end()) < 2) {
        return {mItems.end(), mItems.end(), -1};
    }

    Items& v = mItems;
    Items::iterator itr = baseItr;
    Items::iterator itr_next = itr + 1;

    int rating = (*baseItr).rating;

    for (; itr_next != v.end();) {

        if (compareWords((*itr_next).word, (*itr).word)) {

            std::swap(*itr_next, *itr);

            itr = itr_next;
            itr_next = itr + 1;

        } else {
            break;
        }
    }
    return {baseItr, itr_next, rating};
}

void WordCounter::_updateItemsRatingFrom(Items::iterator const& baseItr, Items::iterator const& endItr, int rating)
{
    for (auto itr = baseItr; itr != endItr; ++itr, ++rating) {
        Item& item = *itr;
        item.rating = rating;
    }
}

void WordCounter::_setMaxWordCount(int value)
{
    if (mMaxWordCount < value) {
        mMaxWordCount = value;
        emit maxCountChanged(mMaxWordCount);
    }
}

int WordCounter::maxCount() const
{
    return mMaxWordCount;
}

void WordCounter::started(QString const& filePath)
{
    emit filePathChanged(filePath);
}

void WordCounter::_resetMaxWordCount()
{
    mMaxWordCount = 0;
    emit maxCountChanged(mMaxWordCount);
}

bool WordCounter::addWord(QString const& word)
{
    if (mMode == 0) {
        return _addWordSortAlphabetically(word);
    }

    return _addWordSortByCount(word);
}

void WordCounter::changeProgress(int progress)
{
    if (mProgress != progress) {
        mProgress = progress;
        emit progressChanged(mProgress);
    }
}

bool WordCounter::_addWordSortByCount(QString const& word)
{
    auto& count = mWordCountMap[word];
    bool newWord = count == 0;
    ++count;

    bool modified = false;

    {
        std::lock_guard<WordCounter> _(*this);

        if (count > mItems.back().count) {
            auto r_itr = _findByWord(word);
            if (r_itr == mItems.rend()) { // the word is not in the chart yet

                Items::reverse_iterator r_itr = mItems.rbegin();

                Item& item = *r_itr;
                item.count = count;
                item.word = word;

                _liftItemUp(r_itr);

            } else {
                (*r_itr).count = count;

                _liftItemUp(r_itr);
            }
            modified = true;
        }
    }

    if (mNotify && modified) {
        emit ratingChanged();
    }

    if (newWord) {
        ++mWordCounter;
        if (mNotify) {
            emit wordCountChanged(mWordCounter);
        }
    }

    ++mTotalCounter;
    if (mNotify) {
        emit totalCountChanged(mTotalCounter);
    }

    return mContinue;
}

bool WordCounter::_addWordSortAlphabetically(QString const& word)
{
    auto& count = mWordCountMap[word];
    bool newWord = count == 0;
    ++count;

    bool modified = false;

    {
        std::lock_guard<WordCounter> _(*this);

        auto r_itr = _findByWord(word);
        if (r_itr == mItems.rend()) { // the word is not in the chart yet
            auto itr = _findMin();
            Item& item = *itr;
            if (item.count < count) {
                item.count = count;
                item.word = word;

//                std::sort(mItems.begin(), mItems.end(),
//                          [](Item const& item1, Item const& item2)
//                {
//                    return compareWords(item1.word, item2.word);
//                });
//                _updateItemsRatingFrom(mItems.begin(), 0);

                if (itr != mItems.begin()) {
                    Items::iterator itr1, itr2;
                    int rating;
                    std::tie(itr1, itr2, rating) = _reorderAlphabetically(itr);
                    _updateItemsRatingFrom(itr1, itr2, rating);
                } else {
                    Items::iterator itr1, itr2;
                    int rating;
                    std::tie(itr1, itr2, rating) = _reorderDown(itr);
                    _updateItemsRatingFrom(itr1, itr2, rating);
                }

                modified = true;
            }
        } else {
            Item& item = *r_itr;
            item.count = count;
            modified = true;
        }
    }

    if (mNotify && modified) {
        emit ratingChanged();
    }

    if (newWord) {
        ++mWordCounter;
        if (mNotify) {
            emit wordCountChanged(mWordCounter);
        }
    }

    ++mTotalCounter;
    if (mNotify) {
        emit totalCountChanged(mTotalCounter);
    }

    return mContinue;
}
