#include "Node.h"

Node::Node(int id, int opinion, vector<int> received_opinion, vector<Node_Identity> neighbors, bool finalized, string is_malicious,vector<bool> v)
{
    Id = id;
    cnt=0;
    Opinion = opinion;
    ReceivedOpinion=received_opinion;
    Neighbors = neighbors;
    Finalized = finalized;
    Is_malicious = is_malicious;
    Is_lying=v;
}



// Shower
void Node::showOpinion(){
    EV << Opinion << endl;
}

void Node::showReceivedOpinion(){
    EV << "[";
    for(long unsigned int i = 0; i < ReceivedOpinion.size(); i++){
        EV << ReceivedOpinion[i]<< ", ";
    }
    EV << "]" << endl;
}

void Node::showNeighbors()
{
    EV << Id << " : [";
    for(long unsigned int i = 0; i < Neighbors.size(); i++){
        EV << Neighbors[i].Id << " : "<< Neighbors[i].Gate_Number << ", ";
    }
    EV << "]" << endl;
}


void Node::setFinalized()
{
  if(cnt >= FINALIZE){
    Finalized=true;
  }
}
