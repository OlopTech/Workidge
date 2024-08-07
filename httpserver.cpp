#include "httpserver.h"
#include <QDebug>
#include <QRegularExpression>
#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>

HttpServer::HttpServer(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &HttpServer::onNewConnection);
}

void HttpServer::startServer(quint16 port)
{
    if (!server->listen(QHostAddress::Any, port))
    {
        qDebug() << "Server could not start!";
    }
    else
    {
        qDebug() << "Server started!";
    }
}

void HttpServer::onNewConnection()
{
    QTcpSocket *socket = server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &HttpServer::onReadyRead);
}

void HttpServer::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) {
        return;
    }

    while (socket->canReadLine())
    {
        QString requestLine = socket->readLine().trimmed();
        if (requestLine.isEmpty()) {
            // Fin des en-têtes, traitement de la requête
            handleRequest(socket);
            return;
        }

        // Stocker les lignes de la requête pour analyse ultérieure
        if (!socket->property("request").isValid()) {
            socket->setProperty("request", QVariant::fromValue(QStringList()));
        }

        QStringList requestLines = socket->property("request").toStringList();
        requestLines.append(requestLine);
        socket->setProperty("request", QVariant::fromValue(requestLines));
    }
}

void HttpServer::handleRequest(QTcpSocket *socket)
{
    QStringList requestLines = socket->property("request").toStringList();
    if (requestLines.isEmpty()) {
        socket->close();
        return;
    }

    // Parse the request line
    QRegularExpression requestLineRegex("^(\\S+)\\s(\\S+)\\sHTTP/1\\.1$");
    QRegularExpressionMatch match = requestLineRegex.match(requestLines.first());
    if (!match.hasMatch())
    {
        qDebug() << "Invalid request line";
        socket->close();
        return;
    }

    QString method = match.captured(1);
    QString path = match.captured(2);
    QMap<QString, QString> headers;
    QByteArray body;

    // Read the headers
    for (int i = 1; i < requestLines.size(); ++i) {
        const QString& line = requestLines[i];
        if (line.isEmpty()) {
            // End of headers
            break;
        }
        QStringList headerParts = line.split(": ");
        if (headerParts.size() == 2) {
            headers[headerParts[0]] = headerParts[1].trimmed();
        }
    }

    // Read the body (if any)
    if (headers.contains("Content-Length"))
    {
        int contentLength = headers["Content-Length"].toInt();
        body = socket->read(contentLength);
    }

    // Generate and send the response
    QByteArray response = generateResponse(method, path, headers, body);
    socket->write(response);
    socket->flush();
    socket->close();
}

QByteArray HttpServer::generateResponse(const QString &method, const QString &path, const QMap<QString, QString> &headers, const QByteArray &body)
{
    Q_UNUSED(headers);
    Q_UNUSED(body);

    QByteArray response;

    if (method == "GET" && path == "/")
    {
        response.append("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
        response.append("Hello, World!");
    }
    else if (method == "GET" && path == "/test")
    {
        response.append("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
        response.append("This is a test page!");
    }
    else if (method == "GET" && path.startsWith("/getfile/cc/"))
    {
        QString filePath = path.mid(QString("/getfile/cc/").length());
        QFile file(filePath);

        if (file.exists() && file.open(QIODevice::ReadOnly))
        {
            QFileInfo fileInfo(file);
            QMimeDatabase mimeDb;
            QMimeType mimeType = mimeDb.mimeTypeForFile(fileInfo);
            QByteArray fileContent = file.readAll();
            file.close();

            response.append("HTTP/1.1 200 OK\r\n");
            response.append("Content-Type: " + mimeType.name().toUtf8() + "\r\n");
            response.append("Content-Disposition: attachment; filename=\"" + fileInfo.fileName().toUtf8() + "\"\r\n");
            response.append("Content-Length: " + QByteArray::number(fileContent.size()) + "\r\n");
            response.append("\r\n");
            response.append(fileContent);
        }
        else
        {
            response.append("HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n");
            response.append("File not found.");
        }
    }
    else
    {
        response.append("HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n");
        response.append("Page not found.");
    }

    return response;
}
