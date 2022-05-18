#ifndef NODE_H
#define NODE_H

#define LYING_PROBA 0.5
#define SILENCE_PROBA 0.5
#define TAU 0.5
#define FINALIZE 10
#define TABU_LIST_SIZE 6

#include <vector>
#include <map>
#include <tuple>
#include <iostream>
#include <math.h>
#include <time.h>
#include <string.h>
#include <random>
#include <numeric>
#include <algorithm>
#include <functional>
#include <omnetpp.h>

using namespace omnetpp;
using namespace std;

struct Node_Identity{
    int Id;
    int Gate_Number;
};

class Node
{
    public :
        int Id;
        int Opinion;
        vector<int> ReceivedOpinion;
        vector<Node_Identity> Neighbors;
        bool Finalized;
        int cnt;
        string Is_malicious;
        vector<bool> Is_lying;

        Node(int id, int opinion, vector<int> received_opinion,vector<Node_Identity> neighbors, bool finalized,string is_malicious,vector<bool> v);

        // Shower
        void showOpinion();
        void showReceivedOpinion();

        void showNeighbors();

        void setFinalized();

};


#endif
