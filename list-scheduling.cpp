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
#include <algorithm>
#include "./list-scheduling.hpp"

using namespace boost;
using namespace std;

void ASAP(Graph& g){
    vector <string> Vtemp = V;
    Graph temp = g;
    typedef property_map<Graph, vertex_index_t>::type IndexMap;
    IndexMap index = get(vertex_index, temp);
    cout << "ASAP : " << endl;

    pair<vertex_iter, vertex_iter> vp;
    for (vp = vertices(temp); vp.first != vp.second; ++vp.first) {
        if((int)in_degree(*vp.first, temp) == 0){
            asap.insert(pair <string,int> (V.at((int)index[*vp.first]), 1));            
            Vtemp.erase(remove(Vtemp.begin(), Vtemp.end(), V.at((int)index[*vp.first])), Vtemp.end());
            cout << "removed : " << V.at((int)index[*vp.first]) << endl;
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
            cout << "checking vertex : " << V.at((int)index[*vp.first]) << endl;
            for(tie(in_i, in_end) = in_edges(v, g); in_i != in_end; ++in_i){
                e = *in_i;
                vertex_t src = source(e, g);
                int i = index[*(vertex_iter) src];
                if(asap.find(V.at(i)) == asap.end()){
                    all_pred_scheduled = false; continue;
                }
                int complete_time = asap.at(V.at(i)) + delay.at(V.at(i).at(0));
                cout << "pred : " << V.at(i) << " complete at : " << complete_time-1 << endl; 
                maxEnd = max(maxEnd, complete_time);
                if(complete_time > cycle_num){
                    all_pred_scheduled = false;
                }
            }
            if(all_pred_scheduled){
                asap.insert(pair <string,int> (V.at((int)index[*vp.first]), maxEnd));
                Vtemp.erase(remove(Vtemp.begin(), Vtemp.end(), V.at((int)index[*vp.first])), Vtemp.end());
                cout << "removed : " << V.at((int)index[*vp.first]) << " = " << maxEnd << endl;
                critical_path_length = max(critical_path_length, maxEnd + delay.at(V.at((int)index[*vp.first]).at(0)));
            }
            cycle_num++;
        }
    }
    
}

void ALAP(Graph& g){
    vector <string> Vtemp = V;
    Graph temp = g;
    typedef property_map<Graph, vertex_index_t>::type IndexMap;
    IndexMap index = get(vertex_index, temp);
    cout << "ALAP : " << endl;
    cout << "Critical path length = " << critical_path_length << endl;

    pair<vertex_iter, vertex_iter> vp;
    for (vp = vertices(temp); vp.first != vp.second; ++vp.first) {
        if((int)out_degree(*vp.first, temp) == 0){
            alap.insert(pair <string,int> (V.at((int)index[*vp.first]), critical_path_length - delay.at(V.at((int)index[*vp.first]).at(0))));
            Vtemp.erase(remove(Vtemp.begin(), Vtemp.end(), V.at((int)index[*vp.first])), Vtemp.end());
            cout << "removed : " << V.at((int)index[*vp.first]) << " = " << critical_path_length - delay.at(V.at((int)index[*vp.first]).at(0)) << endl;
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
            vertex_t v = *vp.first; int minStart = critical_path_length;
            cout << "checking vertex : " << V.at((int)index[*vp.first]) << endl;
            for(tie(out_i, out_end) = out_edges(v, g); out_i != out_end; ++out_i){
                e = *out_i;
                vertex_t targ = target(e, g);
                int i = index[*(vertex_iter) targ];
                if(alap.find(V.at(i)) == alap.end()){
                    all_succ_scheduled = false; continue;
                }
                int start_time = alap.at(V.at(i));
                minStart = min(minStart, start_time);
                cout << "succ : " << V.at(i) << " start at : " << start_time << endl; 
                if(start_time > cycle_num){
                    all_succ_scheduled = false;
                }
            }
            if(all_succ_scheduled){
                alap.insert(pair <string,int> (V.at((int)index[*vp.first]), minStart - delay.at(V.at((int)index[*vp.first]).at(0))));
                Vtemp.erase(remove(Vtemp.begin(), Vtemp.end(), V.at((int)index[*vp.first])), Vtemp.end());
                cout << "removed : " << V.at((int)index[*vp.first]) << " = " << minStart - delay.at(V.at((int)index[*vp.first]).at(0)) << endl;
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

void computeMobilities(){
    for(int i = 0; i < V.size(); i++){
        string v = V.at(i);
        mobility.insert(pair <string,int> (v, alap.at(v) - asap.at(v)));
    }
}

void printMobilities(){
    cout << "Printing Mobilities :" << endl;
    for(int i = 0; i < V.size(); i++){
        cout << V.at(i) << " : " << mobility.at(V.at(i)) << endl; 
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

void printSchedule(){
    cout << "Node\tCycle\tResource number\n";
    for(int i = 0; i < V.size(); i++){
        if(V.at(i).at(0) != 'C')
            cout << V.at(i) << "\t" << schedule.at(V.at(i)) << "\t" << resource_num.at(V.at(i)) << endl;
    }
}

void writeGraph(string file){
    ofstream output(file);
    for(int i = 0; i < E.size(); i++){
        output << E.at(i).first.at(0) << " " << E.at(i).first.at(1) << " " << E.at(i).second.at(0) << " " << E.at(i).second.at(1); 
        if(i != E.size() - 1)
            output << endl;
    }
    output.flush();
    output.close();
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

bool inDegZero(string node){
    for(int i = 0; i < E_original.size(); i++){
        if(E_original.at(i).second == node)
            return false;
    }
    return true;
}

void parseGraph(Graph &graph, string file){
    cout << "Parsing graph\n";
    ifstream graphFile(file);
    V.clear();
    E.clear();
    asap.clear();
    alap.clear();
    mobility.clear();
    critical_path_length = 1;

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

    if(file == "graph2.txt"){
        graphFile.close();
        return;
    }
    E_original = E;
    V.push_back("C0");
    add_vertex("C0", graph);
    AdjGraph& underlying = graph.graph();
    typedef property_map<Graph, vertex_index_t>::type IndexMap;
    IndexMap index = get(vertex_index, underlying);

    pair<vertex_iter, vertex_iter> vp;
    for (vp = vertices(underlying); vp.first != vp.second; ++vp.first) {
        if((int)out_degree(*vp.first, underlying) == 0 && V.at((int)index[*vp.first]) != "C0"){
            E.push_back(pair<string,string>( V.at((int)index[*vp.first]), "C0"));
            add_edge_by_label(V.at((int)index[*vp.first]), "C0", graph);
        }
    }

    // printGraph();
    graphFile.close();
    cout << "Parsing complete\n";
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

vector<string> readyList(Graph& gr, char type){
    list<string> list;

    AdjGraph& underlying = gr.graph();
    typedef property_map<Graph, vertex_index_t>::type IndexMap;
    IndexMap index = get(vertex_index, underlying);

    pair<vertex_iter, vertex_iter> vp;
    for (vp = vertices(underlying); vp.first != vp.second; ++vp.first) {
        bool scheduled = schedule.find(V.at((int)index[*vp.first])) != schedule.end();
        if(scheduled)
            continue;
        if((int)in_degree(*vp.first, underlying) == 0 && V.at((int)index[*vp.first]).at(0) == type){
            list.push_back(V.at((int)index[*vp.first]));
        }
    }

    list.sort(mobilityComparator());
    vector<string> v{list.begin(), list.end()};
    return v;
}

vector<string> listScheduleHelper(Graph& gcopy, char a){
    vector<string> ready = readyList(gcopy, a);
    vector<string> selected;

    int num_critical = 0;
    for(int i = 0; i < ready.size(); i++){
        if(mobility.at(ready.at(i)) == 0)
            num_critical++;
    }

    for(int i = 0; i < min(quantity.at(a), (int)ready.size()); i++){
        cout << ready.at(i) << endl;
        resource_num.insert(pair<string,int>(ready.at(i), i+1));
        selected.push_back(ready.at(i));
        int cycle = (inDegZero(ready.at(i))) ? last_cycle : 1;
        cout << "Start cycle : " << cycle << endl;
        // Determine last cycle of parents
        for(int j = 0; j < E_original.size(); j++){
            auto p = E_original.at(j);
            if(p.second == ready.at(i) && schedule.find(p.first) != schedule.end())
                cycle = max(cycle, schedule.at(p.first) + delay.at(p.first.at(0)));
        }
        schedule.insert(pair<string,int>(ready.at(i), cycle));
        last_cycle = max(last_cycle, cycle);
        cout << "Given schedule " << cycle << " to " << ready.at(i) << endl;
    }

    return selected;
}

bool removeVertices(Graph& gr, vector<string> remove){
    cout << "removing ";
    sort(V.begin(), V.end());
    sort(remove.begin(), remove.end());
    vector<string> difference;
    std::set_difference(
        V.begin(), V.end(),
        remove.begin(), remove.end(),
        std::back_inserter( difference )
    );
    V = difference; 
    for(int i = 0; i < remove.size(); i++){
        cout << remove.at(i) << " ";
        gr.remove_vertex(remove.at(i));        
    }    
    vector<pair<string,string>> Etemp;
    for(int j = 0; j < E.size(); j++){
        if(find(V.begin(), V.end(), E.at(j).first) != V.end() && find(V.begin(), V.end(), E.at(j).second) != V.end())
            Etemp.push_back(E.at(j));
    }
    E = Etemp;
    cout << endl;
    return V.size() == 1;
}

void listSchedule(){
    int cycle = 1;
    vector<string> remove;
    int limit = 1;
    bool done;

    while(limit != 0 || !done){
        Graph g;
        parseGraph(g, "graph2.txt");
        printGraph();
        ASAP(g);
        printASAP();
        ALAP(g);
        printALAP();
        computeMobilities();
        printMobilities();
        remove.clear();
        for(char a : {'C', 'A', 'S', 'M', 'R', 'W'}){
            vector<string> r = listScheduleHelper(g, a);
            remove.insert(std::end(remove), std::begin(r), std::end(r));
        }
        done = removeVertices(g, remove);
        if(done){
            schedule.insert(pair<string, int>(V.at(0), 0));
            resource_num.insert(pair<string,int>(V.at(0), 1));
            break;
        }
        writeGraph("graph2.txt");
        limit = num_vertices(g);
    }    
}

int main(int argc, char** argv){

    if(argc > 1)
        std::cout.setstate(std::ios_base::failbit);
    parseInput();
    parseGraph(g, "graph.txt");
    writeGraph("graph2.txt");
    listSchedule();
    parseGraph(g, "graph.txt");
    std::cout.clear();
    printSchedule();

}