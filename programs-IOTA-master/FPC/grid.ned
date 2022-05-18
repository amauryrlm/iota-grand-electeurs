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

// Created: 2021-5-24 for project FPC

import NodeModule;

import ned.DelayChannel;

//
// A generated network with grid topology.
//

network Grid
{
    parameters:
        int columns = default(15);
        int rows = columns;
        double MALICIOUS_PROBA=default(0.2);
        double PROB_0=default(0.5);
        string type_malicious=default("Semi-Cautious_adversary");
        int number_of_nodes=columns*rows;
        int lambda=default(4);
        int query_size=default(10);
        double beta_=default(0.25);
        int rounds=default(30);
        
    submodules:
        node[columns*rows]: NodeModule {
            @display("p=,,matrix,$columns");
            
        }
    connections allowunconnected:
        for i=0..rows-1, for j=0..columns-1 {
            node[i*columns+j].port++ <--> DelayChannel <--> node[(i+1)*columns+j].port++ if i!=rows-1;
            node[i*columns+j].port++ <--> DelayChannel <--> node[i*columns+j+1].port++ if j!=columns-1;
        }
}
