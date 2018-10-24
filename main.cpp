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
#include <boost/graph/copy.hpp>
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
typedef graph_traits<Graph>::in_edge_iterator in_edge_iter;
typedef graph_traits<Graph>::out_edge_iterator out_edge_iter;

map <char, int> delay;
map <char, int> quantity;
map <string, int> asap;
map <string, int> alap;
int critical_path_length = 1;
vector <string> V;
vector <pair<string,string>> E;
Graph g;

void ASAP(){
    vector <string> Vtemp = V;
    Graph temp = g;
    typedef property_map<Graph, vertex_index_t>::type IndexMap;
    IndexMap index = get(vertex_index, temp);
    // cout << "ASAP : " << endl;

    pair<vertex_iter, vertex_iter> vp;
    for (vp = vertices(temp); vp.first != vp.second; ++vp.first) {
        if((int)in_degree(*vp.first, temp) == 0){
            asap.insert(pair <string,int> (V.at((int)index[*vp.first]), 1));
            Vtemp.erase(remove(Vtemp.begin(), Vtemp.end(), V.at((int)index[*vp.first])), Vtemp.end());
            // cout << "removed : " << V.at((int)index[*vp.first]) << endl;
        }
    }

    in_edge_iter in_i, in_end;
    edge_t e;
    int cycle_num = 2;

    while(Vtemp.size() != 0){
        for (vp = vertices(temp); vp.first != vp.second; ++vp.first) {
            if(find(Vtemp.begin(), Vtemp.end(), V.at((int)index[*vp.first])) == Vtemp.end())
                continue;
            bool all_pred_scheduled = true;
            vertex_t v = *vp.first; int maxEnd = 0;
            // cout << "checking vertex : " << V.at((int)index[*vp.first]) << endl;
            for(tie(in_i, in_end) = in_edges(v, g); in_i != in_end; ++in_i){
                e = *in_i;
                vertex_t src = source(e, g);
                int i = index[*(vertex_iter) src];
                if(asap.find(V.at(i)) == asap.end()){
                    all_pred_scheduled = false; continue;
                }
                int complete_time = asap.at(V.at(i)) + delay.at(V.at(i).at(0));
                // cout << "pred : " << V.at(i) << " complete at : " << complete_time-1 << endl; 
                maxEnd = max(maxEnd, complete_time);
                if(complete_time > cycle_num){
                    all_pred_scheduled = false;
                }
            }
            if(all_pred_scheduled){
                asap.insert(pair <string,int> (V.at((int)index[*vp.first]), maxEnd));
                Vtemp.erase(remove(Vtemp.begin(), Vtemp.end(), V.at((int)index[*vp.first])), Vtemp.end());
                // cout << "removed : " << V.at((int)index[*vp.first]) << " = " << maxEnd << endl;
                critical_path_length = max(critical_path_length, maxEnd + delay.at(V.at((int)index[*vp.first]).at(0)));
            }
            cycle_num++;
        }
    }
    
}

void ALAP(){
    vector <string> Vtemp = V;
    Graph temp = g;
    typedef property_map<Graph, vertex_index_t>::type IndexMap;
    IndexMap index = get(vertex_index, temp);
    // cout << "ALAP : " << endl;
    // cout << "Critical path length = " << critical_path_length << endl;

    pair<vertex_iter, vertex_iter> vp;
    for (vp = vertices(temp); vp.first != vp.second; ++vp.first) {
        if((int)out_degree(*vp.first, temp) == 0){
            alap.insert(pair <string,int> (V.at((int)index[*vp.first]), critical_path_length - delay.at(V.at((int)index[*vp.first]).at(0))));
            Vtemp.erase(remove(Vtemp.begin(), Vtemp.end(), V.at((int)index[*vp.first])), Vtemp.end());
            // cout << "removed : " << V.at((int)index[*vp.first]) << " = " << critical_path_length - delay.at(V.at((int)index[*vp.first]).at(0)) << endl;
        }
    }

    out_edge_iter out_i, out_end;
    edge_t e;
    int cycle_num = critical_path_length - 1;

    while(Vtemp.size() != 0){
        for (vp = vertices(temp); vp.first != vp.second; ++vp.first) {
            if(find(Vtemp.begin(), Vtemp.end(), V.at((int)index[*vp.first])) == Vtemp.end())
                continue;
            bool all_succ_scheduled = true;
            vertex_t v = *vp.first; int minStart;
            // cout << "checking vertex : " << V.at((int)index[*vp.first]) << endl;
            for(tie(out_i, out_end) = out_edges(v, g); out_i != out_end; ++out_i){
                e = *out_i;
                vertex_t targ = target(e, g);
                int i = index[*(vertex_iter) targ];
                if(alap.find(V.at(i)) == alap.end()){
                    all_succ_scheduled = false; continue;
                }
                int start_time = alap.at(V.at(i));
                minStart = min(minStart, start_time);
                // cout << "succ : " << V.at(i) << " start at : " << start_time << endl; 
                if(start_time > cycle_num){
                    all_succ_scheduled = false;
                }
            }
            if(all_succ_scheduled){
                alap.insert(pair <string,int> (V.at((int)index[*vp.first]), minStart - delay.at(V.at((int)index[*vp.first]).at(0))));
                Vtemp.erase(remove(Vtemp.begin(), Vtemp.end(), V.at((int)index[*vp.first])), Vtemp.end());
                // cout << "removed : " << V.at((int)index[*vp.first]) << " = " << minStart - delay.at(V.at((int)index[*vp.first]).at(0)) << endl;
            }
            cycle_num++;
        }
    }
    
}

void printASAP(){
    cout << "Printing ASAP shedule :" << endl;
    for(int i = 0; i < V.size(); i++){
        cout << V.at(i) << " : " << asap.at(V.at(i)) << endl; 
    }
}

void printALAP(){
    cout << "Printing ALAP shedule :" << endl;
    for(int i = 0; i < V.size(); i++){
        cout << V.at(i) << " : " << alap.at(V.at(i)) << endl; 
    }
}

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
    string s;

    for(char i : {'C', 'A', 'S', 'M', 'R', 'W'}){
        input >> s >> a;
        // cout << "inserting q : " << i << " " << a << endl;
        quantity.insert(pair <char, int> (i, a));
        input >> a;
        // cout << "inserting d : " << i << " " << a << endl;
        delay.insert(pair <char, int> (i, a));
    }

    input.close();
}

int main(int argc, char** argv){

    parseInput();
    parseGraph(g);
    ASAP();
    printASAP();
    ALAP();
    printALAP();

}