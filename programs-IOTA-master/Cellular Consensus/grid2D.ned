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

// Created: 2021-5-23 for project Cellular_Consensus

import Node_Module;

import ned.DelayChannel;

//
// A generated network with grid topology.
//
network Grid2D
{
    parameters:
        int count = 0;
        int columns = default(4);
        int rows = columns;
        int number_of_node = columns*rows;
        double PROB_0 = default(0.25);
        double PROB_MALICIOUS = default(0);
    submodules:
        node[columns*rows]: Node_Module {
            @display("p=,,matrix,$columns");
            
        }
    connections allowunconnected:
        for i=0..rows-1, for j=0..columns-1 {
            node[i*columns+j].exchanges++ <--> { @display("ls=green,3"); } <--> node[(i+1)*columns+j].exchanges++ if i!=rows-1;
            node[i*columns+j].exchanges++ <--> { @display("ls=green,3"); } <--> node[i*columns+j+1].exchanges++ if j!=columns-1;
        }
}
