#pragma once

#include <QObject>
#include <QColor>

class ChartModel;

class ChartModelObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int rating READ rating NOTIFY ratingChanged)
    Q_PROPERTY(int index READ index NOTIFY indexChanged)
    Q_PROPERTY(QColor color READ color NOTIFY colorChanged)

public:
    ChartModelObject(int mode, int index, int total, QObject* parent);

    QString name() const;
    int count() const;
    int rating() const;
    int index() const;
    QColor color() const;

    void setRating(int rating);
    void update(QString const& name, int count, int maxCount = 0);

signals:
    void nameChanged(QString);
    void countChanged(int);
    void ratingChanged(int);
    void indexChanged(int);
    void colorChanged(QColor);

private:
    int mMode = 0;
    QString mName;
    int mCount = 0;
    int mRating = 0;
    int mIndex = 0;
    int mTotal = 0;
    int mMaxCount = 0;
    QColor mColor;
    void setColor(int rating);
    void setColor2(int count);
};
