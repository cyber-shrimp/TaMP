#include "server.h"
#include <QCoreApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    QCoreApplication::setApplicationName("Qt Task Server");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Qt Task Application Server");
    parser.addHelpOption();
    parser.addVersionOption();
    
    QCommandLineOption portOption(
        QStringList() << "p" << "port",
        "Port to listen on (default: 33333)",
        "port",
        "33333"
    );
    parser.addOption(portOption);
    
    QCommandLineOption hostOption(
        QStringList() << "H" << "host",
        "Host address to bind to (default: 127.0.0.1)",
        "host",
        "127.0.0.1"
    );
    parser.addOption(hostOption);
    
    parser.process(a);

    quint16 port = static_cast<quint16>(parser.value(portOption).toInt());
    QHostAddress host(parser.value(hostOption));

    Server server;
    if (!server.startServer(host, port)) {
        return 1;
    }

    return a.exec();
}