
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "SECoP-Main.h"
#include "SECoP-Node.h"

#include "SECoP-StatusGui.h"

//using namespace testing;


#include "SECoP.h"
#include  "test_node.h"

#include <QRandomGenerator>
#include <QThread>
#include <QFuture>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QWidget>
#include <QList>
#include <QMap>
#include <QTcpSocket>
#include <QHostAddress>
#include <iostream>
#include <string>
#include <QRegularExpression>

#define CONTEXT_ID "default"
#define CONTEXT_ID_N2 "default_node2"

// Forward declaration of TabInfo struct
struct TabInfo;

#define NODE_PORT 2055
#define NODE_NAME "node1"


void noMessageOutput_protocol(QtMsgType, const QMessageLogContext &, const QString &)
{

}


class IOException : public std::exception {
private:
    std::string message;

public:
    // Constructor accepts a const char* that is used to set
    // the exception message
    IOException(const char* msg)
        : message(msg)
    {
    }

    // Override the what() method to return our message
    const char* what() const throw()
    {
        return message.c_str();
    }
};




class Client  {
public:
    Client();

    void    write(QString SECoP_message);
    QString read(int timeout_msecs = 3000);
    bool    connect(qint16 port);

private:
    QTcpSocket m_socket;


};

Client::Client(){

}

bool Client::connect(qint16 port){
    m_socket.connectToHost(QHostAddress("127.0.0.1"), port);

    return m_socket.waitForConnected();

}

QString Client::read(int timeout_msecs){
    m_socket.waitForReadyRead(timeout_msecs);

    QByteArray response = m_socket.readLine(1000);

    return QString(response);

}


void Client::write(QString SECoP_message){
    // Append newline
    SECoP_message.append("\n");

    QByteArray message =  SECoP_message.toUtf8();

    if(m_socket.write(message)== -1)
        throw IOException("Unable to write message");

    if(!m_socket.waitForBytesWritten())
        throw IOException("Unable to write message");

}



class SECoP_Protocol_Test : public ::testing::Test {
protected:
    void SetUp() override {
        // Code here will be called immediately after the constructor (right before each test).
        qInstallMessageHandler(noMessageOutput_protocol);


        Node_no_wait( "id1", NODE_NAME , NODE_PORT ,true);

        m_client.connect(NODE_PORT);
    }

    void TearDown() override {
        QThread::sleep(1);
        SECoP_S_doneLibrary(false,"default");
        QThread::sleep(1);
    }

public:
    Client m_client;



};


TEST_F(SECoP_Protocol_Test, delete_node) {

    m_client.write("change hpd:target 20");


    QString response =  m_client.read();
    // changed hpd:target [20.0,{"t":1722351020.974}]\n



    QRegularExpression regex(R"(changed hpd:target \[20\.0,\{"t":\d+\.?\d*\}\])");

    QRegularExpressionMatch match = regex.match(response);

    ASSERT_TRUE(match.hasMatch());



}














