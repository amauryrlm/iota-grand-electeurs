#include "Node.h"

#include <fstream>
#include <sstream>

enum MessageType {GIVE_OPINION,RANDOM_WALK,LAUNCH,NEIGHBOR};


ofstream CSV_SIM;



class NodeModule: public cSimpleModule
{
    private:
        int t;
        Node *self;
        vector<int> Neighbors;
        simtime_t powTime;
        vector<cMessage *> Messages;
        void ChangeColor();
        void watts_strogatz();
        vector<int> getValidNeighbors(vector<int>);
        void launch();
        void sendOpinion(cMessage *msg);
        void CalculateOpinion(int time);
        void RandomWalk(cMessage *msg);
        void TabuRandomWalk(cMessage *msg);
        void FIFORandomWalk(cMessage *msg);
        void LRURandomWalk(cMessage *msg);
        void RANDRandomWalk(cMessage *msg);

        void SimulationProba();
        void SimulationProbaMalicious();
        void SimulationProbaMalicious_beta();
        void SimulationDistanceK();
        void SimulationNodeK();
        void SimulationRounds();

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void finish() override;
};

Define_Module(NodeModule);

void NodeModule::watts_strogatz()
{
    int n=getParentModule()->par("number_of_nodes");
    int k=getParentModule()->par("k");
    double p=getParentModule()->par("p");
    for(int j = 1; j < k/2 + 1; j++)
    {
        int new_neigh;
        if(uniform(0,1) <= p)
        {
            new_neigh = intuniform(0,n-1);
            while(new_neigh == getId()-2 || find(Neighbors.begin(), Neighbors.end(), new_neigh) != Neighbors.end())
            {
                new_neigh = intuniform(0,n-1);
                if(Neighbors.size() == n-1)
                    break;
            }
        }
        else
        {
            new_neigh = (getId()+j-2)%n;
        }
        Neighbors.push_back(new_neigh);
        NodeModule *module = check_and_cast<NodeModule*>(getParentModule()->getSubmodule("node",new_neigh));
        module->Neighbors.push_back(getId()-2);
        setGateSize("port",gateSize("port")+1);
        getParentModule()->getSubmodule("node",new_neigh)->setGateSize("port",getParentModule()->getSubmodule("node",new_neigh)->gateSize("port")+1);
        gate("port$o", gateSize("port")-1)->connectTo(getParentModule()->getSubmodule("node",new_neigh)->gate("port$i", getParentModule()->getSubmodule("node",new_neigh)->gateSize("port")-1));
        getParentModule()->getSubmodule("node",new_neigh)->gate("port$o", getParentModule()->getSubmodule("node",new_neigh)->gateSize("port")-1)->connectTo(gate("port$i", gateSize("port")-1));
    }
}

struct list_gate{
    int gate_Id;
    struct list_gate *suivant;
};

struct WalkMessage_t{
    int distance;
    int round;
    vector<int> visited;
    struct list_gate* liste;
};

struct list_gate *add_list(struct list_gate *list,int gate){
    if(list==NULL){
        struct list_gate* new_liste=new struct list_gate;
        new_liste->gate_Id=gate;
        new_liste->suivant=NULL;
        return new_liste;
    }
    else{
        struct list_gate* new_elem=new struct list_gate;
        new_elem->gate_Id=gate;
        new_elem->suivant=list;
        return new_elem;
    }
}

struct GiveOpinion_t{
    int opinion;
    int round;
    struct list_gate* liste;
};



void NodeModule::initialize(){
    float random=rand()/(float)RAND_MAX;
    int opinion;
    double Prob_0=getParentModule()->par("PROB_0");
    if(random <= Prob_0)
    {
      opinion=0;
    }
    else{
      opinion=1;
    }
    vector<Node_Identity>neighbors;
    vector<int> received_opinion;
    int TMAX=getParentModule()->par("rounds");
    vector<bool> v(TMAX,false);
    Node* node = new Node(getId(),opinion,received_opinion,neighbors,false,"",v);
    self=node;
    t=0;
    double malicious_proba=getParentModule()->par("MALICIOUS_PROBA");
    if(uniform(0,1)<=malicious_proba){
        string type=getParentModule()->par("type_malicious");
        self->Is_malicious=type;
        if(uniform(0,1)<=LYING_PROBA){
            self->Is_lying[0]=true;
        }
        else{
            self->Is_lying[0]=false;
        }
    }
    ChangeColor();
    //watts_strogatz(); //Use only if you want to create a Watts-Strogatz graph
    for(int i=0;i<gateSize("port");i++){
        int *myId=new int;
        *myId=getId();
        cMessage *Neighbors=new cMessage("I am your neighbor",NEIGHBOR);
        Messages.push_back(Neighbors);
        Neighbors->setContextPointer(myId);
        send(Neighbors,"port$o",i);
    }
    for(int time=1;time<TMAX;time++){
        if(self->Is_malicious=="Cautious_adversary" && uniform(0,1)<=LYING_PROBA){
            self->Is_lying[time]=true;
        }
        else if(self->Is_malicious=="Cautious_adversary" && uniform(0,1)>LYING_PROBA){
            self->Is_lying[time]=false;
        }
        cMessage *Init=new cMessage("Init",LAUNCH);
        int *timeStamp=new int;
        *timeStamp=time;
        Init->setContextPointer(timeStamp);
        scheduleAt(time,Init);
    }
}

void NodeModule::handleMessage(cMessage *msg){
    if(msg -> isSelfMessage()){
        if(msg->getKind()==LAUNCH && self->Finalized==false){
            int* time=(int *)msg->getContextPointer();
            if(*time > 1){
                CalculateOpinion(*time-1);
            }
            Messages.push_back(msg);
            int n=Messages.size();
            for(int i=0;i<n;i++){
                delete Messages[0];
                Messages.erase(Messages.begin());
            }
            launch();
        }

    }
    else{
        if(msg->getKind()==RANDOM_WALK){
            RandomWalk(msg);
        }
        else if(msg->getKind()==GIVE_OPINION){
            struct GiveOpinion_t *data=(GiveOpinion_t *)msg->getContextPointer();
            if(data->liste!=NULL){
                int sendGate=data->liste->gate_Id;
                list_gate *temp=data->liste;
                data->liste=data->liste->suivant;
                //delete temp;
                msg->setContextPointer(data);
                send(msg,"port$o",sendGate);
            }
            else{
                self->ReceivedOpinion.push_back(data->opinion);
                //delete data;
            }
        }
        else if(msg->getKind()==NEIGHBOR){
            int* neighbor_id=(int*)msg->getContextPointer();
            struct Node_Identity neighbor={*neighbor_id,msg->getArrivalGate()->getIndex()};
            self->Neighbors.push_back(neighbor);
        }


    }

}

void NodeModule::launch(){
    int QUERY_SIZE=getParentModule()->par("query_size");
    for(int i=0;i<QUERY_SIZE;i++){
        cMessage *RandomWalk=new cMessage("Random_walk",RANDOM_WALK);
        Messages.push_back(RandomWalk);
        int n=gateSize("port");
        int rand=intuniform(0, n-1);
        struct WalkMessage_t *data=new struct WalkMessage_t;
        int LAMBDA=getParentModule()->par("lambda");
        data->distance=LAMBDA;
        vector<int> empty_vector;
        data->visited=empty_vector;
        data->visited.push_back(self->Id);
        data->round=t;
        data->liste=NULL;
        RandomWalk->setContextPointer(data);
        send(RandomWalk,"port$o",rand);
    }
    t++;
}


void NodeModule::ChangeColor(){
    if(self->Is_malicious==""){
        if(self->Opinion == 0){
            getDisplayString().setTagArg("i", 1, "red");
        }
        else{
            getDisplayString().setTagArg("i", 1, "cyan");
        }
    }
    else{
        if(self->Opinion == 0){
            getDisplayString().setTagArg("i", 1, "orange");
        }
        else{
            getDisplayString().setTagArg("i", 1, "blue");
        }
    }
}




vector<int> NodeModule::getValidNeighbors(vector<int> visited){
    vector<int> Valid_Neighbors;
    vector<int> Valid_Neighbors_Id;
    bool temp=false;
    for(int i=0;i<self->Neighbors.size();i++){
        temp=false;
        for(int j=0;j<visited.size();j++){
            if(visited[j] == self->Neighbors[i].Id){
                temp=true;
                break;
            }

        }
        if(temp==false){
            Valid_Neighbors.push_back(self->Neighbors[i].Gate_Number);
            Valid_Neighbors_Id.push_back(self->Neighbors[i].Id);
        }
    }
    return Valid_Neighbors;
}



void NodeModule::RandomWalk(cMessage *msg){
    struct WalkMessage_t *temp= (struct WalkMessage_t *)msg->getContextPointer();
    temp->distance=temp->distance-1;

    if(temp->distance > 0){
        temp->visited.push_back(self->Id);
        vector<int> ValidNeighbors=getValidNeighbors(temp->visited);
        int n=ValidNeighbors.size();
        if(n >= 1){
            temp->liste=add_list(temp->liste,msg->getArrivalGate()->getIndex());
            cMessage *RandomWalk=new cMessage("Random_walk",RANDOM_WALK);
            Messages.push_back(RandomWalk);
            int random=intuniform(0, n-1);
            RandomWalk->setContextPointer(temp);
            send(RandomWalk,"port$o",ValidNeighbors[random]);
        }
        else{
            sendOpinion(msg);
        }
    }
    else{
        sendOpinion(msg);
    }
}

void NodeModule::TabuRandomWalk(cMessage *msg){
    struct WalkMessage_t *temp= (struct WalkMessage_t *)msg->getContextPointer();
    temp->distance=temp->distance-1;

    if(temp->distance > 0){
        if(temp->visited.size() == TABU_LIST_SIZE){
            int choice=intuniform(0,1);
            if(choice == 1){
                temp->visited.push_back(self->Id);
                vector<int> occurences;
                for(int i=0;i<temp->visited.size();i++){
                    occurences.push_back(i);
                }
                int element_to_delete=occurences[uniform(0,occurences.size()-1)];
                temp->visited.erase(temp->visited.begin()+element_to_delete);
            }
        }
        else{
            int choice=uniform(0,2);
            if(choice==1){
                temp->visited.push_back(self->Id);
                vector<int> occurences;
                for(int i=0;i<temp->visited.size();i++){
                    occurences.push_back(i);
                }
                int element_to_delete=occurences[uniform(0,occurences.size()-1)];
                temp->visited.erase(temp->visited.begin()+element_to_delete);
            }
            else if(choice==2){
                temp->visited.push_back(self->Id);
            }
        }
        vector<int> ValidNeighbors=getValidNeighbors(temp->visited);
        int n=ValidNeighbors.size();
        temp->liste=add_list(temp->liste,msg->getArrivalGate()->getIndex());
        cMessage *RandomWalk=new cMessage("Random_walk",RANDOM_WALK);
        Messages.push_back(RandomWalk);
        RandomWalk->setContextPointer(temp);
        if(n >= 1){
            int random=intuniform(0, n-1);
            send(RandomWalk,"port$o",ValidNeighbors[random]);
        }
        else{
            int random=intuniform(0, self->Neighbors.size()-1);
            send(RandomWalk,"port$o",self->Neighbors[random].Gate_Number);
        }
    }
    else{
        sendOpinion(msg);
    }
}

void NodeModule::FIFORandomWalk(cMessage *msg){
    struct WalkMessage_t *temp= (struct WalkMessage_t *)msg->getContextPointer();
    temp->distance=temp->distance-1;

    if(temp->distance > 0){
        if(temp->visited.size() == TABU_LIST_SIZE){
            temp->visited.pop_back();
        }
        temp->visited.insert(temp->visited.begin(),self->Id);
        vector<int> ValidNeighbors=getValidNeighbors(temp->visited);
        int n=ValidNeighbors.size();
        temp->liste=add_list(temp->liste,msg->getArrivalGate()->getIndex());
        cMessage *RandomWalk=new cMessage("Random_walk",RANDOM_WALK);
        Messages.push_back(RandomWalk);
        RandomWalk->setContextPointer(temp);
        if(n >= 1){
            int random=intuniform(0, n-1);
            //EV << random <<endl;
            send(RandomWalk,"port$o",ValidNeighbors[random]);
        }
        else{
            int random=intuniform(0, self->Neighbors.size()-1);
            send(RandomWalk,"port$o",self->Neighbors[random].Gate_Number);
        }
    }
    else{
        sendOpinion(msg);
    }
}

void NodeModule::LRURandomWalk(cMessage *msg){
    struct WalkMessage_t *temp= (struct WalkMessage_t *)msg->getContextPointer();
    temp->distance=temp->distance-1;

    if(temp->distance > 0){
        temp->visited.erase(remove(temp->visited.begin(), temp->visited.end(), self->Id), temp->visited.end());
        if(temp->visited.size() == TABU_LIST_SIZE){
            temp->visited.pop_back();
        }
        temp->visited.insert(temp->visited.begin(),self->Id);
        vector<int> ValidNeighbors=getValidNeighbors(temp->visited);
        int n=ValidNeighbors.size();
        temp->liste=add_list(temp->liste,msg->getArrivalGate()->getIndex());
        cMessage *RandomWalk=new cMessage("Random_walk",RANDOM_WALK);
        Messages.push_back(RandomWalk);
        RandomWalk->setContextPointer(temp);
        if(n >= 1){
            int random=intuniform(0, n-1);
            send(RandomWalk,"port$o",ValidNeighbors[random]);
        }
        else{
            int random=intuniform(0, self->Neighbors.size()-1);
            send(RandomWalk,"port$o",self->Neighbors[random].Gate_Number);
        }
    }
    else{
        sendOpinion(msg);
    }
}

void NodeModule::RANDRandomWalk(cMessage *msg){
    struct WalkMessage_t *temp= (struct WalkMessage_t *)msg->getContextPointer();
    temp->distance=temp->distance-1;

    if(temp->distance > 0){
        if (find(temp->visited.begin(),temp->visited.end(),self->Id)==temp->visited.end()){
            temp->visited.insert(temp->visited.begin(),self->Id);
            if(temp->visited.size() == TABU_LIST_SIZE+1){

                int random=intuniform(0,TABU_LIST_SIZE);
                temp->visited.erase(temp->visited.begin()+random);
            }
        }
        vector<int> ValidNeighbors=getValidNeighbors(temp->visited);
        int n=ValidNeighbors.size();
        temp->liste=add_list(temp->liste,msg->getArrivalGate()->getIndex());
        cMessage *RandomWalk=new cMessage("Random_walk",RANDOM_WALK);
        Messages.push_back(RandomWalk);
        RandomWalk->setContextPointer(temp);
        if(n >= 1){
            int random=intuniform(0, n-1);
            send(RandomWalk,"port$o",ValidNeighbors[random]);
        }
        else{
            int random=intuniform(0, self->Neighbors.size()-1);
            send(RandomWalk,"port$o",self->Neighbors[random].Gate_Number);
        }
    }
    else{
        sendOpinion(msg);
    }
}



void NodeModule::sendOpinion(cMessage *msg){
    struct WalkMessage_t *temp= (struct WalkMessage_t *)msg->getContextPointer();
    int sendGate= msg->getArrivalGate()->getIndex();
    struct GiveOpinion_t *data=new struct GiveOpinion_t;
    if(self->Is_malicious=="Cautious_adversary" && self->Is_lying[temp->round]==true){
        if(self->Opinion==0){
            data->opinion=1;
        }
        else{
            data->opinion=0;
        }
    }
    else if(self->Is_malicious=="Cautious_adversary" && self->Is_lying[temp->round]==false){
        data->opinion=self->Opinion;
    }
    else if(self->Is_malicious=="Berserk_adversary"){
        if(uniform(0,1) < LYING_PROBA){
            if(self->Opinion==0){
                data->opinion=1;
            }
            else{
                data->opinion=0;
            }
        }
        else{
            data->opinion=self->Opinion;
        }
    }
    else{
        data->opinion=self->Opinion;
    }
    data->round=temp->round;
    data->liste=temp->liste;
    cMessage *GiveOpinion=new cMessage("Sending Opinion",GIVE_OPINION);
    Messages.push_back(GiveOpinion);
    GiveOpinion->setContextPointer(data);
    if(!(self->Is_malicious=="Semi-Cautious_adversary" && uniform(0,1) < SILENCE_PROBA)){
        send(GiveOpinion,"port$o",sendGate);
    }
}

void NodeModule::CalculateOpinion(int time){
    double mean = 0;
    int new_opinion;
    for(int i=0;i<self->ReceivedOpinion.size();i++){
        mean+=self->ReceivedOpinion[i];
    }
    mean=mean/(double)self->ReceivedOpinion.size();
    if(time==1){
        if(mean >= TAU){
          new_opinion=1;
        }
        else{
            new_opinion=0;
        }
    }
    else{
        double BETA=getParentModule()->par("beta_");
        double unif=uniform(BETA,1-BETA);
        if(mean > unif){
            new_opinion=1;
        }
        else if(mean < unif){
            new_opinion=0;
        }
        else{
            new_opinion=self->Opinion;
        }
    }
    if(new_opinion==self->Opinion){
        self->cnt++;
    }
    else{
        self->cnt=0;
    }
    self->Opinion=new_opinion;
    self->setFinalized();
    ChangeColor();
    vector<int> empty_vector;
    self->ReceivedOpinion=empty_vector;
}

void NodeModule::SimulationProba(){
    double Prob_0=getParentModule()->par("PROB_0");

    string file="./CSV_FILE/PROBA" + to_string((int)(Prob_0*100+0.2)) + ".csv";
    CSV_SIM.open(file.c_str(),fstream::app);
    if(getId()!=2){
        CSV_SIM << ";";
    }
    CSV_SIM << self->Opinion;
    int number=getParentModule()->par("number_of_nodes");
    if(getId()==number+1){
        CSV_SIM<<endl;
    }
    CSV_SIM.close();
}

void NodeModule::SimulationProbaMalicious(){
    double Proba_malicious=getParentModule()->par("MALICIOUS_PROBA");

    string file="./CSV_FILE/MALICIOUS_PROBA" + to_string((int)(Proba_malicious*100)) + ".csv";
    CSV_SIM.open(file.c_str(),fstream::app);
    if(getId()!=2){
        CSV_SIM << ";";
    }
    CSV_SIM << self->Opinion;
    int number=getParentModule()->par("number_of_nodes");
    if(getId()==number+1){
        CSV_SIM<<endl;
    }
    CSV_SIM.close();
}

void NodeModule::SimulationProbaMalicious_beta(){
    double Proba_malicious=getParentModule()->par("MALICIOUS_PROBA");
    double BETA = getParentModule()->par("beta_");
    string file="./CSV_FILE/MALICIOUS_PROBA" + to_string((int)(Proba_malicious*100+0.2)) + +"_BETA" + to_string((int)(BETA*100+0.2))+ ".csv";
    CSV_SIM.open(file.c_str(),fstream::app);
    if(getId()!=2){
        CSV_SIM << ";";
    }
    CSV_SIM << self->Opinion;
    int number=getParentModule()->par("number_of_nodes");
    if(getId()==number+1){
        CSV_SIM<<endl;
    }
    CSV_SIM.close();
}

void NodeModule::SimulationDistanceK(){
    int QUERY_SIZE=getParentModule()->par("query_size");
    int LAMBDA=getParentModule()->par("lambda");
    string file="./CSV_FILE/DISTANCE" + to_string(LAMBDA)+"_K" + to_string(QUERY_SIZE)+".csv";
    CSV_SIM.open(file.c_str(),fstream::app);
    if(getId()!=2){
        CSV_SIM << ";";
    }
    CSV_SIM << self->Opinion;
    int number=getParentModule()->par("number_of_nodes");
    if(getId()==number+1){
        CSV_SIM<<endl;
    }
    CSV_SIM.close();
}


void NodeModule::SimulationNodeK(){
    int NUMBER=getParentModule()->par("number_of_nodes");
    int K=getParentModule()->par("query_size");
    string file="./CSV_FILE/NODE" + to_string(NUMBER)+ "_K" + to_string(K) + ".csv";
    CSV_SIM.open(file.c_str(),fstream::app);
    if(getId()!=2){
        CSV_SIM << ";";
    }
    CSV_SIM << self->Opinion;
    int number=getParentModule()->par("number_of_nodes");
    if(getId()==number+1){
        CSV_SIM<<endl;
    }
    CSV_SIM.close();
}


void NodeModule::SimulationRounds(){
    int TMAX=getParentModule()->par("rounds");
    string file="./CSV_FILE/ROUNDS" + to_string(TMAX)+".csv";
    CSV_SIM.open(file.c_str(),fstream::app);
    if(getId()!=2){
        CSV_SIM << ";";
    }
    CSV_SIM << self->Opinion;
    int number=getParentModule()->par("number_of_nodes");
    if(getId()==number+1){
        CSV_SIM<<endl;
    }
    CSV_SIM.close();
}

void NodeModule::finish(){
    int TMAX=getParentModule()->par("rounds");
    CalculateOpinion(TMAX-2);
    SimulationProba();
    int n=Messages.size();
    for(int i=0;i<n;i++){
        delete Messages[0];
        Messages.erase(Messages.begin());
    }
    delete self;
}

