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

import Node_Module;

import ned.DelayChannel;

network Watts_Strogatz
{
    parameters:
        int count = 0;
        int number_of_node = 225;
        double PROB_0 = default(0.5);
        double PROB_MALICIOUS = default(0);
    submodules:
        node[number_of_node]: Node_Module {
            @display("p=,,ring");
            
        }
}
