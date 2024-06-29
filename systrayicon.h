#ifndef SYSTRAYICON_H
#define SYSTRAYICON_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QCoreApplication>
#include <QProcess>

class SysTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit SysTrayIcon(QObject *parent = nullptr, QString port=0);

    void showCustomMessage(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int millisecondsTimeoutHint = 5000);

private slots:
    void open();

private:
    QMenu *trayMenu;
    QAction *openAction;
};

#endif // SYSTRAYICON_H
