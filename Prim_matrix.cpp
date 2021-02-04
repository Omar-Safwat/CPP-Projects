/*Reads a graph from a file and finds Minimum spanning using Prim's Algorithm

                    8th November 2020

                    Omar Safwat
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <limits>
#include <numeric>
using namespace std;

const int INF_DIST = numeric_limits<int>::max(); //Infinite distance

typedef struct NODE
{
    int vertex;
    int weight;
} NODE;
bool operator<(NODE N1, NODE N2) { return (N1.weight < N2.weight ? true : false); }

void heapify(vector<NODE> &pq, int size, int root)
{
    int smallest = root;
    int l = 2 * root + 1;
    int r = 2 * root + 2;

    // if left child is smaller than root
    if (l < size && pq[l] < pq[smallest])
        smallest = l;

    // if right child is smaller than smallest so far
    if (r < size && pq[r] < pq[smallest])
        smallest = r;

    if (smallest != root)
    {
        swap(pq[root], pq[smallest]);

        // recursively heapify the affected sub-tree
        heapify(pq, size, smallest);
    }
}

void heapSort(vector<NODE> &pq)
{
    int size = pq.size();
    //Builds a heap (rearrange).
    for (int root = size / 2 - 1; root >= 0; root--)
        heapify(pq, size, root);

    for (int i = size - 1; i > 0; i--)
    {
        //Moves the smallest vertex (At the root, pq[0]) to the end of the array (Bottom right corner of heap).
        swap(pq[0], pq[i]);
        //Moves the next smallest vertex to the root, to repeat the above step.
        heapify(pq, i, 0);
    }
}

class PriorityQueue
{
public:
    explicit PriorityQueue(int n);
    void insert(NODE N) { pq.push_back(N); }
    void sort_queue() { heapSort(pq); }
    int top() { return pq.back().vertex; } // returns vetrex with top priority
    void pop_queue() { pq.pop_back(); }
    void chgPriority(int v, int w);
    void print_queue()
    {
        cout << "My Queue is:\n";
        for (int i = 0; i < pq.size(); i++)
            cout << pq[i].weight << "\t\t";
    }
    bool is_empty() { return pq.empty(); }

private:
    vector<NODE> pq;
};

PriorityQueue::PriorityQueue(int n)
{
    for (int i = 1; i < n; i++)
        insert({i, INF_DIST});
    insert({0, 0});
}
void PriorityQueue::chgPriority(int v, int w)
{
    int i = 0;
    //Looks for the element in Queue with this vertex ID.
    while (pq[i].vertex != v)
        i++;
    //Update nodes weight.
    pq[i].weight = w;
}

class Graph
{
public:
    explicit Graph(ifstream &my_file);
    vector<int> get_edges(int row) { return adj[row]; }
    int get_size() { return size; }
    void print_graph();

private:
    int size;
    vector<vector<int>> adj; //2D vector to store edges
};

Graph::Graph(ifstream &my_file)
{
    my_file >> size;    //Size of graph is always the first int written in the file
    for (int i = 0; i < size; i++)
    {
        adj.push_back(vector<int>(size, 0));
    }
    int row, col, dist;
    //Reading the edges from file written in this order: U V Distance.
    while (my_file)
    {
        my_file >> row;
        my_file >> col;
        my_file >> dist;
        adj[row][col] = dist;
    }
    my_file.close();
}
void Graph::print_graph()
{
    for (auto i = adj.begin(); i != adj.end(); i++)
    {
        for (auto j = (*i).begin(); j != (*i).end(); j++)
        {
            cout << *j << "\t";
        }
        cout << endl;
    }
}

void update_adjNodes(Graph &g, PriorityQueue &PQ, vector<bool> &visited, vector<int> &parent, vector<int> &min_dist)
{
    int top = PQ.top();                   //top is the vertex with top priority (least weight)
    visited[top] = true;
    PQ.pop_queue();
    vector<int> edges = g.get_edges(top); //A vector containing the data of neighbouring nodes
    int v = 0;
    for (auto i = edges.begin(); i != edges.end(); i++, v++)
    {
        //Updating distance values
        if (*i > 0 && visited[v] == false && *i < min_dist[v])
        {
            min_dist[v] = *i;
            parent[v] = top;
            PQ.chgPriority(v, *i);
        }
    }
    PQ.sort_queue();
}

void prim_algo(Graph &g)
{
    int n = g.get_size();
    vector<int> min_dist(n, INF_DIST); //To store the shortest edge for each node
    min_dist[0] = 0;
    vector<bool> visited(n, false);
    visited[0] = true;
    vector<int> parent(n);             //To save the path with the least cost
    parent[0] = 0;
    PriorityQueue PQ(n);
    vector<int> cost(n, 0);

    while (!PQ.is_empty())
        update_adjNodes(g, PQ, visited, parent, min_dist);

    cout << "Printing out the path to each vertex:\n\n";
    for (int i = 0; i < parent.size(); i++)
    {
        cout << i;
        int my_parent = parent[i];
        cost[i] = min_dist[i];
        while (my_parent > 0)
        {
            cout << "<-" << my_parent;
            cost[i] += min_dist[my_parent];
            my_parent = parent[my_parent];
        }
        cout << "<-0\n\n";
    }
    cout << "Vertex\t\tCost to this node\n\n";  
    int node = 0;
    for (auto iter = cost.begin(); iter != cost.end(); node++, iter++)
        cout << node << "\t\t" << *iter << endl;
    cout << endl << "Cost of tree = " << accumulate(min_dist.begin(), min_dist.end(), 0) << endl;
}

int main()
{
    ifstream my_file("week4_hw.txt");
    if (my_file.is_open())
    {
        Graph g(my_file);
        //g.print_graph();
        prim_algo(g);
    }
    else
    {
        cout << "File has failed to open" << endl;
    }
    return 0;
}