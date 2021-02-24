/*
 * Node.h
 *
 *  Created on: 2020年11月23日
 *      Author: 54391
 */

#ifndef NODE_H_
#define NODE_H_

#include<omnetpp.h>
#include<vector>
#include"test_m.h"


using namespace std;
using namespace omnetpp;

#define CONECT_EVENT 10
#define NEXT_EVENT 11
#define SINK_EVENT 12
#define BUILD_EVENT 13

#define CONECT_MSG 20

#define TRANS_TIME 100
#define ROUND_TIME 10

/**
 *      每个node节点固有的属性
 * */
class Node : public cSimpleModule{

private:
    int back_id; //上一节点标号
    double distance; //到sink的距离
    int hopCount = 0; //到sink的跳数
    int dead;//死亡节点数
    int msg_send;//消息发送量
    int msg_recive;//消息接受量
    int datasend;//数据发送量
    int datarecive;//数据收到量
    int datatrans;//数据转发量

    double energy;
    double R;
    double x;
    double y;
    double senRange;

    double eperbit;
    double fs;
    double amp;
    double thresholdDistance;

    int conect_databits;
    int data_databits;
    int node_num;

    bool isDead;
    int currentRound;

    int flag;
    cModule* sink;

    TestMsg* dataMessage;
    TestMsg* conectDataMessage;
    TestMsg* nextRoundMessage;
    TestMsg* sinkMessage;
    static vector<Node*> nodev;
    void nextRound();
    void conectNode(TestMsg* msg);    //传送或者是转发簇头收集的融合数据
    double distance_sq_to_sink();
    double distance_sq_to_node(Node* nod);
    double sendDataEnergyConsume(double distance,int bitnumber);
    double receiveMessageEnergyConsume(int bitnumber);
    void conectBack(int id);
    void conectSink();
    void showHope(int hop);
    void datatoSink();
    void outengy(int cRound);
    void end();
    void New_cnecet();

public:
    Node();
    ~Node();
    int getx();
    int gety();
    int gethop();
    double getdistance();
    double getEnergy();
    bool alive(double decEnergy);

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);
    virtual void finish();

};

#endif /* NODE_H_ */
