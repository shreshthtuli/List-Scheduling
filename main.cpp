/*

MIT License

Copyright (c) 2018 Shreshth Tuli

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <iostream>
#include <vector>
#include <fstream>
#include <deque>
#include <string>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/graph_utility.hpp>

using namespace boost;
using namespace std;

class Node
{
    public:
    char type;
    int start, id;

    Node(){};
    Node(char t, int idn){type = t; id = idn;}
};

typedef labeled_graph<adjacency_list<listS, vecS, bidirectionalS, Node>, string> Graph;
typedef adjacency_list<listS, vecS, bidirectionalS, Node> AdjGraph;
typedef graph_traits<Graph>::vertex_descriptor vertex_t;
typedef graph_traits<Graph>::vertex_iterator vertex_iter;
typedef graph_traits<Graph>::edge_descriptor edge_t;
typedef graph_traits<Graph>::edge_iterator edge_iter;

map <char, int> delay;
map <char, int> quantity;
vector <string> V;
vector <pair<string,string>> E;
Graph g;

void printVertices(){
    for(int i = 0; i < V.size(); i++)
        cout << V.at(i) << endl;
}

void printEdges(){
    for(int i = 0; i < E.size(); i++)
        cout << E.at(i).first << " " << E.at(i).second << " " << endl;
}

void printGraph(){
    cout << "GRAPH : " << endl;
    printVertices(); 
    printEdges();
}

void printInDegrees(){
    AdjGraph& underlying = g.graph();
    typedef property_map<Graph, vertex_index_t>::type IndexMap;
    IndexMap index = get(vertex_index, underlying);

    pair<vertex_iter, vertex_iter> vp;
    for (vp = vertices(underlying); vp.first != vp.second; ++vp.first) {
        cout << "Indegree of : " << V.at((int)index[*vp.first]) << " = " << in_degree(*vp.first, underlying) << endl;
    }

    /*
    For iterating over all vertices use vertex iterator and index[*vp.first] for index in V
    For iterating over all edges use E
    */
}

void parseGraph(Graph &graph){
    ifstream graphFile("graph.txt");

    while(!graphFile.eof()){
        vertex_t u, v;
        stringstream ss, ss1;
        char type; int id;
        Node *n[2];

        for(int i = 0; i <= 1; i++){
            graphFile >> type >> id;
            n[i] = new Node(type, id);
        }

        // Add nodes to graph
        ss.str(""); ss << n[0]->type << n[0]->id;
        if(std::find(V.begin(), V.end(), ss.str()) == V.end()) {
            add_vertex(ss.str(), graph);
            V.push_back(ss.str());
        }
        
        ss1.str(""); ss1 << n[1]->type << n[1]->id;
        if(std::find(V.begin(), V.end(), ss1.str()) == V.end()) {
            add_vertex(ss1.str(), graph);
            V.push_back(ss1.str());
        }

        // Add edge    
        E.push_back(pair<string,string>(ss.str(), ss1.str()));
        add_edge_by_label(ss.str(), ss1.str(), graph);
    }

    printGraph();
    graphFile.close();
}

void parseInput(){
    ifstream input("input.txt");
    int a;

    for(char i : {'C', 'A', 'S', 'M', 'R', 'W'}){
        input >> a >> a;
        quantity.insert(pair <char, int> (i, a));
        input >> a;
        delay.insert(pair <char, int> (i, a));
    }

    input.close();
}

int main(int argc, char** argv){

    parseInput();
    parseGraph(g);

}