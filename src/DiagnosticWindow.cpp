#include "DiagnosticWindow.h"
#include <QCloseEvent>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QEvent>
#include <QPlainTextEdit>
#include <QStandardPaths>
#include <QTimer>
#include <QTextCursor>
#include <QVBoxLayout>
#include <cstdio>

DiagnosticWindow::DiagnosticWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle(tr("CgPhone · Diagnóstico SIP"));
    resize(820, 500);
    setAttribute(Qt::WA_QuitOnClose, false);

    m_output = new QPlainTextEdit(this);
    m_output->setReadOnly(true);
    m_output->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_output->setStyleSheet(QStringLiteral(
        "QPlainTextEdit { background:#0B1220; color:#D7E3F4; border:0;"
        "font-family:Consolas,monospace; font-size:11pt; padding:8px; }"));
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_output);

    const QString logDirectory = QDir(QStandardPaths::writableLocation(
        QStandardPaths::AppLocalDataLocation)).filePath(QStringLiteral("logs"));
    QDir().mkpath(logDirectory);
    const QString logPath = QDir(logDirectory).filePath(QStringLiteral("cgphone-sip.log"));

    m_output->setPlaceholderText(tr("Esperando actividad SIP de PJSIP…"));
    m_logFile.setFileName(logPath);
    m_logFile.open(QIODevice::ReadOnly | QIODevice::Text);
    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setInterval(250);
    connect(m_refreshTimer, &QTimer::timeout, this, &DiagnosticWindow::refreshLog);
    m_refreshTimer->start();
}

void DiagnosticWindow::toggleVisibility() {
    if (isVisible()) {
        hide();
    } else {
        refreshLog();
        show();
        raise();
        activateWindow();
    }
}

void DiagnosticWindow::closeEvent(QCloseEvent *event) {
    // Cerrar la ventana de diagnóstico sólo la oculta. No finaliza CgPhone y
    // puede volver a mostrarse con Shift+F12.
    event->ignore();
    hide();
}

void DiagnosticWindow::changeEvent(QEvent *event) {
    QWidget::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange && isMinimized())
        QTimer::singleShot(0, this, &QWidget::hide);
}

void DiagnosticWindow::refreshLog() {
    if (!m_logFile.isOpen()) {
        if (!m_logFile.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    }
    if (m_logFile.size() < m_position) {
        m_position = 0;
        m_output->clear();
    }
    if (m_logFile.size() == m_position) return;
    m_logFile.seek(m_position);
    const QByteArray bytes = m_logFile.readAll();
    m_position = m_logFile.pos();
    if (!bytes.isEmpty()) {
        m_output->moveCursor(QTextCursor::End);
        m_output->insertPlainText(QString::fromLocal8Bit(bytes));
        m_output->moveCursor(QTextCursor::End);
    }
}
