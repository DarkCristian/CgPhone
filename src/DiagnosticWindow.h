#pragma once

#include <QWidget>
#include <QFile>

class QPlainTextEdit;
class QTimer;
class QCloseEvent;

class DiagnosticWindow final : public QWidget {
    Q_OBJECT
public:
    explicit DiagnosticWindow(QWidget *parent = nullptr);
public slots:
    void toggleVisibility();
protected:
    void closeEvent(QCloseEvent *event) override;
private slots:
    void refreshLog();
private:
    QPlainTextEdit *m_output = nullptr;
    QTimer *m_refreshTimer = nullptr;
    QFile m_logFile;
    qint64 m_position = 0;
};
