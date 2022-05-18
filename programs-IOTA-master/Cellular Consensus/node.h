#ifndef NODE_H_
#define NODE_H_

#include <vector>
#include <map>
#include <tuple>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <time.h>
#include <string.h>
#include <random>
#include <numeric>
#include <algorithm>
#include <functional>
#include <limits>
#include <omnetpp.h>

#define INFTY numeric_limits<int>::max()
#define L 10
#define ROUNDS 10
#define PROB_CAUTIOUS 0
#define PROB_SEMI_CAUTIOUS 1
#define PROB_BERSERK 0
#define LYING_PROBA 0.5
#define SILENCE_PROBA 0.5


using namespace std;
using namespace omnetpp;

class Node
{
    public :
        int Id;
        vector<int> Opinion;
        vector<tuple<int, int>*> Neighbors;
        tuple<vector<int>, vector<int>> Blacklist;
        vector<map<int, tuple<int, int, map<int, tuple<int, int>>>>> Received_opinion;
        bool Finalized;
        vector<int> Weight;
        vector<vector<bool>> Send;
        string Is_malicious;
        bool is_lying;
        int Count;
        int Round;

        Node(int id, vector<int> opinion, vector<tuple<int, int>*> neighbors, tuple<vector<int>, vector<int>> blacklist, vector<map<int, tuple<int, int, map<int, tuple<int, int>>>>> received_opinion, bool finalized, vector<int> weight, vector<vector<bool>> send, string is_malicious, int count, int round);

        // Getter
        int getId();

        vector<int> getOpinion();

        vector<tuple<int, int>*> getNeighbors();

        tuple<vector<int>, vector<int>> getBlacklist();

        vector<map<int, tuple<int, int, map<int, tuple<int, int>>>>> getReceived_opinion();

        bool getFinalized();

        vector<int> getWeight();

        vector<vector<bool>> getSend();

        string getIs_malicious();

        int getCount();

        int getRound();

        // Setter
        void setWeight(int weight, int t);

        void setFinalized(bool finalized);

        // Adder
        void addNeighbors(tuple<int, int>* Node_to_add);

        void addBlacklist(tuple<int, int>* Node_to_add);

        void addOpinion(int opinion_to_add);

        void addWeight(int weight_to_add);

        void addNeigh_to_Receiv(int Node_to_add, int Node_opinion, int Node_weight);

        void addNeigh_of_Neigh_to_Receiv(int Node_where_add, int Node_to_add, int Node_opinion, int Node_weight);

        // Shower
        void showOpinion();

        void showNeighbors();

        void showBlacklist();

        void showMap_node_tuple_int(map<int, tuple<int, int>> m);

        void showTuple(tuple<int, int, map<int, tuple<int, int>>> t);

        void showMap_node_tuple(map<int, tuple<int, int, map<int, tuple<int, int>>>> m);

        void showReceived_opinion();

        bool opinion_changed();
};

#endif /* NODE_H_ */
