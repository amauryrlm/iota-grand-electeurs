enum MessageType {Init, Received_Opinion, Time, Watts};

#include "node.h"

ofstream LOG_SIM;

typedef struct MsgOpinion
{
   int node;
   int op;
   int wt;
}MsgOpinion;


typedef struct MsgNeighOpinion
{
   int node;
   int neighbors;
   int op;
   int wt;
}MsgNeighOpinion;

typedef struct MsgReceivedOpinion
{
    MsgOpinion* my_op;
    vector<MsgNeighOpinion*>* my_neighbors_op;
}MsgReceivedOpinion;


class Node_Module : public cSimpleModule
{
    public:
        vector<int> Neighbors;
        bool Ready = false;
        void heartbeat(int m);
        int majority_opinion(int m);
        int majority_opinion_test(tuple<int, int>* neighbors, int m);
        bool is_inconsistent(tuple<int, int>* neighbors, int m);
        bool is_finalized(tuple<int, int>* neighbors);
        void cellular_consensus();
        string initialize_type_node();
        void simulation_prob();
        void simulation_nb_nodes();
        void simulation_nb_malicious();
        void watts_strogatz(int n, int k, double p);
    private:
        Node* node;
        int issueCount;
    protected:
        // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Node_Module);






int opposite_opinion(int op, double prob)
{
    float random1 = rand()/(float)RAND_MAX;
    if(random1 > prob)
        return op;
    float random2 = rand()/(float)RAND_MAX;
    if(op == 0)
    {
        if(random2 <= 0.5)
            return 1;
        return -1;
    }
    else if(op == 1)
    {
        if(random2 <= 0.5)
            return 0;
        return -1;
    }
    else
    {
        if(random2 <= 0.5)
            return 0;
        return 1;
    }
}








void Node_Module::heartbeat(int m)
{
    int op = node->Opinion[m];
    int wt = node->Weight[m];

    for(int i = 0; i < gateSize("exchanges"); i++)
    {
        node->Send[m][i] = false;
        double silence=rand()/(float)RAND_MAX;
        if(node->Is_malicious != "semi_cautious" || silence > SILENCE_PROBA)
        {
            node->Send[m][i] = true;
            MsgOpinion* my_op = new MsgOpinion;
            vector<MsgNeighOpinion*>* my_neighbors_op = new vector<MsgNeighOpinion*>;
            MsgReceivedOpinion* receiv_op = new MsgReceivedOpinion;
            if(node->Is_malicious == "berserk")
                my_op->op = opposite_opinion(op, LYING_PROBA);
            else
                my_op->op = op;
            my_op->node = node->Id;
            my_op->wt = wt;
            receiv_op->my_op = my_op;
            receiv_op->my_neighbors_op = my_neighbors_op;

            map<int, tuple<int, int, map<int, tuple<int, int>>>>::iterator it;
            for(it = node->Received_opinion[m-1].begin(); it!=node->Received_opinion[m-1].end(); it++)
            {
                if(get<0>(*(node->Neighbors[i])) != get<0>(*it))
                {
                    MsgNeighOpinion* u = new MsgNeighOpinion;
                    u->node = node->Id;
                    u->neighbors = get<0>(*it);
                    u->op = get<0>(get<1>(*it));
                    u->wt = get<1>(get<1>(*it));
                    receiv_op->my_neighbors_op->push_back(u);
                }
            }

            cMessage *msg_receiv_op = new cMessage("Received_Opinion_Msg", Received_Opinion);
            msg_receiv_op->setContextPointer(receiv_op);
            send(msg_receiv_op, "exchanges$o", i);
        }
    }
}









int Node_Module::majority_opinion(int m)
{
  int total = 0, weight0 = 0, weight1 = 0;
  map<int, tuple<int, int, map<int, tuple<int, int>>>>::iterator it;
  for(it = node->Received_opinion[m-1].begin(); it!=node->Received_opinion[m-1].end(); it++)
  {
    if (get<0>(get<1>(*it)) != -1)
    {
      if(get<0>(get<1>(*it)) == 0)
      {
          weight0 += get<1>(get<1>(*it));
      }
      else if(get<0>(get<1>(*it)) == 1)
      {
          weight1 += get<1>(get<1>(*it));
      }
      total += get<1>(get<1>(*it));
    }
  }
  double prob = rand() / (double) RAND_MAX;
  if (weight0 > total/2)
  {
    if (node->Is_malicious != "" && node->Is_malicious != "berserk")
    {
      if(node->Is_malicious == "cautious" && prob <= LYING_PROBA)
      {
        if (rand() / (double) RAND_MAX <= 0.5)
          return 1;
        return -1;
      }
    }
    return 0;
  }
  else if (weight1 > total/2)
  {
    if (node->Is_malicious != "" && node->Is_malicious != "berserk")
    {
      if (node->Is_malicious == "cautious" && prob <= LYING_PROBA)
      {
        if (rand() / (double) RAND_MAX <= 0.5)
          return 0;
        return -1;
      }
    }
    return 1;
  }
  else
  {
    if (node->Is_malicious != "" && node->Is_malicious != "berserk")
    {
      if (node->Is_malicious == "cautious" && prob <= LYING_PROBA)
      {
        if (rand() / (double) RAND_MAX <= 0.5)
          return 0;
        return 1;
      }
    }
    return -1;
  }
}









int Node_Module::majority_opinion_test(tuple<int, int>* neighbors, int m)
{
    int total = 0, weight0 = 0, weight1 = 0;
    map<int, tuple<int, int>> mapp = get<2>(node->Received_opinion[m][get<0>(*(neighbors))]);
    if(node->Opinion[m-1] != -1 && node->Send[m-1][get<1>(*(neighbors))] == true)
    {
      if(node->Opinion[m-1] == 0)
      {
        weight0 += node->Weight[m-1];
      }
      else if(node->Opinion[m-1] == 1)
      {
        weight1 += node->Weight[m-1];
      }
      total += node->Weight[m-1];
    }
    map<int, tuple<int, int>>::iterator it;
    for(it = mapp.begin(); it!=mapp.end(); it++)
    {
      if (get<0>(get<1>(*it)) != -1)
      {
        if(get<0>(get<1>(*it)) == 0)
        {
          weight0 += get<1>(get<1>(*it));
        }
        else if(get<0>(get<1>(*it)) == 1)
        {
          weight1 += get<1>(get<1>(*it));
        }
        total += get<1>(get<1>(*it));
      }
    }
    if(weight0 > total/2)
      return 0;
    else if(weight1 > total/2)
      return 1;
    else
      return -1;
}










bool Node_Module::is_inconsistent(tuple<int, int>* neighbors, int m)
{
  if(majority_opinion_test(neighbors,m-1) == get<0>(node->Received_opinion[m-1][get<0>(*(neighbors))]))
    return false;
  return true;
}








bool Node_Module::is_finalized(tuple<int, int>* neighbors)
{
    if( (int) node->Received_opinion.size() < L)
    {
        return false;
    }
    for(long unsigned int i=node->Received_opinion.size()-L-1;i<node->Received_opinion.size()-1;i++)
    {
        if(get<0>(node->Received_opinion[i][get<0>(*(neighbors))]) != get<0>(node->Received_opinion[i+1][get<0>(*(neighbors))]))
        {
          return false;
        }
    }
    return true;
}




void Node_Module::cellular_consensus()
{
    node->Count = 0;
    map<int, tuple<int, int, map<int, tuple<int, int>>>> r;
    node->Received_opinion.push_back(r);
    node->Weight.push_back(node->Weight[node->Round-1]);
    node->Send.push_back(node->Send[node->Round-1]);
    for(int i = 0; i < node->Neighbors.size(); i++)
    {
        map<int, tuple<int, int, map<int, tuple<int, int>>>>::iterator it = node->Received_opinion[node->Round-1].find(get<0>(*(node->Neighbors[i])));
        if(node->Round > 1 && it != node->Received_opinion[node->Round-1].end() && find(get<0>(node->Blacklist).begin(), get<0>(node->Blacklist).end(), get<0>(*(node->Neighbors[i]))) == get<0>(node->Blacklist).end() && is_finalized(node->Neighbors[i]) == false && is_inconsistent(node->Neighbors[i], node->Round) == true)
        {
            node->Received_opinion[node->Round-1].erase(it);
            node->addBlacklist(node->Neighbors[i]);
            cDisplayString& connDispStr = gate("exchanges$o", get<1>(*(node->Neighbors[i])))->getDisplayString();
            connDispStr.parse("ls=red,3");
            node->Weight[node->Round]--;
        }
    }
    if(node->Finalized == true)
        node->Opinion.push_back(node->Opinion[node->Round-1]);
    else
        node->Opinion.push_back(majority_opinion(node->Round));

    if(node->Is_malicious == "")
    {
        if(node->Opinion[node->Round] == 0)
            getDisplayString().setTagArg("i", 1, "red");
        else if(node->Opinion[node->Round] == 1)
            getDisplayString().setTagArg("i", 1, "cyan");
        else if(node->Opinion[node->Round] == -1)
            getDisplayString().setTagArg("i", 1, "black");
    }
    else
    {
        if(node->Opinion[node->Round] == 0)
            getDisplayString().setTagArg("i", 1, "orange");
        else if(node->Opinion[node->Round] == 1)
            getDisplayString().setTagArg("i", 1, "blue");
        else if(node->Opinion[node->Round] == -1)
            getDisplayString().setTagArg("i", 1, "purple");
    }
    if(node->Round != ROUNDS)
        heartbeat(node->Round);
    if(node->Opinion[node->Round] != -1 && node->opinion_changed() == false)
        node->Finalized = true;
    cMessage *msg_time = new cMessage("Time_Msg", Time);
    scheduleAt(node->Round+2,msg_time);
}






string Node_Module::initialize_type_node()
{
    float malicious = rand()/(float)RAND_MAX;
    double Prob_malicious = getParentModule()->par("PROB_MALICIOUS");
    string is_malicious;
    if (malicious <= Prob_malicious)
    {
        float random = rand()/(float)RAND_MAX;
        if(random <= PROB_CAUTIOUS)
            is_malicious = "cautious";
        else if(PROB_CAUTIOUS < random && random <= PROB_CAUTIOUS + PROB_SEMI_CAUTIOUS)
            is_malicious = "semi_cautious";
        else
            is_malicious = "berserk";
    }
    else
        is_malicious = "";
    return is_malicious;
}



vector<string> split(string str, char delim)
{
    vector<string> tokens;
    int pos_delim;
    pos_delim = str.find(",");
    tokens.push_back(str.substr(0,pos_delim));
    tokens.push_back(str.substr(pos_delim+1,str.length()));
    return tokens;
}





void Node_Module::watts_strogatz(int n, int k, double p)
{
    for(int j = 1; j < k/2 + 1; j++)
    {
        int new_neigh;
        if(uniform(0,1) <= p)
        {
            new_neigh = intuniform(0,n-1);
            while(new_neigh == getId()-2 || find(Neighbors.begin(), Neighbors.end(), new_neigh) != Neighbors.end())
            {
                new_neigh = intuniform(0,n-1);
                if(Neighbors.size() >= n-1)
                    break;
            }
        }
        else
        {
            new_neigh = (getId()+j-2)%n;
        }
        Neighbors.push_back(new_neigh);
        Node_Module *module = check_and_cast<Node_Module*>(getParentModule()->getSubmodule("node",new_neigh));
        module->Neighbors.push_back(getId()-2);
        setGateSize("exchanges",gateSize("exchanges")+1);
        getParentModule()->getSubmodule("node",new_neigh)->setGateSize("exchanges",getParentModule()->getSubmodule("node",new_neigh)->gateSize("exchanges")+1);
        gate("exchanges$o", gateSize("exchanges")-1)->connectTo(getParentModule()->getSubmodule("node",new_neigh)->gate("exchanges$i", getParentModule()->getSubmodule("node",new_neigh)->gateSize("exchanges")-1));
        cDisplayString& connDispStr = gate("exchanges$o", gateSize("exchanges")-1)->getDisplayString();
        connDispStr.parse("ls=green,3");
        getParentModule()->getSubmodule("node",new_neigh)->gate("exchanges$o", getParentModule()->getSubmodule("node",new_neigh)->gateSize("exchanges")-1)->connectTo(gate("exchanges$i", gateSize("exchanges")-1));
        cDisplayString& connDispStr2 = getParentModule()->getSubmodule("node",new_neigh)->gate("exchanges$o", getParentModule()->getSubmodule("node",new_neigh)->gateSize("exchanges")-1)->getDisplayString();
        connDispStr2.parse("ls=green,3");
    }
}









void Node_Module::initialize()
{
    int nb = getParentModule()->par("number_of_node");
    watts_strogatz(nb, 20, 1.0);
    cMessage *msg_time = new cMessage("Time_Msg", Time);
    scheduleAt(1,msg_time);
}






bool compareByGate(const tuple<int, int>* a, const tuple<int, int>* b)
{
    return get<1>(*a) < get<1>(*b);
}







void Node_Module::simulation_prob()
{
    double prob = getParentModule()->par("PROB_0");

    string file="./WATTS_STROGATZ/Semi_Cautious/Conv_with_semi_cautious_nb_rounds/10/CONV" + to_string((int)(prob*100+0.5)) + ".csv";

    int number = getParentModule()->par("number_of_node");
    int count = getParentModule()->par("count");

    LOG_SIM.open(file.c_str(),fstream::app);
    if(node->Is_malicious == "")
        LOG_SIM << node->Opinion[ROUNDS] << ";";
    if(count == number)
        LOG_SIM<<endl;
    LOG_SIM.close();
}



void Node_Module::handleMessage(cMessage *msg)
{
    if(msg->getKind() == Init)
    {
        int* id = (int*) msg->getContextPointer();
        tuple<int, int>* tup = new tuple<int, int>;
        int gate = msg->getArrivalGate()->getIndex();
        *tup = {*id, gate};
        delete(id);
        node->Neighbors[gate] = tup;
        delete msg;


        double silence=rand()/(float)RAND_MAX;
        if(node->Is_malicious != "semi_cautious" || silence > SILENCE_PROBA)
        {
            node->Send[node->Round][gate] = true;
            int op = node->Opinion[0];
            int wt = node->Weight[0];
            MsgOpinion* my_op = new MsgOpinion;
            my_op->node = node->Id;
            my_op->op = op;
            my_op->wt = wt;

            MsgReceivedOpinion* receiv_op = new MsgReceivedOpinion;
            receiv_op->my_op = my_op;
            receiv_op->my_neighbors_op = nullptr;

            cMessage *msg_receiv_op = new cMessage("Received_Opinion_Msg", Received_Opinion);
            msg_receiv_op->setContextPointer(receiv_op);
            send(msg_receiv_op, "exchanges$o", gate);
        }
    }
    else if(msg->getKind() == Received_Opinion)
    {
        node->Count++;
        MsgReceivedOpinion* receiv_op = (MsgReceivedOpinion*) msg->getContextPointer();
        if(find(get<1>(node->Blacklist).begin(), get<1>(node->Blacklist).end(), msg->getArrivalGate()->getIndex()) == get<1>(node->Blacklist).end())
        {
            node->addNeigh_to_Receiv(receiv_op->my_op->node, receiv_op->my_op->op, receiv_op->my_op->wt);
            if(receiv_op->my_neighbors_op != nullptr)
            {
                for(int i = 0; i < receiv_op->my_neighbors_op->size(); i++)
                {
                    node->addNeigh_of_Neigh_to_Receiv((*receiv_op->my_neighbors_op)[i]->node, (*receiv_op->my_neighbors_op)[i]->neighbors, (*receiv_op->my_neighbors_op)[i]->op, (*receiv_op->my_neighbors_op)[i]->wt);
                }
            }
        }

        if(receiv_op->my_neighbors_op != nullptr)
        {
            for(int i = 0; i < receiv_op->my_neighbors_op->size(); i++)
            {
                delete((*receiv_op->my_neighbors_op)[i]);
            }
        }
        delete(receiv_op->my_op);
        delete(receiv_op);
        delete msg;
    }
    else if(msg->getKind() == Time)
    {
        delete msg;
        if(Ready == false)
        {
            double random=rand()/(float)RAND_MAX;
        string is_malicious = initialize_type_node();
        vector<int> opinion;
        double Prob_0 = getParentModule()->par("PROB_0");
        if(random <= Prob_0)
        {
          opinion.push_back(0);
          if(is_malicious == "")
              getDisplayString().setTagArg("i", 1, "red");
          else
              getDisplayString().setTagArg("i", 1, "orange");
        }
        else{
          opinion.push_back(1);
          if(is_malicious == "")
              getDisplayString().setTagArg("i", 1, "cyan");
          else
              getDisplayString().setTagArg("i", 1, "blue");
        }
        vector<tuple<int, int>*> neighbors(gateSize("exchanges"), nullptr);
        vector<int> blacklist_id;
        vector<int> blacklist_gate;
        tuple<vector<int>, vector<int>> blacklist = {blacklist_id, blacklist_gate};
        vector<map<int, tuple<int, int, map<int, tuple<int, int>>>>> received_opinion;
        map<int, tuple<int, int, map<int, tuple<int, int>>>> m;
        received_opinion.push_back(m);
        vector<int> weight = {gateSize("exchanges")};
        vector<bool> s(gateSize("exchanges"), false);
        vector<vector<bool>> send_vector = {s};
        node = new Node(getId(),opinion,neighbors,blacklist,received_opinion,false,weight,send_vector,is_malicious,0,0);
        for(int i = 0; i < gateSize("exchanges"); i++)
        {
            cMessage *msg_init = new cMessage("Init_Msg", Init);
            int* id = new int;
            *id = node->Id;
            msg_init->setContextPointer(id);
            send(msg_init, "exchanges$o", i);
        }
            cMessage *msg_time = new cMessage("Time_Msg", Time);
            scheduleAt(node->Round+2,msg_time);
            Ready = true;
        }
        else
        {
            node->Round++;
            int r = node->Round;
            int count = getParentModule()->par("count");
            getParentModule()->par("count").setIntValue(count + 1);
            int number = getParentModule()->par("number_of_node");
            cellular_consensus();
            count = getParentModule()->par("count");
            if(node->Round == ROUNDS)
            {
                simulation_prob();
                for(int i = 0; i < node->Neighbors.size(); i++)
                {
                    delete(node->Neighbors[i]);
                }
                delete(node);
            }
            if(count == number)
            {
                char text[128];
                sprintf(text,"Round number: %d", r - 1);
                getSimulation()->getActiveEnvir()->alert(text);
                getParentModule()->par("count").setIntValue(0);
                if(r == ROUNDS)
                    endSimulation();
            }
        }
    }
}
