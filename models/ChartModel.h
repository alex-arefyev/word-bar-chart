#pragma once

#include <QQmlListProperty>
#include "ObjectListModel.h"
#include "reader/wordcounter.h"
#include <vector>

namespace data
{
    struct DeviceNameList;
}

class ChartModel : public ObjectListModel<ChartModel>
{
    Q_OBJECT
    Q_PROPERTY(int size READ size NOTIFY sizeChanged)
    Q_PROPERTY(QQmlListProperty<QObject> content READ content)
    Q_PROPERTY(QObject * parent READ parent WRITE setParent)
    Q_PROPERTY(int maxCount READ maxCount NOTIFY maxCountChanged)
    Q_CLASSINFO("DefaultProperty", "content")

signals:
    void sizeChanged();
    void maxCountChanged(int);

public:
    explicit ChartModel(int mode, int barNum, QObject* parent);

    QQmlListProperty<QObject> content();
    QObject* createObject();

    int maxCount() const;

    void clearWords();
    void sync(WordCounter::Items const& items);

private:
    int mMaxWordCount = 0;
};
