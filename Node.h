/*
 * Node.h
 *
 *  Created on: 2020��11��23��
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
 *      ÿ��node�ڵ���е�����
 * */
class Node : public cSimpleModule{

private:
    int back_id; //��һ�ڵ���
    double distance; //��sink�ľ���
    int hopCount = 0; //��sink������
    int dead;//�����ڵ���
    int msg_send;//��Ϣ������
    int msg_recive;//��Ϣ������
    int datasend;//���ݷ�����
    int datarecive;//�����յ���
    int datatrans;//����ת����

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
    void conectNode(TestMsg* msg);    //���ͻ�����ת����ͷ�ռ����ں�����
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
