#include "DiagnosticWindow.h"
#include <QCloseEvent>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QPlainTextEdit>
#include <QStandardPaths>
#include <QTimer>
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

    const QString logPath = QDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
                                .filePath(QStringLiteral("CgPhone-diagnostic.log"));
#ifdef Q_OS_WIN
    FILE *stream = nullptr;
    _wfreopen_s(&stream, reinterpret_cast<const wchar_t *>(QDir::toNativeSeparators(logPath).utf16()), L"w", stdout);
    _wfreopen_s(&stream, reinterpret_cast<const wchar_t *>(QDir::toNativeSeparators(logPath).utf16()), L"a", stderr);
#else
    freopen(logPath.toLocal8Bit().constData(), "w", stdout);
    freopen(logPath.toLocal8Bit().constData(), "a", stderr);
#endif
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);

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
    event->ignore();
    hide();
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
