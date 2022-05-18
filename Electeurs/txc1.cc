#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class SimpleNode : public cSimpleModule
{
  private:
    int round;
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};
Define_Module(HonestNode);

class MaliciousNode : public HonestNode
{

};
Define_Module(MaliciousNode);

void SimpleNode::initialize()
{
    round = par("limit");
    WATCH(round);

    if (par("sendMsgOnInit").boolValue() == true) {
        EV << "Sending initial message\n";
        cMessage *msg = new cMessage("Hello");
        send(msg, "out");
    }
}

void SimpleNode::handleMessage(cMessage *msg)
{
    round--;
        if (round == 0) {
            EV << getName() << "'s round reached zero, deleting message\n";
            delete msg;
        }
        else {
            EV << getName() << "'s round is " << round << ", sending back message\n";
            send(msg, "out");
        }
}
