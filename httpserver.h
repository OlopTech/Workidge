#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>

class HttpServer : public QObject
{
    Q_OBJECT
public:
    explicit HttpServer(QObject *parent = nullptr);
    void startServer(quint16 port);

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    void handleRequest(QTcpSocket *socket);
    QByteArray generateResponse(const QString &method, const QString &path, const QMap<QString, QString> &headers, const QByteArray &body);

    QTcpServer *server;
};

#endif // HTTPSERVER_H
