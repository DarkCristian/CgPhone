#include "CallHistoryModel.h"
#include <QSettings>

CallHistoryModel::CallHistoryModel(QObject *parent) : QAbstractListModel(parent) { load(); }
int CallHistoryModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : m_entries.size(); }

QVariant CallHistoryModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.size()) return {};
    const auto &e = m_entries.at(index.row());
    switch (role) {
    case PeerRole: return e.peer;
    case DirectionRole: return e.direction;
    case TimestampRole: return e.timestamp.toString("dd/MM/yyyy  HH:mm");
    case DurationRole: return QString("%1:%2").arg(e.duration / 60, 2, 10, QLatin1Char('0')).arg(e.duration % 60, 2, 10, QLatin1Char('0'));
    case MissedRole: return e.missed;
    case DialTargetRole: return e.dialTarget;
    default: return {};
    }
}

QHash<int, QByteArray> CallHistoryModel::roleNames() const {
    return {{PeerRole,"peer"},{DirectionRole,"direction"},{TimestampRole,"timestamp"},{DurationRole,"duration"},{MissedRole,"missed"},{DialTargetRole,"dialTarget"}};
}

void CallHistoryModel::addCall(const Entry &entry) {
    beginInsertRows({}, 0, 0); m_entries.prepend(entry); endInsertRows(); save();
}
void CallHistoryModel::clear() { beginResetModel(); m_entries.clear(); endResetModel(); save(); }

void CallHistoryModel::load() {
    QSettings s;
    const int count = s.beginReadArray("callHistory");
    m_entries.reserve(count);
    for (int i = 0; i < count; ++i) {
        s.setArrayIndex(i);
        m_entries.push_back({s.value("peer").toString(), s.value("direction").toString(),
                             s.value("timestamp").toDateTime(), s.value("duration").toInt(),
                             s.value("missed").toBool(), s.value("dialTarget").toString()});
    }
    s.endArray();
}

void CallHistoryModel::save() const {
    QSettings s;
    s.beginWriteArray("callHistory", m_entries.size());
    for (int i = 0; i < m_entries.size(); ++i) {
        s.setArrayIndex(i); const auto &e = m_entries.at(i);
        s.setValue("peer", e.peer); s.setValue("direction", e.direction);
        s.setValue("timestamp", e.timestamp); s.setValue("duration", e.duration); s.setValue("missed", e.missed); s.setValue("dialTarget", e.dialTarget);
    }
    s.endArray(); s.sync();
}
