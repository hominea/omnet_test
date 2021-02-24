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

network wsn
{
	parameters:
		double R;		//半径
		int node_num;	//节点数目,500
		int conect_databits; 
		int data_databits;
	submodules:
		node[node_num]: Node{};
		sink:Sink{};
	connections allowunconnected:	//可以允许为不连接
}
