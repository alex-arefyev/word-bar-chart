#pragma once

#include <QAbstractListModel>
#include <QDebug>

template <typename T>
class ObjectListModel : public QAbstractListModel
{
public:
    explicit ObjectListModel(QObject *parent = nullptr)
        : QAbstractListModel(parent)
    {}

    int size() const
    {
        return _data.size();
    }

    int rowCount(const QModelIndex &p) const
    {
        Q_UNUSED(p)
        return _data.size();
    }

    QVariant data(const QModelIndex &index, int role) const
    {
        Q_UNUSED(role)
        return QVariant::fromValue(_data[index.row()]);
    }

    QHash<int, QByteArray> roleNames() const
    {
        static QHash<int, QByteArray> roles = { { Qt::UserRole + 1, "object" } };
        return roles;
    }

public slots:
    void add(QObject * obj)
    {
        insert(obj, _data.size());
    }
    void insert(QObject * obj, int i)
    {
        if (i < 0 || i > _data.size()) i = _data.size();
        beginInsertRows(QModelIndex(), i, i);
        _data.insert(i, obj);
        obj->setParent(this);
        static_cast<T*>(this)->sizeChanged();
        endInsertRows();
    }

    QObject * take(int i)
    {
        if ((i > -1) && (i < _data.size()))
        {
            beginRemoveRows(QModelIndex(), i, i);
            QObject * obj = _data.takeAt(i);
            obj->setParent(0);
            static_cast<T*>(this)->sizeChanged();
            endRemoveRows();
            return obj;
        }
        else {
            qDebug() << "ERROR: take() failed - object out of bounds!";
        }
        return 0;
    }

    QObject* get(int i)
    {
        if ((i > -1) && (i < _data.size())) {
            return _data[i];
        }
        else {
            qDebug() << "ERROR: get() failed - object out of bounds!";
        }
        return 0;
    }

    void internalChange(QObject* obj)
    { // added to force sort/filter reevaluation
        int i = _data.indexOf(obj);
        if (i == -1) {
            qDebug() << "internal change failed, obj not found";
            return;
        } else {
            dataChanged(index(i), index(i));
        }
    }


protected:
    QList<QObject*> _data;

    void clear()
    {
        if (!_data.isEmpty())
        {
            beginRemoveRows(QModelIndex(), 0, _data.size() - 1);
            _data.clear();
            static_cast<T*>(this)->sizeChanged();
            endRemoveRows();
        }
    }

    void updateSize(int newSize)
    {
        if (newSize < _data.size())
        {
            beginRemoveRows(QModelIndex(), newSize, _data.size() - 1);
            int diff = _data.size() - newSize;
            _data.erase(_data.end() - diff, _data.end());
            static_cast<T*>(this)->sizeChanged();
            endRemoveRows();
        }
        else if (newSize > _data.size())
        {
            beginInsertRows(QModelIndex(), _data.size(), newSize - 1);
            _data.reserve(newSize);
            for (int i = _data.size(); i < newSize; ++i)
            {
                _data.append(static_cast<T*>(this)->createObject());
            }
            static_cast<T*>(this)->sizeChanged();
            endInsertRows();
        }
    }
};
