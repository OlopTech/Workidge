#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QString>

class HttpServer : public QObject
{
    Q_OBJECT
public:
    explicit HttpServer(QObject *parent = nullptr);
    void startServer(quint16 port);

signals:
    void requestReceived(const QString &method, const QString &path, const QMap<QString, QString> &headers, const QByteArray &body);

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    QTcpServer *server;
    void handleRequest(QTcpSocket *socket); // Changement ici : Un seul argument
    QString generateResponse(const QString &method, const QString &path, const QMap<QString, QString> &headers, const QByteArray &body);
};

#endif // HTTPSERVER_H
