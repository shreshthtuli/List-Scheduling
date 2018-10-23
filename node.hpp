class Node
{
    Resource type;
    int start;
    int delay;
    
    Node(Node::Resource t, int d){
        type = t;
        delay = d;
    }
}