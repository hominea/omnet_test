/*
 * Sink.cc
 *
 *  Created on: 2020年11月23日
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
 *      1、sink节点获取node节点的坐标,通过index来实现.
 *      2、sink节点只管发送第一次，剩下都是node节点的事情.
 *      3、先尝试使用handleMessage，后用active来实现
 * */

#define TRANS_EVENT 10  //发送消息的标识
#define NEXT_EVENT 11   //下一个事件

#define DATA_MSG 20     //数据消息

#define TRANS_TIME 100  //发送间隔
//#define ROUND_TIME 100  //下一轮间隔
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
    //生成消息
    //virtual WsnMsg *generateMessage();
    void virtual initialize();
    void virtual handleMessage(cMessage* msg);
    void nextRound();   //下一跳周期.
    double countDistance(double xx,double yy);
    void transmitData();
};

Define_Module(Sink);

/**
 *      获取父模块，设置自己的门大小
 * */
void Sink::initialize()
{
    cModule* parent = this->getParentModule();
    this->setGateSize("in",parent->par("node_num"));
    nodeNum = parent->par("node_num");
    cModule* node = new cModule;   //这样写没问题，能够初始化成功.
    //需要获得门的连接.
    for(int i=0;i<nodeNum;i++){
        node = parent->getSubmodule("node",i);   //获取.ned中的参数
        cnodes.push_back(node);
        if (node!=nullptr){
            vector<double> l;
            l.push_back(node->par("x"));
            l.push_back(node->par("y"));
            nodes.push_back(l);
        }
    }

    //发送消息
    //wsn *msg = generateMessage();
    this->transDataMessage = new omnetpp::cMessage("conect_msg");
    //Sets the message kind
    this->transDataMessage->setKind(TRANS_EVENT);       //设置两条消息

    this->nextRoundMessage = new omnetpp::cMessage("nextRound");
    this->nextRoundMessage->setKind(NEXT_EVENT);

//    this->msg = new WsnMsg("Message");
//    this->msg->setKind(TRANS_EVENT);
    this->nextRound();
}

/**
 *  下一条消息，产生发送消息和周期消息.
 * */
void Sink::nextRound(){
    //scheduleAt(simTime()+uniform(0,TRANS_TIME),transDataMessage);
    scheduleAt(simTime(),transDataMessage);
    //发送下一轮的时间周期,simTime() + ROUND_TIME
    //scheduleAt(simTime()+ROUND_TIME,nextRoundMessage);
}

/**
 *      消息发送函数
 * */

double Sink::countDistance(double xx,double yy){
    double d = xx*xx + yy*yy;
    return sqrt(d);
}

void Sink::transmitData(){
    //先判断离自己的距离
    TestMsg* msg = NULL;
    char name[20];
    msg = new TestMsg("conect_data");
    //msg->setName("sink to");    //换个名字
    msg->setKind(CONECT_MSG);
    msg->setSrcProcId(1);       //一跳
    msg->setId(this->getIndex());
    //EV<<this->getIndex()<<endl;
    for(int i=0;i<nodeNum;i++){
        if (this->countDistance(nodes[i][0],nodes[i][1])<=100){
            //表示可以发送.
            //sprintf(name,"0 0 %d %d",int(nodes[i][0]),int(nodes[i][1]));
            //msg->setName(name);
            //建立连接.
            cGate* ingate = cnodes[i]->gate("in",i); //输入门,id是自己的id
            cGate* outgate = this->gate("out");     //自己的输出门
            outgate->connectTo(ingate); //连接到sink节点输入
            msg->setDistance(this->countDistance(nodes[i][0],nodes[i][1]));
            msg->setHopCount(1);
            //send(msg->dup(),"out");
            sendDelayed(msg->dup(),0.01,"out");
            if(outgate->isConnected()){
                              outgate->disconnect();  //断开连接
                        }
            EV<<"i is "<<i<<",x is "<<nodes[i][0]<<",y is "<<nodes[i][1]<<"\n";
//            break;
        }
    }
}

/**
 *  消息判断处理.
 * */
void Sink::handleMessage(cMessage* msg)
{
    //判断消息是否是scheduleAt发送的，这里肯定是
    if (msg->isSelfMessage()){
        switch(msg->getKind()){
            case TRANS_EVENT:
                //EV<<"no problems1111\n";
                this->transmitData();
                break;
            case NEXT_EVENT:
                this->nextRound(); //如果是下一个事件 （生成一个随机事件 + 下个事件）
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



