#ifndef TEST_NODE_H
#define TEST_NODE_H

#include "SECoP.h"
void Node(const char* context_id,const char* Node_id,unsigned short port,int sleep_time);

void Node_no_wait(const char* context_id,const char* Node_id,unsigned short port,bool enablefunctionpointers);


#endif // TEST_NODE_H
