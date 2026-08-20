#include <QtTest>
#include <QSettings>
#include "CallHistoryModel.h"

class CoreTests : public QObject {
    Q_OBJECT
private slots:
    void init() { QCoreApplication::setOrganizationName("CgPhoneTests"); QCoreApplication::setApplicationName("Core"); QSettings().remove("callHistory"); }
    void historyStartsEmpty() { CallHistoryModel m; QCOMPARE(m.rowCount(), 0); }
    void historyPrependsCalls() {
        CallHistoryModel m;
        m.addCall({"101", "saliente", QDateTime::fromString("2026-01-01T10:00:00", Qt::ISODate), 65, false});
        m.addCall({"202", "entrante", QDateTime::fromString("2026-01-01T11:00:00", Qt::ISODate), 0, true});
        QCOMPARE(m.rowCount(), 2);
        QCOMPARE(m.data(m.index(0), CallHistoryModel::PeerRole).toString(), QString("202"));
        QCOMPARE(m.data(m.index(1), CallHistoryModel::DurationRole).toString(), QString("01:05"));
    }
    void clearHistory() { CallHistoryModel m; m.addCall({"1","saliente",QDateTime::currentDateTime(),1,false}); m.clear(); QCOMPARE(m.rowCount(),0); }
};
QTEST_MAIN(CoreTests)
#include "tst_core.moc"
