#include <QCoreApplication>
#include <QApplication>
#include "httpserver.h"
#include "systrayicon.h"
#include <QFile>

int findAvailablePort() {
    QTcpServer server;
    if(server.listen(QHostAddress::LocalHost, 0)) {
        int port = server.serverPort();
        server.close();  // Ferme le serveur et libère le port
        return port;
    }
    return -1;  // retourne -1 si aucun port n'est disponible, ce qui est peu probable
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString fileName = "port";
    int port = 0;
    if(QFile::exists(fileName)){
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Impossible d'ouvrir le fichier.";
            return 1;
        }

        QTextStream in(&file);
        port = in.readLine().toInt();

        file.close();
        QTcpServer testserver;
        if(testserver.listen(QHostAddress::LocalHost, port)) {
            testserver.close();
        }else {
            port=findAvailablePort();
            QMessageBox::critical(nullptr, "Erreur fatale - Workidge", "Le port par défaut est occupé, nous utiliseront donc cet autre port : "+QString::number(port));
        }
    }else {
        port=findAvailablePort();
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Impossible d'ouvrir le fichier en écriture.";
            return 1;
        }

        QTextStream out(&file);
        out << QString::number(port) << Qt::endl;

        file.close();
    }

    HttpServer server;
    server.startServer(port);  // Démarrer le serveur sur le port 8080

    SysTrayIcon trayIcon(nullptr, QString::number(port));

    return a.exec();
}
