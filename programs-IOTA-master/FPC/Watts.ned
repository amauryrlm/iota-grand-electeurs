//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

// Created: 2021-6-15 for project FPC_NEW

import NodeModule;

import ned.DelayChannel;

//
// A generated network with ring topology.
//
network Watts
{
    parameters:
        int count=default(0);
        double MALICIOUS_PROBA=default(0.33);
        double PROB_0=default(0.5);
        string type_malicious=default("Cautious_adversary");
        int number_of_nodes=default(10);
        int lambda=default(4);
        int query_size=default(10);
        double beta_=default(0.25);
        int rounds=default(30);
        int k=default(4);
        double p=default(1);
    submodules:
        node[number_of_nodes]: NodeModule;
}
