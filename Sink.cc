/*
 * Sink.cc
 *
 *  Created on: 2020��11��23��
 *      Author: 54391
 */

/*
 * Sink.cc
 *
 *  Created on: 2011-5-16
 *      Author: RenJu
 */

#include <omnetpp.h>
#include "Node.h"
#include <fstream>
#include <string.h>
#include<vector>
#include"test_m.h"
using namespace std;

/**
 *      1��sink�ڵ��ȡnode�ڵ������,ͨ��index��ʵ��.
 *      2��sink�ڵ�ֻ�ܷ��͵�һ�Σ�ʣ�¶���node�ڵ������.
 *      3���ȳ���ʹ��handleMessage������active��ʵ��
 * */

#define TRANS_EVENT 10  //������Ϣ�ı�ʶ
#define NEXT_EVENT 11   //��һ���¼�

#define DATA_MSG 20     //������Ϣ

#define TRANS_TIME 100  //���ͼ��
//#define ROUND_TIME 100  //��һ�ּ��
class Sink:public cSimpleModule
{
private:
    ofstream out;
    int nodeNum;
    vector<vector<double>>nodes;
    vector<cModule*> cnodes;

    cMessage* transDataMessage;
    cMessage* nextRoundMessage;
   // WsnMsg* msg;

protected:
    //������Ϣ
    //virtual WsnMsg *generateMessage();
    void virtual initialize();
    void virtual handleMessage(cMessage* msg);
    void nextRound();   //��һ������.
    double countDistance(double xx,double yy);
    void transmitData();
};

Define_Module(Sink);

/**
 *      ��ȡ��ģ�飬�����Լ����Ŵ�С
 * */
void Sink::initialize()
{
    cModule* parent = this->getParentModule();
    this->setGateSize("in",parent->par("node_num"));
    nodeNum = parent->par("node_num");
    cModule* node = new cModule;   //����дû���⣬�ܹ���ʼ���ɹ�.
    //��Ҫ����ŵ�����.
    for(int i=0;i<nodeNum;i++){
        node = parent->getSubmodule("node",i);   //��ȡ.ned�еĲ���
        cnodes.push_back(node);
        if (node!=nullptr){
            vector<double> l;
            l.push_back(node->par("x"));
            l.push_back(node->par("y"));
            nodes.push_back(l);
        }
    }

    //������Ϣ
    //wsn *msg = generateMessage();
    this->transDataMessage = new omnetpp::cMessage("conect_msg");
    //Sets the message kind
    this->transDataMessage->setKind(TRANS_EVENT);       //����������Ϣ

    this->nextRoundMessage = new omnetpp::cMessage("nextRound");
    this->nextRoundMessage->setKind(NEXT_EVENT);

//    this->msg = new WsnMsg("Message");
//    this->msg->setKind(TRANS_EVENT);
    this->nextRound();
}

/**
 *  ��һ����Ϣ������������Ϣ��������Ϣ.
 * */
void Sink::nextRound(){
    //scheduleAt(simTime()+uniform(0,TRANS_TIME),transDataMessage);
    scheduleAt(simTime(),transDataMessage);
    //������һ�ֵ�ʱ������,simTime() + ROUND_TIME
    //scheduleAt(simTime()+ROUND_TIME,nextRoundMessage);
}

/**
 *      ��Ϣ���ͺ���
 * */

double Sink::countDistance(double xx,double yy){
    double d = xx*xx + yy*yy;
    return sqrt(d);
}

void Sink::transmitData(){
    //���ж����Լ��ľ���
    TestMsg* msg = NULL;
    char name[20];
    msg = new TestMsg("conect_data");
    //msg->setName("sink to");    //��������
    msg->setKind(CONECT_MSG);
    msg->setSrcProcId(1);       //һ��
    msg->setId(this->getIndex());
    //EV<<this->getIndex()<<endl;
    for(int i=0;i<nodeNum;i++){
        if (this->countDistance(nodes[i][0],nodes[i][1])<=100){
            //��ʾ���Է���.
            //sprintf(name,"0 0 %d %d",int(nodes[i][0]),int(nodes[i][1]));
            //msg->setName(name);
            //��������.
            cGate* ingate = cnodes[i]->gate("in",i); //������,id���Լ���id
            cGate* outgate = this->gate("out");     //�Լ��������
            outgate->connectTo(ingate); //���ӵ�sink�ڵ�����
            msg->setDistance(this->countDistance(nodes[i][0],nodes[i][1]));
            msg->setHopCount(1);
            //send(msg->dup(),"out");
            sendDelayed(msg->dup(),0.01,"out");
            if(outgate->isConnected()){
                              outgate->disconnect();  //�Ͽ�����
                        }
            EV<<"i is "<<i<<",x is "<<nodes[i][0]<<",y is "<<nodes[i][1]<<"\n";
//            break;
        }
    }
}

/**
 *  ��Ϣ�жϴ���.
 * */
void Sink::handleMessage(cMessage* msg)
{
    //�ж���Ϣ�Ƿ���scheduleAt���͵ģ�����϶���
    if (msg->isSelfMessage()){
        switch(msg->getKind()){
            case TRANS_EVENT:
                //EV<<"no problems1111\n";
                this->transmitData();
                break;
            case NEXT_EVENT:
                this->nextRound(); //�������һ���¼� ������һ������¼� + �¸��¼���
                break;
        }
    }
    else
    {
        TestMsg *ttmsg = check_and_cast<TestMsg *>(msg);
        switch(msg->getKind())
        {
        case SINK_EVENT:
            simtime_t eed = ttmsg->getArrivalTime() - ttmsg->getCreationTime();
            EV<<ttmsg->getId()<<"'s data is arrived"<<endl;
            ofstream out("F:\\time.txt",ios::app);
            if(out.is_open())
            {
                out<<eed<<"\n";
                //out<<msg->getArrivalTime()<<" "<<msg->getCreationTime()<<"\n";
                //out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getx()<<" "<<nodev[i]->gety()<<" "<<nodev[i]->getEnergy()<<"\n";
                out.close();
            }
            delete msg;
            break;
        }
    }

    //delete msg;
}



