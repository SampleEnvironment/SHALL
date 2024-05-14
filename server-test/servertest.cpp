#include <QtTest>
#include <QCoreApplication>
#include "SECoP.h"
#include "SECoP-Main.h"

#include <QRandomGenerator>
#include <QThread>
#include <QFuture>
#include <QtConcurrent>
#include <QFutureWatcher>


#define CONTEXT_ID "default"
#define CONTEXT_ID_N2 "default_node2"


#include <QtGlobal>


void noMessageOutput(QtMsgType, const QMessageLogContext &, const QString &)
{

}



void Local_GetTemperature(const char* name, enum SECoP_S_error* piError, CSECoPbaseType** ppData, CSECoPbaseType** ppSigma, double* timestamp)
{
    Q_UNUSED(name);
    Q_UNUSED(piError);
    Q_UNUSED(timestamp);
    *ppSigma = SECoP_V_create("{\"type\":\"double\"}");

    double randomDouble = QRandomGenerator::global()->generateDouble();

    SECoP_V_modifyDouble(*ppData, 0, 0, randomDouble);
    SECoP_V_modifyDouble(*ppSigma, 0, 0, 0.01); // fixed error value
}


void Local_GetTarget(const char* name, enum SECoP_S_error* piError, CSECoPbaseType** ppData, CSECoPbaseType** ppSigma, double* timestamp)
{
    Q_UNUSED(name);
    Q_UNUSED(piError);
    Q_UNUSED(ppSigma);
    Q_UNUSED(timestamp);

    SECoP_V_modifyDouble(*ppData, 0, 0, 20);

}

void Local_SetTarget(const char* name, enum SECoP_S_error* piError, CSECoPbaseType** ppData, CSECoPbaseType** ppSigma, double* timestamp)
{
    Q_UNUSED(name);
    Q_UNUSED(ppSigma);
    Q_UNUSED(timestamp);
    printf("settarget\n");
    fflush(stdout);

    double dblTarget(std::numeric_limits<double>::quiet_NaN());

    if (SECoP_V_getDouble(*ppData, 0, 0, &dblTarget))
    {
        if (dblTarget < -273.15)
            dblTarget = -273.15;
        else if (dblTarget > 1000.0)
            dblTarget = 1000.0;
        SECoP_V_modifyDouble(*ppData, 0, 0, dblTarget);
    }
    else
        *piError = SECoP_S_ERROR_INVALID_VALUE;

}


void Local_GetStatus(const char* name, enum SECoP_S_error* piError, CSECoPbaseType** ppData, CSECoPbaseType** ppSigma, double* timestamp)
{
    Q_UNUSED(name);
    Q_UNUSED(piError);
    Q_UNUSED(ppSigma);
    Q_UNUSED(timestamp);

    QString szStatus("IDLE");
    qint64 i(0);
    if (szStatus.contains(QString("idle"), Qt::CaseInsensitive))
    {
        szStatus.clear();
        //Counting flowers on the wall, that don't bother me at all Playing solitaire til dawn with a deck of fiftyone Smoking cigarettes and watching Captain Kangaroo Now don't tell me I've
        szStatus.append("IDLE Counting flowers on the wall, that don't bother me at all Playing solitaire til dawn with a deck of fiftyone Smoking cigarettes and watching Captain Kangaroo Now don't tell me I've nothing to do");
        i |= 0x01;
    }
    if (szStatus.contains(QString("pause"), Qt::CaseInsensitive))
        i |= 0x02;
    if (szStatus.contains(QString("start"), Qt::CaseInsensitive))
        i |= 0x04;
    if (szStatus.contains(QString("stop"), Qt::CaseInsensitive))
        i |= 0x08;
    if (szStatus.contains(QString("reset"), Qt::CaseInsensitive))
        i |= 0x10;
    if (szStatus.contains(QString("shutdown"), Qt::CaseInsensitive))
        i |= 0x20;
    if (szStatus.contains(QString("finish"), Qt::CaseInsensitive))
        i |= 0x40;
    switch (i) // only one word should found to map a status
    {
    case 0x01: i = 100; break;
    case 0x02: i = 101; break;
    case 0x04: i = 300; break;
    case 0x08: i = 200; break;
    case 0x10: i = 400; break;
    case 0x20: i = 400; break;
    case 0x40: i = 100; break;
    default: i = -1; break; // unknown, if less or more words are found
    }
    SECoP_V_modifyInteger(*ppData, 1, 0, i);
    SECoP_V_modifyString(*ppData, 2, qUtf8Printable(szStatus), -1);


}

void funcCall(const char* name, const CSECoPbaseType* pArgument, enum SECoP_S_error* piError, CSECoPbaseType** ppReturn, double* timestamp)
{
    Q_UNUSED(pArgument);
    Q_UNUSED(piError);
    Q_UNUSED(ppReturn);
    Q_UNUSED(timestamp);

    QString szCommand(name);
    if (szCommand.endsWith(":stop"))
    {
        printf("settarget\n");
        fflush(stdout);
    }

}





void Node1(const char* context_id,const char* Node_id,unsigned short port){

    SECoP_S_initLibrary(nullptr, true, true,context_id);

    SECoP_S_createNode(Node_id, "TestNode", port,context_id);
    //      SECoP_S_addPropertyJSON("order","[\"hpdtest\"]");
    SECoP_S_addModule("hpd",context_id);
    SECoP_S_addPropertyString("description", "Hotplate drivable",context_id);
    SECoP_S_addPropertyJSON("interface_classes", "[\"Drivable\",\"Writable\",\"Readable\"]",context_id);
    SECoP_S_addPropertyDouble("pollinterval", 10.0,context_id);
    SECoP_S_addReadableParameter("value", &Local_GetTemperature,context_id);
    SECoP_S_addPropertyJSON("datainfo", "{\"type\":\"double\",\"unit\":\"K\"}",context_id);
    SECoP_S_addPropertyDouble("pollinterval", 1.0,context_id);
    SECoP_S_addPropertyString("description", "actual temperature",context_id);
    SECoP_S_addReadableParameter("status", &Local_GetStatus,context_id);
    SECoP_S_addPropertyJSON("datainfo", "{\"type\":\"tuple\",\"members\":[{\"type\":\"enum\",\"members\":{\"IDLE\":100,\"WARN\":200,\"BUSY\":300,\"BUSY_Stabilizing\":380,\"ERROR\":400,\"DISABLED\":0}},{\"type\":\"string\"}]}",context_id);
    SECoP_S_addPropertyString("description", "machine status",context_id);
    SECoP_S_addPropertyDouble("pollinterval", 1.0,context_id);
    //          SECoP_S_addReadableParameter2("useramp2", &SECoPModul::getRampBool, SECoPModul::theInstance);
    SECoP_S_addWritableParameter("target", &Local_GetTarget, &Local_SetTarget,context_id);
    SECoP_S_addPropertyJSON("datainfo", "{\"type\":\"double\",\"unit\":\"K\"}",context_id);
    SECoP_S_addPropertyString("description", "target temperature",context_id);
    SECoP_S_addCommand("stop",&funcCall,context_id);
    SECoP_S_addPropertyString("description", "stops and settings are not stored no resume",context_id);
    SECoP_S_nodeComplete(context_id);

    SECoP_S_showStatusWindow(true);





    QThread::sleep(10);


    SECoP_S_doneLibrary(true,context_id);
}




class context_testing : public QObject
{
    Q_OBJECT

public:
    context_testing();
    ~context_testing();

private slots:
    void initTestCase();

    void single_init();
};

context_testing::context_testing() {}

context_testing::~context_testing() {}

void context_testing::initTestCase(){
    qInstallMessageHandler(noMessageOutput);

};

void context_testing::single_init() {
    const char* context_id = "default";
    const char* context_id_N2 = "default_n2";


    const char* name_default = "Node1";
    const char* name_default_N2 = "Node2";

    unsigned int port_default    = 2055;
    unsigned int port_default_N2 = 2056;





    SECoP_S_setManyThreads(0);


    // Create QFutureWatcher
    QFutureWatcher<void> watcher;


    QFuture<void> future1 = QtConcurrent::run(&Node1, context_id, name_default, port_default);



    QFuture<void> future2 = QtConcurrent::run(&Node1, context_id_N2, name_default_N2, port_default_N2);


    watcher.setFuture(future1);
    watcher.setFuture(future2);

    // Wait for all futures to finish
    watcher.waitForFinished();

    /// SECoP_S_doneLibrary(true,context_id);



}




QTEST_APPLESS_MAIN(context_testing)

#include "servertest.moc"
