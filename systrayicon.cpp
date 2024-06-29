#include "systrayicon.h"
#include "qthread.h"

SysTrayIcon::SysTrayIcon(QObject *parent, QString port) : QSystemTrayIcon(parent)
{
    trayMenu = new QMenu();

    QAction *ccTextAction = new QAction("Le port actuellement utilisé est "+port, this);
    ccTextAction->setEnabled(false); // Désactiver l'action (non cliquable)
    trayMenu->addAction(ccTextAction);

    openAction = new QAction("Ouvrir", this);
    connect(openAction, &QAction::triggered, this, &SysTrayIcon::open);
    trayMenu->addAction(openAction);

    trayMenu->addSeparator();

    // Ajouter l'action "Quitter"
    QAction *quitAction = new QAction("Quitter", this);
    trayMenu->addAction(quitAction);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    this->setContextMenu(trayMenu);
    this->setIcon(QIcon(":/icon.png"));  // Assurez-vous d'avoir une icône appropriée
    this->show();
}

void SysTrayIcon::open()
{
    // Afficher une notification
    this->showCustomMessage("Information", "Workidge va redémarrer pour sauvegarder vos paramètres", QSystemTrayIcon::Warning, 5000);

    // Attendre un court instant avant de redémarrer
    QCoreApplication::processEvents();
    QThread::sleep(2);

    // Redémarrer l'application
    QStringList arguments = QCoreApplication::arguments();

    QString program = arguments.at(0);
    QStringList args = arguments.mid(1);
    args.append("--reboot");
    QProcess::startDetached(program, args);
    QCoreApplication::exit(0);
}

void SysTrayIcon::showCustomMessage(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon icon, int millisecondsTimeoutHint)
{
    // Afficher une notification système
    QSystemTrayIcon::showMessage(title, message, icon, millisecondsTimeoutHint);
}
