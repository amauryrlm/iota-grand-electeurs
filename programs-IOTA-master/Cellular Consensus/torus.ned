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

import ned.DelayChannel;

//
// A generated network with torus topology.
//
network Torus
{
    parameters:
        int count = 0;
        int columns = default(6);
        int rows = default(6);
        int number_of_node = columns*rows;
        double PROB_0 = default(0.5);
        @display("bgb=595,404");
    submodules:
        node[columns*rows]: Node_Module {
            @display("p=263,155,matrix,$columns");

        }
    connections:
        for i=0..rows-1, for j=0..columns-1 {
            node[i*columns+j].exchanges++ <--> { @display("ls=green,3"); } <--> node[((i+1)%rows)*columns+j].exchanges++;
            node[i*columns+j].exchanges++ <--> { @display("ls=green,3"); } <--> node[(i*columns+(j+1)%columns)].exchanges++;
        }
}
