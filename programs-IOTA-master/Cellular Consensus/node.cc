#include "node.h"

Node::Node(int id, vector<int> opinion, vector<tuple<int, int>*> neighbors, tuple<vector<int>, vector<int>> blacklist, vector<map<int, tuple<int, int, map<int, tuple<int, int>>>>> received_opinion, bool finalized, vector<int> weight, vector<vector<bool>> send, string is_malicious, int count, int round)
{
    Id = id;
    Opinion = opinion;
    Neighbors = neighbors;
    Blacklist = blacklist;
    Received_opinion = received_opinion;
    Finalized = finalized;
    Weight = weight;
    Send = send;
    Is_malicious = is_malicious;
    is_lying=false;
    Count = count;
    Round = round;
}

// Getter
int Node::getId()
{
    return Id;
}
vector<int> Node::getOpinion()
{
    return Opinion;
}
vector<tuple<int, int>*> Node::getNeighbors()
{
    return Neighbors;
}
tuple<vector<int>, vector<int>> Node::getBlacklist()
{
    return Blacklist;
}
vector<map<int, tuple<int, int, map<int, tuple<int, int>>>>> Node::getReceived_opinion()
{
    return Received_opinion;
}
bool Node::getFinalized()
{
    return Finalized;
}
vector<int> Node::getWeight()
{
    return Weight;
}
vector<vector<bool>> Node::getSend()
{
    return Send;
}
string Node::getIs_malicious()
{
    return Is_malicious;
}

int Node::getCount()
{
    return Count;
}

int Node::getRound()
{
    return Round;
}

// Setter
void Node::setWeight(int weight, int t)
{
    Weight[t]=weight;
}
void Node::setFinalized(bool finalized)
{
    Finalized=finalized;
}

// Adder
void Node::addNeighbors(tuple<int, int>* Node_to_add){
    Neighbors.push_back(Node_to_add);
}
void Node::addBlacklist(tuple<int, int>* Node_to_add){
    get<0>(Blacklist).push_back(get<0>(*Node_to_add));
    get<1>(Blacklist).push_back(get<1>(*Node_to_add));
}
void Node::addOpinion(int opinion_to_add){
    Opinion.push_back(opinion_to_add);
}
void Node::addWeight(int weight_to_add){
    Weight.push_back(weight_to_add);
}

void Node::addNeigh_to_Receiv(int Node_to_add, int Node_opinion, int Node_weight)
{
    map<int, tuple<int, int>> m1 = {};
    tuple<int, int, map<int, tuple<int, int>>> t2 = {Node_opinion, Node_weight, m1};
    int size = Received_opinion.size();
    Received_opinion[size-1][Node_to_add] = t2;
}

void Node::addNeigh_of_Neigh_to_Receiv(int Node_where_add, int Node_to_add, int Node_opinion, int Node_weight)
{
    int size = Received_opinion.size();
    get<2>(Received_opinion[size-1][Node_where_add])[Node_to_add] = {Node_opinion, Node_weight};
}

// Shower
void Node::showOpinion(){
    EV << "[";
    for(long unsigned int i = 0; i < Opinion.size(); i++){
        EV << Opinion[i] << ", ";
    }
    EV << "]" << endl;
}

void Node::showNeighbors()
{
    EV << Id << " : [";
    for(long unsigned int i = 0; i < Neighbors.size(); i++){
        if(Neighbors[i] == nullptr)
            EV << "(null); ";
        else
            EV << "(" << get<0>(*(Neighbors[i])) << ", " << get<1>(*(Neighbors[i])) << "); ";
    }
    EV << "]" << endl;
}

void Node::showBlacklist()
{
    EV << Id << " : [";
    for(long unsigned int i = 0; i < get<0>(Blacklist).size(); i++){
        EV << get<0>(Blacklist)[i] << ", ";
    }
    EV << "]" << endl;
}

void Node::showMap_node_tuple_int(map<int, tuple<int, int>> m)
{
    EV << "{";
    map<int, tuple<int, int>>::iterator it;
    for(it = m.begin(); it!=m.end(); it++){
        EV << get<0>(*it) << ": (" << get<0>(get<1>(*it)) << "," << get<1>(get<1>(*it)) << "); ";
    }
    EV << "}";
}

void Node::showTuple(tuple<int, int, map<int, tuple<int, int>>> t)
{
    EV << "(" << get<0>(t) << "," << get<1>(t)<< ",";
    showMap_node_tuple_int(get<2>(t));
    EV << ")";
}

void Node::showMap_node_tuple(map<int, tuple<int, int, map<int, tuple<int, int>>>> m)
{
    EV << "{";
    map<int, tuple<int, int, map<int, tuple<int, int>>>>::iterator it;
    for(it = m.begin(); it!=m.end(); it++){
        EV << get<0>(*it) << ": ";
        showTuple(get<1>(*it));
        EV << "; ";
    }
    EV << "}";
}

void Node::showReceived_opinion()
{
    EV << Id << " : [";
    for(long unsigned int i = 0; i < Received_opinion.size(); i++)
    {
        showMap_node_tuple(Received_opinion[i]);
        EV << ",   ";
    }
    EV << "]" << endl;
}


bool Node::opinion_changed()
{
  if( (int) Opinion.size() < L){
    return true;
  }
  for(long unsigned int i=Opinion.size()-L;i<Opinion.size()-1;i++){
    if(Opinion[i] != Opinion[i+1]){
        return true;
    }
  }
  return false;
}
