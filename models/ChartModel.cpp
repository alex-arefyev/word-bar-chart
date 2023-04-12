#include "ChartModel.h"
#include "ChartModelObject.h"

ChartModel::ChartModel(int mode, int barNum, QObject* parent)
    : ObjectListModel(parent)
{
    Q_ASSERT(barNum > 1);
    for (int i = 0; i < barNum; ++i) {
        add(new ChartModelObject(mode, i, barNum, this));
    }
}

QQmlListProperty<QObject> ChartModel::content()
{
    return QQmlListProperty<QObject>(this, _data);
}

QObject* ChartModel::createObject()
{
    return new ChartModelObject(0, 0, 1, this);
}

int ChartModel::maxCount() const
{
    return mMaxWordCount;
}

void ChartModel::clearWords()
{
    for (int i = 0; i < _data.size(); ++i) {
        ChartModelObject* obj = qobject_cast<ChartModelObject*>(_data[i]);
        obj->update(QString(), 0);
    }
    mMaxWordCount = 0;
    emit maxCountChanged(mMaxWordCount);
}

void ChartModel::sync(WordCounter::Items const& items)
{
    Q_ASSERT((int)items.size() == size());
    int maxCount = 0;//mMaxWordCount;
    std::for_each(items.begin(), items.end(), [&maxCount](WordCounter::Item const& item)
    {
        maxCount = qMax(maxCount, item.count);
    });
    for (auto const& item: items) {
        ChartModelObject* obj = qobject_cast<ChartModelObject*>(_data[item.index]);
        obj->update(item.word, item.count, maxCount);
        obj->setRating(item.rating);
        //maxCount = qMax(maxCount, item.count);
    }
    if (maxCount > mMaxWordCount) {
        mMaxWordCount = maxCount;
        emit maxCountChanged(mMaxWordCount);
    }
}
