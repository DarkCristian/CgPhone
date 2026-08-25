#include "AppController.h"
#include "SystemAudioController.h"
#include "DiagnosticWindow.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#include <QStyle>
#include <QWindow>
#include <QCursor>
#include <QIcon>
#include <QEvent>
#include <QKeyEvent>
#ifdef Q_OS_WIN
#include <shobjidl.h>
#include <windows.h>
#endif

namespace {
class DiagnosticShortcutFilter final : public QObject {
public:
    explicit DiagnosticShortcutFilter(AppController *controller) : m_controller(controller) {}
protected:
    bool eventFilter(QObject *watched, QEvent *event) override {
        Q_UNUSED(watched)
        if (event->type() != QEvent::KeyPress) return false;
        const auto *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() != Qt::Key_F12 ||
            !(keyEvent->modifiers() & Qt::ShiftModifier) ||
            keyEvent->isAutoRepeat()) return false;
        QMetaObject::invokeMethod(m_controller, &AppController::toggleDebugConsole,
                                  Qt::QueuedConnection);
        return true;
    }
private:
    AppController *m_controller;
};

void applyAlwaysOnTop(QWindow *window, bool enabled) {
    if (!window) return;
#ifdef Q_OS_WIN
    const HWND nativeWindow = reinterpret_cast<HWND>(window->winId());
    SetWindowPos(nativeWindow, enabled ? HWND_TOPMOST : HWND_NOTOPMOST,
                 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
#else
    window->setFlag(Qt::WindowStaysOnTopHint, enabled);
    window->show();
#endif
}
}

int main(int argc, char *argv[]) {
#ifdef Q_OS_WIN
    SetCurrentProcessExplicitAppUserModelID(L"CgPhone.Softphone");
#endif
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    QCoreApplication::setOrganizationName("CgPhone");
    QCoreApplication::setApplicationName("CgPhone");
    const QIcon appIcon(QStringLiteral(":/qt/qml/CgPhone/assets/app/CgPhone.png"));
    app.setWindowIcon(appIcon);
    QQuickStyle::setStyle("Basic");
    AppController controller;
    DiagnosticShortcutFilter diagnosticShortcut(&controller);
    app.installEventFilter(&diagnosticShortcut);
    SystemAudioController systemAudio;
    DiagnosticWindow diagnosticWindow;
    QObject::connect(&controller, &AppController::debugConsoleToggleRequested,
                     &diagnosticWindow, &DiagnosticWindow::toggleVisibility);
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appController", &controller);
    engine.rootContext()->setContextProperty("systemAudio", &systemAudio);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, [] { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    engine.loadFromModule("CgPhone", "Main");
    if (engine.rootObjects().isEmpty()) return -1;

    if (controller.configurationMode()) {
        app.setQuitOnLastWindowClosed(true);
        return app.exec();
    }

    auto *window = qobject_cast<QWindow *>(engine.rootObjects().constFirst());
    if (window) {
        window->setIcon(appIcon);
        window->show();
        applyAlwaysOnTop(window, controller.alwaysVisible());
    }
    QSystemTrayIcon tray(appIcon);
    QMenu trayMenu;
    auto *answerAction = trayMenu.addAction(QObject::tr("Atender llamado"));
    auto *openAction = trayMenu.addAction(QObject::tr("Abrir CgPhone"));
    auto *logoutAction = trayMenu.addAction(QObject::tr("Desloguearse de la central"));
    trayMenu.addSeparator();
    auto *closeAction = trayMenu.addAction(QObject::tr("Cerrar CgPhone"));
    answerAction->setEnabled(controller.incoming());

    const auto showWindow = [window] {
        if (!window) return;
        window->setVisibility(QWindow::Windowed); window->show(); window->raise(); window->requestActivate();
    };
    QObject::connect(openAction, &QAction::triggered, &app, showWindow);
    QObject::connect(answerAction, &QAction::triggered, &controller, [&controller,showWindow]{ controller.answer(); showWindow(); });
    QObject::connect(logoutAction, &QAction::triggered, &controller, [&controller,showWindow]{ controller.logoutAgent(); showWindow(); });
    QObject::connect(closeAction, &QAction::triggered, &app, [&app,window] {
        if (QMessageBox::question(nullptr, QObject::tr("Cerrar CgPhone"), QObject::tr("¿Realmente desea cerrar CgPhone? Dejará de recibir llamadas.")) == QMessageBox::Yes) {
            if (window) window->setProperty("allowExit", true);
            app.quit();
        }
    });
    QObject::connect(&controller, &AppController::callChanged, answerAction, [&controller,answerAction]{ answerAction->setEnabled(controller.incoming()); });
    const auto refreshTrayTooltip = [&controller,&tray] {
        const auto identity = controller.sipUser().trimmed().isEmpty() ? QStringLiteral("CgPhone")
                                                                       : QStringLiteral("CgPhone · %1").arg(controller.sipUser());
        tray.setToolTip(QStringLiteral("%1 · %2").arg(identity, controller.registrationText()));
    };
    QObject::connect(&controller, &AppController::registrationChanged, &tray, refreshTrayTooltip);
    QObject::connect(&controller, &AppController::accountChanged, &app, [&controller,window,refreshTrayTooltip] {
        if (window) {
            window->show();
            applyAlwaysOnTop(window, controller.alwaysVisible());
            if (controller.alwaysVisible()) window->raise();
        }
        refreshTrayTooltip();
    });
    QObject::connect(&tray, &QSystemTrayIcon::activated, &app, [&trayMenu](QSystemTrayIcon::ActivationReason reason){ if (reason == QSystemTrayIcon::Trigger) trayMenu.popup(QCursor::pos()); });
    if (window) QObject::connect(window, &QWindow::visibilityChanged, &app, [window](QWindow::Visibility visibility){ if (visibility == QWindow::Minimized) window->hide(); });
    refreshTrayTooltip();
    tray.show();
    return app.exec();
}
