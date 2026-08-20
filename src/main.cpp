#include "AppController.h"
#include "SystemAudioController.h"
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

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    QCoreApplication::setOrganizationName("CgPhone");
    QCoreApplication::setApplicationName("CgPhone");
    const QIcon appIcon(QStringLiteral(":/qt/qml/CgPhone/assets/app/CgPhone.png"));
    app.setWindowIcon(appIcon);
    QQuickStyle::setStyle("Basic");
    AppController controller;
    SystemAudioController systemAudio;
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
        window->setFlag(Qt::WindowStaysOnTopHint, controller.alwaysVisible());
        window->show();
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
            window->setFlag(Qt::WindowStaysOnTopHint, controller.alwaysVisible());
            window->show();
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
