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
simple Node
{
    parameters:
        double energy;
        double eperbit;
        double fs;
        double amp;
        double thresholdDistance;
        double senRange;			// 发送范围(最大能到达100)
        double x;					// 节点坐标 x
        double y;					// 节点坐标 y
        @display("p=$x,$y;i=misc/node_vs,black");
        
    gates:
        input in[];
        output out[2];					
    
}
