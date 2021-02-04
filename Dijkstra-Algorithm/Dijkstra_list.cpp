/*Shortest Path algortihm using Adjacency list representation

                    2nd November 2020

                    Omar Safwat
*/                    

#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <utility>
#include <cmath>
using namespace std;

const double MAX_DIST = 10.0;   //Max edge distance
const double MIN_DIST = 1.0;
const int INF_DIST = 12.0;      //Infinite distance
const int NNODES = 50;          //Default number of Nodes generated per graph

//NODE represents the vertices in graph
typedef struct NODE 
{ 
    int vertex; 
    int weight;
    NODE* next;
}NODE;
bool operator< (NODE N1, NODE N2) { return (N1.weight < N2.weight ? true : false); }

//Heap sorting functions
void heapify(vector<NODE> &pq, int size, int root)
{
    int smallest = root;
    int l = 2*root + 1;
    int r = 2*root + 2;
 
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
        heapify (pq, i, 0);
    }
}

//Priority Queue using STL vector
class PriorityQueue
{
    public:
    explicit PriorityQueue (int n = NNODES);
    void insert (NODE N) {pq.push_back (N);}
    void sort_queue () {heapSort (pq);}
    int top () {return pq.back().vertex;}             // returns NODE with top priority
    void pop_queue () {pq.pop_back();}
    void chgPriority (int v, int w);
    void print_queue () 
    {
        cout << "My Queue is:\n";
        for (int i = 0; i < pq.size(); i++)
            cout << pq[i].weight << "\t\t";
    }
    bool is_empty () {return pq.empty ();}
    private:
    vector <NODE> pq;
};

PriorityQueue::PriorityQueue (int n)
{
    for (int i = 0; i < n; i++)
        insert ({i, INF_DIST, NULL});
}
void PriorityQueue::chgPriority (int v, int w)
{
    int i = 0;
    //Looks for the element in Queue with this vertex ID.
    while (pq[i].vertex != v)
        i++;
    //Update nodes weight.
    pq[i].weight = w;    
}

//Defining class adjacency_list
class adj_list
{
    public:
    friend class Graph;
    adj_list () : head (NULL), tail (NULL) {}
    void add_edge (int v, int w)
    {
        NODE* newNode = new NODE;
        newNode->vertex = v;
        newNode->weight = w;
        newNode->next = NULL;
        
        if (head == NULL)
        {
            head = newNode;
            tail = newNode;
        }
        else
        {
            tail->next = newNode;
            tail = newNode;
        } 
    }
    NODE* get_head () {return head;}
    void print_list () 
    {
        NODE* current = head;
        while (current != NULL)
        {
            cout << current->vertex << "->";
            current = current->next;
        }
    }
    ~adj_list () 
    {
        //cout << "list destructor was invoked";
        while (head != NULL)
        {
            NODE* temp = new NODE;
            temp = head;
            head = head->next;
            delete temp;
        }
    }
    private:
    NODE* head;
    NODE* tail;
};

//Class Graph
class Graph
{
    public:
    explicit Graph (int n = NNODES);
    void print_graph ();
    vector <NODE> get_adjNodes (int vertex);
   ~Graph () {delete [] adj;}

    private:    
    adj_list* adj;           //An array (adj) of pointers to linked lists, which store graph's data
    int size;
};

Graph::Graph (int n) : size (n)
{
    default_random_engine e(time(0));
    bernoulli_distribution prob (0.4);                  //Graph density factor is 40%
    uniform_int_distribution <int> d(0, 49);            //distribute edge distances with uniform prob.

    adj = new adj_list [size];                          //Array of list heads for each vertex                                                                  
    int MAX_edge_num = 0.5* size * (size -1);           //Max number of edges for an undirected graph with size as number of vertices                  
   
    for (int j = 0; j < MAX_edge_num / size; j++)
        //For each adjacency node
        for (int i = 0; i < size; i++)
        {
            //Probability an egde exist
            bool p = prob (e);

            //pick a random vertix and a random edge to connect with vertex i
            int v = d (e);
            int w = d (e) % 10 + 1;

            //Assign results to adjacency list
            if (p == true) {adj [i].add_edge (v, w); adj [v].add_edge (i, w);}  
        }
}

void Graph::print_graph ()
{
    for (int i = 0; i < size; i++)
    {
        cout << i << "->";
        adj [i].print_list ();
        cout << endl;
    }
}

vector <NODE> Graph::get_adjNodes (int top)
{
    NODE* current = adj [top].get_head ();
    vector <NODE> adj_nodes;                //Create a Vector to store data of adjacent nodes
    while (current != NULL)
    {
        adj_nodes.push_back ({current->vertex, current->weight, NULL});
        current = current->next;
    }
    return adj_nodes;                       //return the vector to Dijkstra function
}

void update_adjNodes (Graph &g, PriorityQueue &PQ, int dist [], int parent [])
{
    int top = PQ.top ();                                //top is the vertex with top priority (least weight)
    PQ.pop_queue ();
    vector <NODE> adjNodes = g.get_adjNodes (top);      //A vector containing the data of neighbouring nodes

    for (int i = 0; i < adjNodes.size (); i++)
    {   
        int v = adjNodes [i].vertex;
        int w = adjNodes [i].weight;
        //Updating distance values
        if ((w + dist [top]) < dist [v])
        {   
            dist [v] = w + dist [top];
            parent [v] = top;
            //cout << parent [v] << endl;
            PQ.chgPriority (v, w + dist [top]);
        }
    }
    PQ.sort_queue ();
}

//Shortest path algorithm
void dijkstra (Graph &g, int src, int n)   
{
    int dist [n];                           //To record min dist from source node
    int parent [n];                         //Parent array saves the shortest path information
    PriorityQueue PQ (n);
    for (int i = 0; i < n; i++) 
        dist [i] = INF_DIST;    

    dist [src] = 0;
    parent [src] = 0;
    while (!PQ.is_empty ())
        update_adjNodes (g, PQ, dist, parent); 
    
    //Calculating average shortest path
    double avg = 0;
    for (int i = 0; i < n; i++)
    {
        //To check if the node has atleast one edge (connected)
        if (dist [i] < INF_DIST)
            avg += (dist [i] - avg) / (i+1);    //A superior calculation to prevent overflow
    }
    cout << "Average Shortest Path is: " << avg << "\n\nPrintiong out shortest path of each vertex:\n\n";
    for (int i = 0; i < n; i++)
    {
        cout << i;
        int v = parent [i];
        while (v > 0)
        {
            cout << "<-" << v;
            v = parent [v];
        }
        cout << "<-0\n\n";
    }
}

int main ()
{
        //Graph is generated randomly and shortest path for each vertex is printed out
        Graph g(50);
        dijkstra (g, 0, 50);
        return 0;
}







