
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
#include <QTcpSocket>
#include <QHostAddress>
#include  "test_node.h"

#define CONTEXT_ID "default"
#define CONTEXT_ID_N2 "default_node2"

// Forward declaration of TabInfo struct
struct TabInfo;



void noMessageOutput(QtMsgType, const QMessageLogContext &, const QString &)
{

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


    QFuture<void> future1 = QtConcurrent::run(&Node, context_id, name_default, port_default,5);



    QFuture<void> future2 = QtConcurrent::run(&Node, context_id_N2, name_default_N2, port_default_N2,1);


    watcher.setFuture(future1);
    watcher.setFuture(future2);

    // Wait for all futures to finish
    watcher.waitForFinished();
    SECoP_S_showStatusWindow(true);
    QThread::sleep(2);


    SECoP_S_showStatusWindow(true);

    QThread::sleep(1);


}


TEST_F(Context_id_Test, GetCommad) {


    const char* context_id_1 = "id1";
    const char* context_id_2 = "id2";


    const char* node1 = "node1";
    const char* node2 = "node2";

    unsigned int port1 = 2055;
    unsigned int port2 = 2056;

    Node_no_wait( context_id_1, node1, port1,false);


    Node_no_wait(context_id_2, node2, port2,false);

    QTcpSocket socket55;
    QTcpSocket socket56;

    socket56.connectToHost(QHostAddress("127.0.0.1"), 2056); // Change IP and port as needed

    socket55.connectToHost(QHostAddress("127.0.0.1"), 2055); // Change IP and port as needed

    ASSERT_TRUE(socket55.waitForConnected(3000));
    ASSERT_TRUE(socket56.waitForConnected(3000));

    QByteArray command = "change hpd:target 500\n";

    // command to id2 is sent first
    socket56.write(command);
    socket55.write(command);


    ASSERT_TRUE(socket55.waitForBytesWritten(3000));
    ASSERT_TRUE(socket56.waitForBytesWritten(3000));


    unsigned long long pllId = 10;
    enum SECoP_S_action  Action = SECoP_S_ACTION_NONE ;
    char* szParameter = (char*) malloc(40);
    int piParameterSize = 40;

    CSECoPbaseType * ppValue = SECoP_V_fromJSON("{\"type\":\"double\",\"unit\":\"K\"}",nullptr);



    //### ContextID: id1 --> Node1
    while (Action != SECoP_S_ACTION_CHANGE) {
        pllId = 10;
        Action = SECoP_S_ACTION_NONE ;
        piParameterSize = 40;
        SECoP_S_getStoredCommand(&pllId,&Action,szParameter,&piParameterSize,&ppValue,context_id_1);
    }

    QString corr_string = QString("node1:hpd:target");

    ASSERT_TRUE(corr_string == szParameter);

    SECoP_S_putCommandAnswer(pllId,SECoP_S_error(NoError),ppValue,nullptr,0);

    socket55.waitForReadyRead(3000);

    QByteArray response = socket55.readAll();



    Action = SECoP_S_ACTION_NONE ;
    //### ContextID: id2 --> Node2
    while (Action != SECoP_S_ACTION_CHANGE) {
        pllId = 10;
        Action = SECoP_S_ACTION_NONE ;
        piParameterSize = 40;
        SECoP_S_getStoredCommand(&pllId,&Action,szParameter,&piParameterSize,&ppValue,context_id_2);
    }

    corr_string = QString("node2:hpd:target");

    ASSERT_TRUE(corr_string == szParameter);


    SECoP_S_putCommandAnswer(pllId,SECoP_S_error(NoError),ppValue,nullptr,0);

    socket55.waitForReadyRead(3000);

    response = socket55.readAll();


    socket55.disconnectFromHost();
    socket56.disconnectFromHost();



    SECoP_S_showStatusWindow(true);

    QThread::sleep(5);


}



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


    Node_no_wait( context_id_1, node1, port1,true);


    Node_no_wait(context_id_2, node2, port2,true);


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


    Node_no_wait( context_id_1, node1, port1,true);

    ASSERT_TRUE(Context_id_Test::gui_is_Visible());

    Node_no_wait(context_id_2, node2, port2,true);

    ASSERT_TRUE(Context_id_Test::gui_is_Visible());

    SECoP_S_doneLibrary(true,context_id_2);

    ASSERT_TRUE(Context_id_Test::gui_is_Visible());

    Node_no_wait(context_id_2, node2, port2,true);

    //delete Nodea
    SECoP_S_deleteNode(node1);

    ASSERT_TRUE(Context_id_Test::gui_is_Visible());

    SECoP_S_doneLibrary(true,context_id_2);


    ASSERT_TRUE(Context_id_Test::gui_is_Visible());

    SECoP_S_doneLibrary(true,context_id_1);

    ASSERT_FALSE(Context_id_Test::gui_is_Visible());
}
















