/*
 * networklayer.cc
 *
 *  Created on: 2017¦~5¤ë18¤é
 *      Author: USER
 */
#include <map>
#include <omnetpp.h>
#include "Message_m.h"
using namespace omnetpp;

class Networklayer: public cSimpleModule
{
    private:
        bool host;
        int num;
        int myaddress;
        int event_num;
        simtime_t iaTime;
    protected:
        virtual void finish();
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        void findTopo(cMessage *msg);
        void schedule_message();
};

Define_Module(Networklayer);

void Networklayer::initialize()
{
    event_num = getSystemModule()->par("event");
    num = getSystemModule()->par("node_num");
    if(strcmp("host",getParentModule()->getName())==0)
    {
        host = true;
    }
    else
    {
        host = false;
    }
    myaddress = getParentModule()->par("address");
    if(host)
    {
        schedule_message();
    }
}
void Networklayer::handleMessage(cMessage *msg)
{
    Message *ttmsg = check_and_cast<Message *>(msg);
    switch(ttmsg->getMes_type())
    {
        case 0:
            schedule_message();
            ttmsg->setMes_type(1);
            findTopo(ttmsg);
            break;
        case 1:
            findTopo(ttmsg);
            break;
        case 2:
            event_num = getSystemModule()->par("event");
            getSystemModule()->par("event") = ++event_num;
            delete ttmsg;
            break;
    }
    if(event_num==5)
        endSimulation();
}
void Networklayer::findTopo(cMessage *msg)
{
    Message *ttmsg = check_and_cast<Message *>(msg);
    cTopology *topo = new cTopology("topo");
    topo->extractByNedTypeName(cStringTokenizer("Host Switch").asVector());
    cModule *t1 = getModuleByPath(ttmsg->getDest_name());
    cTopology::Node *targetnode = topo->getNodeFor(t1);
    cTopology::Node *thisnode = topo->getNodeFor(getParentModule());
    topo->calculateUnweightedSingleShortestPathsTo(targetnode);
    cTopology::LinkOut *link = thisnode->getPath(0);
    cTopology::Node *neighbour = link->getRemoteNode();
    //EV << thisnode->getPath(0)->getLocalGate()->getFullName() << endl;
    //EV << neighbour->getModule()->getName() << endl;
    if(strcmp("switch",neighbour->getModule()->getName())==0 and ttmsg->getMes_type() != 0)
        ttmsg->setMes_type(1);
    else
        ttmsg->setMes_type(2);
    send(ttmsg,"out",link->getLocalGate()->getIndex());
}
void Networklayer::schedule_message()
{
    char buf[20];
    int b = myaddress;
    while(b == myaddress)
    {
        b = intuniform(0,num-3,0);
    }
    Message *msg = new Message();
    msg->setSource(myaddress);
    msg->setDest(b);
    msg->setMes_type(0);
    sprintf(buf,"host[%d]",b);
    msg->setDest_name(buf);
    iaTime = par("iaTime");
    scheduleAt(simTime()+iaTime,msg);
}

 void Networklayer::finish()
 {
     EV << "Finish!!";
 }
