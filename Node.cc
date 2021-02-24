/*
 * Node.cc
 *
 *  Created on: 2011-10-1
 *      Author: RenJu
 */
#include<omnetpp.h>
#include "Node.h"
#include <fstream>
using namespace std;
#include "test_m.h"


Define_Module(Node);

vector<Node*> Node::nodev;

Node::Node()
{
    conectDataMessage = nextRoundMessage =  sinkMessage = NULL;
}

Node::~Node()
{
    cancelAndDelete(conectDataMessage);
    cancelAndDelete(nextRoundMessage);
}

int Node::getx()
{
    return x;
}

int Node::gety()
{
    return y;
}

int Node::gethop()
{
    return hopCount;
}

double Node::getEnergy()
{
    return energy;
}

double Node::getdistance()
{
    return distance;
}

void Node::initialize()
{
    back_id = 0;
    distance = 999999;
    hopCount = 99;
    dead = 0;
    msg_send = 0;
    msg_recive = 0;
    datasend = 0;
    datarecive = 0;
    datatrans = 0;

    cModule* parent = this->getParentModule();
    energy = par("energy");
    x = par("x");
    y = par("y");
    senRange = par("senRange");

    fs = par("fs");
    eperbit = par("eperbit");
    amp = par("amp");
    thresholdDistance = par("thresholdDistance");


    conect_databits = parent->par("conect_databits");
    data_databits = parent->par("data_databits");
    node_num = parent->par("node_num");
    R = parent->par("R");

    sink = parent->getSubmodule("sink");

    flag = 0;
    this->isDead = false;
    this->currentRound = 0;

    this->setGateSize("in",this->node_num+200);    //多给2个位置，怕指针出错

    this->conectDataMessage = new TestMsg("conect_msg");
    this->conectDataMessage->setKind(CONECT_EVENT);
    this->nextRoundMessage = new TestMsg("nextRound");
    this->nextRoundMessage->setKind(NEXT_EVENT);
    this->sinkMessage = new TestMsg("sinkData");
    this->sinkMessage->setKind(SINK_EVENT);
    nodev.push_back(this);
    //scheduleAt(simTime()+3000,buildMessage);
    //nextRound();

}


void Node::nextRound()
{
    this->currentRound++;
    int i;
    if(!this->isDead)
    {

        if(nodev[this->back_id]->isDead)
            New_cnecet();//父节点死亡，此轮找下一父节点`
        else
        {
            //        for(i=0;i<20;i++){
            datatoSink();
            //        }


            //        scheduleAt(simTime(),sinkMessage);
            //     if (flag == 0){

            //        flag=1;
            //     }
        }
        scheduleAt(simTime()+ROUND_TIME,nextRoundMessage);
    }
}

//父节点死亡，找下一父节点
void Node::New_cnecet()
{
    for(int i=0;i<node_num;i++)
    {
        if (this->distance_sq_to_node(nodev[i])<=senRange &&
                !nodev[i]->isDead &&
                this->getdistance()>nodev[i]->getdistance())
        {
            this->hopCount=nodev[i]->hopCount+1;
            this->back_id=i;
            this->conectBack(this->back_id);
            break;
        }

    }
}


double Node::distance_sq_to_sink()
{
    double sx = sink->par("x");
    double sy = sink->par("y");
    double val = (x-sx)*(x-sx)+(sy-y)*(sy-y);
    return sqrt(val);
}

double Node::distance_sq_to_node(Node* nod)
{
    double chx = nod->getx();
    double chy = nod->gety();
    double dis = (x-chx)*(x-chx)+(y-chy)*(y-chy);
    return sqrt(dis);
}

bool Node::alive(double decEnergy)
{
    if( energy > 0 )
        return true;
    else
    {
        energy += decEnergy;
        this->getDisplayString().setTagArg("i",1,"white");
        this->isDead = true;
        //this->energy=0;
        this->dead=1;//标记死亡
        cGate* outgate = this->gate("out",1);
        if(outgate->isConnected()){
            outgate->disconnect();  //断开连接
        }
        end();//判断是否停止仿真


        //        EV << this->currentRound;
        //        endSimulation();
        return false;
    }
}

//判断时候结束仿真
void Node::end()
{
    int i;
    int avg=0;
    for(i=0; i<1000; i++)
    {
        avg=nodev[i]->dead + avg;
    }
    if(1==avg)//1节点死亡
    //if(10==avg)//%1节点死亡
    //if(50==avg)//%5节点死亡
    {
        EV << this->currentRound;
        endSimulation();

    }
}

/*
 * 接收数据时的能耗
 */
double Node::receiveMessageEnergyConsume(int bitnumber)
{
    double decEnergy = bitnumber * this->eperbit;
    energy -= decEnergy;
    return decEnergy;
}

/**
 * 发送数据的能耗
 */
double Node::sendDataEnergyConsume(double distance, int bitnumber)
{
    double decEnergy = 0;
    if(distance < this->thresholdDistance){
        decEnergy = (this->eperbit + fs*distance )*bitnumber;
    }else{
        decEnergy = (this->eperbit + amp*distance*distance )*bitnumber;
    }
    energy -= decEnergy;
    return decEnergy;
}

void Node::conectBack(int id)
{
    cGate* ingate = nodev[id]->gate("in",getIndex());
    cGate* outgate = this->gate("out",1);
    if(outgate->isConnected()){
        outgate->disconnect();  //断开连接
    }
    outgate->connectTo(ingate);
    this->showHope(this->hopCount);
    //    this->getDisplayString().setTagArg("i",1,"white");
    //    bubble("this->hopCount");
}

void Node::conectSink()
{
    //cGate* ingate = sink->gate("in",this->getId());
    cGate* ingate = sink->gate("in",getIndex());
    cGate* outgate = this->gate("out",1);
    if(outgate->isConnected()){
        outgate->disconnect();  //断开连接
    }
    outgate->connectTo(ingate);
    this->showHope(this->hopCount);
    //    getDisplayString().setTagArg("i", 1, "yellow");
    //    getDisplayString().setTagArg("t", 0, "TRANSMIT");
    //    bubble("this->hopCount");
}

void Node::showHope(int hop)
{
    switch(hop)
    {
    case 1:
        getDisplayString().setTagArg("t", 0, "1 hop");
        break;
    case 2:
        getDisplayString().setTagArg("t", 0, "2 hop");
        break;
    case 3:
        getDisplayString().setTagArg("t", 0, "3 hop");
        break;
    case 4:
        getDisplayString().setTagArg("t", 0, "4 hop");
        break;
    case 5:
        getDisplayString().setTagArg("t", 0, "5 hop");
        break;
    case 6:
        getDisplayString().setTagArg("t", 0, "6 hop");
        break;
    case 7:
        getDisplayString().setTagArg("t", 0, "7 hop");
        break;
    case 8:
        getDisplayString().setTagArg("t", 0, "8 hop");
        break;
    case 9:
        getDisplayString().setTagArg("t", 0, "9 hop");
        break;
    case 10:
        getDisplayString().setTagArg("t", 0, "10 hop");
        break;
    case 11:
        getDisplayString().setTagArg("t", 0, "11 hop");
        break;
    case 12:
        getDisplayString().setTagArg("t", 0, "12 hop");
        break;
    case 13:
        getDisplayString().setTagArg("t", 0, "13 hop");
        break;
    case 14:
        getDisplayString().setTagArg("t", 0, "14 hop");
        break;
    case 15:
        getDisplayString().setTagArg("t", 0, "15 hop");
        break;
    }
}

void Node::datatoSink()
{
    TestMsg* msg = NULL;
    double decEnergy = 0;
    decEnergy = this->sendDataEnergyConsume(this->distance_sq_to_node(nodev[this->back_id]), this->data_databits);//发送耗能
    if(this->alive(decEnergy))
    {

        TestMsg* msg = NULL;
        msg = new TestMsg("sink_data");
        msg->setKind(SINK_EVENT);
        msg->setId(this->getIndex());
        msg->setHopCount(hopCount);
        //send(msg,"out",1);
        sendDelayed(msg->dup(),0.01,"out",1);
        this->datasend++;
        //this->hopCount=1+nodev[this->back_id]->hopCount;
    }
    delete msg;

}

void Node::conectNode(TestMsg* msg)
{
    //先判断离自己的距离
    char name[20];
    double decEnergy = 0;
    msg = new TestMsg("conect_data");
    msg->setKind(CONECT_MSG);
    msg->setSrcProcId(1);       //一跳
    msg->setId(this->getIndex());


    for(int i=0;i<node_num;i++){
        /**
         *      ①在自己发送范围内
         *      ②不会回发
         *      ③除了一跳.
         * */
        if (this->distance_sq_to_node(nodev[i])<=senRange &&
                this->getdistance()<nodev[i]->getdistance()&&
                nodev[i]->distance_sq_to_sink()>=senRange &&
                this->hopCount<nodev[i]->hopCount){
            cGate* ingate = nodev[i]->gate("in",getIndex()); //输入门,id是自己的id
            cGate* outgate = this->gate("out",0);     //自己的输出门

            if(outgate->isConnected()){
                outgate->disconnect();  //断开连接
            }
            outgate->connectTo(ingate); //连接到sink节点输入
            msg->setDistance(this->distance_sq_to_node(nodev[i])+this->distance);
            msg->setHopCount(this->hopCount+1);
            decEnergy = this->sendDataEnergyConsume(this->distance_sq_to_node(nodev[i]), this->conect_databits);
            if(this->alive(decEnergy))
            {
                send(msg->dup(),"out",0);
            }
            this->msg_send++;
            if(outgate->isConnected()){
                outgate->disconnect();  //断开连接
            }
            //EV<<"i is "<<i<<",x is "<<nodev[i][0]<<",y is "<<nodev[i][1]<<"\n";
            //            break;
        }

    }
    delete msg;
    //现在连
    if(this->hopCount>1)
        this->conectBack(this->back_id);
    else
        this->conectSink();
    this->flag = 0;
    //连接完成开始发送数据到Sink
    scheduleAt(1,nextRoundMessage);
    //nextRound();
}

void Node::handleMessage(cMessage* msg)
{
    if(!this->isDead)
    {
        if( msg->isSelfMessage() )
        {
            TestMsg* data = NULL;
            switch(msg->getKind())
            {
            case CONECT_EVENT:

                delete msg;
                this->conectNode(data);
                delete data;
                break;
            case NEXT_EVENT:
                //nextRound();
                //outengy(this->currentRound);
                //delete msg;
                break;
            case BUILD_EVENT:
                if (this->back_id!=0)
                {
                    if(this->hopCount>1)
                        this->conectBack(this->back_id);
                    else
                        this->conectSink();
                }
                break;
            case SINK_EVENT:
                datatoSink();
                //delete msg;
                break;
            default:
                error("Invalid event:%s", msg->getName());
            }
        }
        else
        {
            double decEnergy = 0;
            TestMsg *ttmsg = check_and_cast<TestMsg *>(msg);
            switch(ttmsg->getKind())
            {
            case CONECT_MSG:
                this->msg_recive++;
                //如果距离近，则更新最近距离
                decEnergy = this->receiveMessageEnergyConsume(this->conect_databits);  //接收消息耗能
                if(this->alive(decEnergy))
                {
                    if(this->distance>ttmsg->getDistance())
                    {
                        this->hopCount = ttmsg->getHopCount();
                        this->back_id=ttmsg->getId();
                        this->distance = ttmsg->getDistance();
                        delete ttmsg;
                    }
                }
                if (flag == 0){
                    //收到多个后发送.
                    scheduleAt(simTime()+0.01 ,conectDataMessage);
                    flag = 1;
                }
                EV<<"Back_id "<<this->back_id<<"  Dis_to_sink "<<this->distance<<"  hops "<<this->hopCount<<"\n";
                break;
            case SINK_EVENT:
                this->datarecive++;
                decEnergy = this->receiveMessageEnergyConsume(this->data_databits);  //接收消息耗能
                EV<<msg->getArrivalTime()<<" "<<msg->getCreationTime()<<"\n";
                if(this->alive(decEnergy))
                {
                    decEnergy = this->sendDataEnergyConsume(this->distance_sq_to_node(nodev[this->back_id]), this->data_databits);//发送耗能
                    if(this->alive(decEnergy))
                    {
                        if(nodev[this->back_id]->isDead)
                        {
                            New_cnecet();
                        }
                        //send(msg,"out",1);
                        sendDelayed(msg->dup(),0.01,"out",1);
                        this->datatrans++;
                    }
                }
                break;
                default:
                    error("Invalid message:%s",ttmsg->getName());
            }//switch
        }//else
    }//if(!this->isDead)
}

void Node::outengy(int cRound)
{
    ofstream out("F:\\out3.txt",ios::app);
    if(out.is_open())
    {
        out<<cRound<<" "<<this->getEnergy()<<"\n";
        //out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getx()<<" "<<nodev[i]->gety()<<" "<<nodev[i]->getEnergy()<<"\n";
        out.close();
    }



    //    switch(cRound)
    //    {
    //    case 1:
    //    {ofstream out("F:\\out1.txt");if(out.is_open())
    //    {
    //        for(int i = 0; i < nodev.size(); i++)
    //        {
    //            out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getEnergy()<<"\n";
    //            //out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getx()<<" "<<nodev[i]->gety()<<" "<<nodev[i]->getEnergy()<<"\n";
    //        }
    //    out.close();
    //    }break;}
    //    case 2:
    //    {ofstream out("F:\\out2.txt");if(out.is_open())
    //    {
    //        for(int i = 0; i < nodev.size(); i++)
    //        {
    //            out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getEnergy()<<"\n";
    //            //out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getx()<<" "<<nodev[i]->gety()<<" "<<nodev[i]->getEnergy()<<"\n";
    //        }
    //    out.close();
    //    }break;}
    //    case 3:
    //    {ofstream out("F:\\out3.txt");if(out.is_open())
    //    {
    //        for(int i = 0; i < nodev.size(); i++)
    //        {
    //            out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getEnergy()<<"\n";
    //            //out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getx()<<" "<<nodev[i]->gety()<<" "<<nodev[i]->getEnergy()<<"\n";
    //        }
    //    out.close();
    //    }break;}
    //    case 4:
    //    {ofstream out("F:\\out4.txt");if(out.is_open())
    //    {
    //        for(int i = 0; i < nodev.size(); i++)
    //        {
    //            out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getEnergy()<<"\n";
    //            //out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getx()<<" "<<nodev[i]->gety()<<" "<<nodev[i]->getEnergy()<<"\n";
    //        }
    //    out.close();
    //    }break;}
    //    case 5:
    //    {ofstream out("F:\\out5.txt");if(out.is_open())
    //    {
    //        for(int i = 0; i < nodev.size(); i++)
    //        {
    //            out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getEnergy()<<"\n";
    //            //out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getx()<<" "<<nodev[i]->gety()<<" "<<nodev[i]->getEnergy()<<"\n";
    //        }
    //    out.close();
    //    }break;}
    //    case 6:
    //    {ofstream out("F:\\out6.txt");if(out.is_open())
    //    {
    //        for(int i = 0; i < nodev.size(); i++)
    //        {
    //            out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getEnergy()<<"\n";
    //            //out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getx()<<" "<<nodev[i]->gety()<<" "<<nodev[i]->getEnergy()<<"\n";
    //        }
    //    out.close();
    //    }break;}
    //    case 7:
    //    {ofstream out("F:\\out7.txt");if(out.is_open())
    //    {
    //        for(int i = 0; i < nodev.size(); i++)
    //        {
    //            out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getEnergy()<<"\n";
    //            //out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getx()<<" "<<nodev[i]->gety()<<" "<<nodev[i]->getEnergy()<<"\n";
    //        }
    //    out.close();
    //    }break;}
    //    case 8:
    //    {ofstream out("F:\\out8.txt");if(out.is_open())
    //    {
    //        for(int i = 0; i < nodev.size(); i++)
    //        {
    //            out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getEnergy()<<"\n";
    //            //out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getx()<<" "<<nodev[i]->gety()<<" "<<nodev[i]->getEnergy()<<"\n";
    //        }
    //    out.close();
    //    }break;}
    //    case 9:
    //    {ofstream out("F:\\out9.txt");if(out.is_open())
    //    {
    //        for(int i = 0; i < nodev.size(); i++)
    //        {
    //            out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getEnergy()<<"\n";
    //            //out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getx()<<" "<<nodev[i]->gety()<<" "<<nodev[i]->getEnergy()<<"\n";
    //        }
    //    out.close();
    //    }break;}
    //    default:
    //    {
    //        ofstream out("F:\\out-n.txt");
    //        if(out.is_open())
    //                {
    //                    for(int i = 0; i < nodev.size(); i++)
    //                    {
    //                        //out<<nodev[i]->getEnergy()<<"\n"
    //                        out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getEnergy()<<"\n";
    //                        //out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getx()<<" "<<nodev[i]->gety()<<" "<<nodev[i]->getEnergy()<<"\n";
    //                    }
    //                out.close();
    //                }
    //    }
    //    }

}

void Node::finish(){
    ofstream out("F:\\end.txt");

    if(out.is_open())
    {
//        out<<"到sink的距离"<<" "
//               <<"跳数"<<" "
//               <<"发送消息"<<" "
//               <<"接受消息"<<" "
//               <<"发送数据"<<" "
//               <<"接受数据"<<" "
//               <<"转发数据"<<" "
//               <<"剩余能量"<<" "
//               <<"消耗能量"<<"\n";
        for(int i = 0; i < nodev.size(); i++){
            //out<<nodev[i]->getIndex()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getx()<<" "<<nodev[i]->gety()<<" "<<nodev[i]->getEnergy()<<"\n";
            //out<<nodev[i]->distance_sq_to_sink()<<" "<<nodev[i]->gethop()<<" "<<nodev[i]->getEnergy()<<"\n";
            out<<nodev[i]->distance_sq_to_sink()<<" "
                    <<nodev[i]->gethop()<<" "
                    <<nodev[i]->msg_send<<" "
                    <<nodev[i]->msg_recive<<" "
                    <<nodev[i]->datasend<<" "
                    <<nodev[i]->datarecive<<" "
                    <<nodev[i]->datatrans<<" "
                    <<nodev[i]->getEnergy()<<" "
                    <<500000000-nodev[i]->getEnergy()<<"\n";
        }
        out.close();
    }
}
