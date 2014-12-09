#include "mainwindow.h"
#include <QApplication>
#include <QLocalSocket>
#include <QLocalServer>

int main(int argc, char *argv[])
{
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(500))
        return; // Exit already a process running

    QLocalServer m_localServer = new QLocalServer(this);
    connect(m_localServer, SIGNAL(newConnection()), this, SLOT(newLocalSocketConnection()));
    m_localServer->listen(serverName);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
