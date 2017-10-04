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


//
// TODO documentation
//
simple Networklayer
{
    parameters:
        volatile double iaTime;
    @display("i=block/switch");
    gates:
        input in[];
        output out[];
}
module Host
{
    parameters:
        int address;
    @display("i=device/pc");
    gates:
        input in[];
        output out[];
    submodules:
        networklayer:Networklayer
        {
            gates:
                in[sizeof(in)];
                out[sizeof(out)];
        }
    connections:
        for i=0..sizeof(in)-1
        {
            networklayer.out[i] --> out[i];
            in[i] --> networklayer.in[i];
        }
}
module Switch
{
    parameters:
        int address;
    @display("i=device/switch");
    gates:
        input in[];
        output out[];
    submodules:
        networklayer:Networklayer
        {
            gates:
                in[sizeof(in)];
                out[sizeof(out)];
        }
    connections:
        for i=0..sizeof(in)-1
        {
            networklayer.out[i] --> out[i];
            in[i] --> networklayer.in[i];
        }
}
channel lanlink extends ned.DatarateChannel
{
    parameters:
        delay = 10s;
        datarate = 100000000bps; // bits/sec
}

channel wanlink extends ned.DatarateChannel
{
    parameters:
        delay = 5s;
        datarate = 1000000bps; // bits/sec
}
network Network
{
    parameters:
        int event;
        int node_num;
    submodules:
        host[4]:Host;
        switch[2]:Switch;
   connections:
       host[0].out++ --> lanlink --> switch[0].in++;
       host[0].in++ <-- lanlink <-- switch[0].out++;
       
       host[1].out++ --> lanlink --> switch[0].in++;
       host[1].in++ <-- lanlink <-- switch[0].out++;
       
       switch[1].out++ --> wanlink --> switch[0].in++;
       switch[1].in++ <-- wanlink <-- switch[0].out++;
       
       host[2].out++ --> lanlink --> switch[1].in++;
       host[2].in++ <-- lanlink <-- switch[1].out++;
       
       host[3].out++ --> lanlink --> switch[1].in++;
       host[3].in++ <-- lanlink <-- switch[1].out++;
       
}

