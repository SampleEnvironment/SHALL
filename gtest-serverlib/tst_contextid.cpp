
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "SECoP-Main.h"
#include "SECoP-Node.h"

#include "SECoP-StatusGui.h"

//using namespace testing;


#include "SECoP.h"


#include <QRandomGenerator>
#include <QThread>
#include <QFuture>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QWidget>
#include <QList>
#include <QMap>

#define CONTEXT_ID "default"
#define CONTEXT_ID_N2 "default_node2"

// Forward declaration of TabInfo struct
struct TabInfo;



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





void Node(const char* context_id,const char* Node_id,unsigned short port,int sleep_time){

    SECoP_S_initLibrary(nullptr, true, true,context_id);

    SECoP_S_setManyThreads(0);

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





    QThread::sleep(sleep_time);


    SECoP_S_doneLibrary(true,context_id);


    QThread::sleep(2);
}



void Node_no_wait(const char* context_id,const char* Node_id,unsigned short port){

    SECoP_S_initLibrary(nullptr, true, true,context_id);


    SECoP_S_setManyThreads(0);

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




}






class Context_id_Test : public ::testing::Test {
protected:
    void SetUp() override {
        // Code here will be called immediately after the constructor (right before each test).
        qInstallMessageHandler(noMessageOutput);
    }

    void TearDown() override {
        QThread::sleep(1);
        SECoP_S_doneLibrary(false,"default");
        QThread::sleep(1);
    }



    SECoP_S_StatusGui* get_statusGui(){
        return SECoP_S_Main::getInstance()->m_pGui;
    }

    int get_TabListSize(){
        SECoP_S_StatusGui* p_gui = get_statusGui();
        return p_gui->m_aTabs.size();
    }

    SECoP_S_Node* get_NodeAtIdxTabList(int i){
        SECoP_S_StatusGui* p_gui = get_statusGui();
        return p_gui->m_aTabs[i].m_pNode;
    }

    QList<SECoP_S_Node*> get_NodeList(){
        return SECoP_S_Main::getInstance()->m_apNodes;
    }

    QMap<QString, SECoP_S_Node*> get_ContextIDMap(){
        return SECoP_S_Main::getInstance()->m_ContextIdMap;
    }

    int nodePosition(QString szNode){
        return SECoP_S_Main::getInstance()->nodePosition(szNode);
    }

    SECoP_S_Node* getLastNode(QString szContextID){
        return SECoP_S_Main::getInstance()->getLastNode(szContextID);
    }

    bool gui_is_Visible(){
        return get_statusGui()->m_bShowGUI;
    }



};




// void context_testing::initTestCase(){
//     //qInstallMessageHandler(noMessageOutput);

// };


TEST_F(Context_id_Test, delete_node) {

    const char* context_id_1 = "delete_one_node1";
    const char* context_id_2 = "delete_one_node2";


    const char* node1 = "Node1";
    const char* node2 = "Node2";

    unsigned int port1    = 2057;
    unsigned int port2 = 2058;


    Node_no_wait( context_id_1, node1, port1);


    Node_no_wait(context_id_2, node2, port2);


    // both nodes initialized
    ASSERT_EQ(Context_id_Test::get_NodeList().size(),2);



    //delete Nodea
    SECoP_S_deleteNode(node1);


    ASSERT_EQ(Context_id_Test::get_NodeList().size(),1);
    SECoP_S_Node * last_node = Context_id_Test::get_NodeList().at(0);
    ASSERT_TRUE(last_node->getNodeID() == node2);

    ASSERT_EQ(get_TabListSize(),1);


}


TEST_F(Context_id_Test, gui_state) {

    const char* context_id_1 = "delete_one_node1";
    const char* context_id_2 = "delete_one_node2";


    const char* node1 = "Node1";
    const char* node2 = "Node2";

    unsigned int port1    = 2057;
    unsigned int port2 = 2058;


    Node_no_wait( context_id_1, node1, port1);

    ASSERT_TRUE(Context_id_Test::gui_is_Visible());

    Node_no_wait(context_id_2, node2, port2);

    ASSERT_TRUE(Context_id_Test::gui_is_Visible());

    SECoP_S_doneLibrary(true,context_id_2);

    ASSERT_TRUE(Context_id_Test::gui_is_Visible());

    Node_no_wait(context_id_2, node2, port2);

    //delete Nodea
    SECoP_S_deleteNode(node1);

    ASSERT_TRUE(Context_id_Test::gui_is_Visible());

    SECoP_S_doneLibrary(true,context_id_2);


    ASSERT_TRUE(Context_id_Test::gui_is_Visible());

    SECoP_S_doneLibrary(true,context_id_1);

    ASSERT_FALSE(Context_id_Test::gui_is_Visible());
}


TEST_F(Context_id_Test, concurrent) {


    const char* context_id = "default";
    const char* context_id_N2 = "default_n2";


    const char* name_default = "Node1";
    const char* name_default_N2 = "Node2";

    unsigned int port_default    = 2055;
    unsigned int port_default_N2 = 2056;

    SECoP_S_initLibrary(nullptr, true, true,"null_id");



    // Create QFutureWatcher
    QFutureWatcher<void> watcher;


    QFuture<void> future1 = QtConcurrent::run(&Node, context_id, name_default, port_default,10);



    QFuture<void> future2 = QtConcurrent::run(&Node, context_id_N2, name_default_N2, port_default_N2,5);


    watcher.setFuture(future1);
    watcher.setFuture(future2);

    // Wait for all futures to finish
    watcher.waitForFinished();
    SECoP_S_showStatusWindow(true);
    QThread::sleep(2);


    SECoP_S_showStatusWindow(true);

    QThread::sleep(5);


}












