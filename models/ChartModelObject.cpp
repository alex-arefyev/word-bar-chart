#include "ChartModelObject.h"
#include "ChartModel.h"

ChartModelObject::ChartModelObject(int mode, int index, int total, QObject* parent)
    : QObject(parent)
    , mMode(mode)
    , mIndex(index)
    , mTotal(total)
{
    Q_ASSERT(mTotal > 0);
    Q_ASSERT(mIndex < mTotal);
    setColor(mRating);
}

QString ChartModelObject::name() const
{
    return mName;
}

int ChartModelObject::count() const
{
    return mCount;
}

int ChartModelObject::rating() const
{
    return mRating;
}

int ChartModelObject::index() const
{
    return mIndex;
}

QColor ChartModelObject::color() const
{
    return mColor;
}

void ChartModelObject::setRating(int rating)
{
    Q_ASSERT(rating < mTotal);

    if (mRating != rating) {
        mRating = rating;
        emit ratingChanged(mRating);
        if (mMode != 0) {
            setColor(mRating);
        }
    }
}

void ChartModelObject::update(QString const& name, int count, int maxCount)
{
    if (mName != name) {
        mName = name;
        emit nameChanged(mName);
    }
    if (mCount != count) {
        mCount = count;
        emit countChanged(mCount);
        if (maxCount != mMaxCount) {
            mMaxCount = maxCount;
        }
        if (mMode == 0) {
            setColor2(count);
        }
        return;
    }
    if (maxCount != mMaxCount) {
        mMaxCount = maxCount;
        if (mMode == 0) {
            setColor2(count);
        }
    }
}

void ChartModelObject::setColor(int rating)
{
    double r = ((double)rating / mTotal);
    mColor = QColor::fromHslF(r * 0.8, 0.95, 0.5);
    mColor.setAlphaF(0.75);
    emit colorChanged(mColor);
}

void ChartModelObject::setColor2(int count)
{
    double r = ((double)(mMaxCount - count) / mMaxCount);
    mColor = QColor::fromHslF(r * 0.8, 0.95, 0.5);
    mColor.setAlphaF(0.75);
    emit colorChanged(mColor);
}
