#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include <QVector>

class CallHistoryModel final : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles { PeerRole = Qt::UserRole + 1, DirectionRole, TimestampRole, DurationRole, MissedRole, DialTargetRole };
    struct Entry { QString peer; QString direction; QDateTime timestamp; int duration = 0; bool missed = false; QString dialTarget; };

    explicit CallHistoryModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void addCall(const Entry &entry);
    Q_INVOKABLE void clear();

private:
    void load();
    void save() const;
    QVector<Entry> m_entries;
};
