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

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/graph_utility.hpp>

using namespace boost;
using namespace std;

class Node{
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
map <string, int> mobility;
map <string, int> schedule;
map <string, int> resource_num;
int critical_path_length = 1;
vector <string> V;
vector <pair<string,string>> E;
vector <pair<string,string>> E_original;
Graph g;
Graph g_original;
int last_cycle = 1;

struct mobilityComparator{
    bool operator ()(const string s, const string s1){
        return mobility.at(s) < mobility.at(s1);
    }
};