simple HonestNode
{
   parameters:
       bool sendMsgOnInit = default(true);
       int limit = default(10);
    gates:
        input in;
        output out;
}

simple MaliciousNode extends HonestNode
{
	parameters:
	    bool interfer = default(true);
}

network Network
{
    submodules:
        node1: HonestNode{
            
        }
        node2: MaliciousNode{
        }
    connections:
        node1.out --> {  delay = 100ms; } --> node2.in;
        node1.in <-- {  delay = 100ms; } <-- node2.out;
}