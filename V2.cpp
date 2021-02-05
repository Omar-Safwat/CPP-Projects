//A Hex game. The program prints out the board in ASCII, and allows for user input, deciding whether it's valid or not
//and whether the game is won.
//By: Omar Safwat
// 4th of Feb. 2021

#include<iostream>
#include<iterator>
#include<vector>
#include<string>
#include<random>
#include<ctime>
using namespace std;

const int boardSize = 11;
//Random seed for the move by the program
default_random_engine e(time(nullptr));
uniform_int_distribution<int> distribution(0, boardSize - 1);
//Creat a disjoint "Union-find" set class
class DisjSet
{
    public:
    //Create a set of n items "Number of Hexes available on the board"
    DisjSet(int n)
    {
        size = new int[n];
        parent = new int[n];
        this -> n = n;
        makeSet();
    }

    //create a tree for each hex on the board
    void makeSet()
    {
        for (int i = 0; i < n; i++)
        {
            parent[i] = i;
            size[i] = 1;
        }
    }

    int findSet(int v)
    {
        if (v == parent[v])
            return v;
        return parent[v] = findSet(parent[v]);
    }

    void unionSets(int a, int b)
    {
        int root_a = findSet(a);
        int root_b = findSet(b);
        //Compare ranks assigned to roots of each tree
        if(root_a != root_b)
        {
            if(size[root_a] < size[root_b])
            {
                parent[root_a] = root_b;
                size[root_b] += size[root_a];
            }
            else if(size[root_a] > size[root_b])
            {
                parent[root_b] = root_a;
                size[root_a] += size[root_b];
            }
            else
            {
                parent[root_b] = root_a;
                size[root_a] += size[root_b];
            }
        }
    };

    protected:
    int *size, *parent, n;

};

//Class to manage and display the board
class board
{
    public:
    board(int n) : SIZE(n), my_set(n * n), boardMap(n, vector<char>(n, '.')) {}
    //Method to place a stone on the board
    void placeStone (int input_row, int input_col) {boardMap[input_row - 1][input_col - 1] = 'B';}
    //Get stone value occupying a certain position
    char stoneValue (int input_row, int input_col) {return boardMap[input_row - 1][input_col - 1];}
    //Connect adjacet stones to form a bridge
    void connectStones (int input_row, int input_col);
    //Display board
    void display ();
    //Check if either player won
    bool is_won (int input_row, int input_col);
    //AI's` play move
    int play ();

    protected:
    //Number of Hexes per column and row
    int SIZE;
    DisjSet my_set;
    //boardMap: '.' for empty hex, 'B' for Blue hex, 'R' for Red hex
    vector<vector<char>> boardMap;
};

//Associate stones on the board of the same colour to make a path with DisjSet.
void board::connectStones (int input_row, int input_col)
{
    //let (i,j) be the matrix coordinates of the new hex input by the player
    //subtract an additional 1, because C++ starts index from 0
    int i = input_row - 1, j = input_col - 1;
    char newStone = boardMap[i][j];
    //Column and row of adjacent hex to connect with.
    int row_adj, col_adj;
    //If the new stone is at the edges of the board
    if(input_col == SIZE | input_row == 1 | input_row == SIZE | input_col == 1)
    {
        //If it is at the top edge
        if(input_row == 1)
        {   
            //If Top left corner, has two edges
            if(input_col == 1)
            {
                row_adj = i + 1;
                col_adj = j;
                if(boardMap[row_adj][col_adj] == newStone)
                    my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
                row_adj = i;
                col_adj = j + 1;
                if(boardMap[row_adj][col_adj] == newStone)
                    my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
            }
            //If top right corner, has 3 edges
            else if(input_col = SIZE)
            {
                row_adj = i;
                col_adj = j - 1;
                if(boardMap[row_adj][col_adj] == newStone)
                    my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
                row_adj = i + 1;
                for(col_adj = j - 1; col_adj <= j; col_adj++)
                    if(boardMap[row_adj][col_adj] == newStone)
                        my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
            }
            //Anywhere between the two corners, has 4 edges
            else
            {
                row_adj = i;
                for(col_adj = j - 1; col_adj <= j + 1; col_adj += 2)
                    if(boardMap[row_adj][col_adj] == newStone)
                        my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
                row_adj = i + 1;
                for(col_adj = j - 1; col_adj <= j; col_adj++)
                    if(boardMap[row_adj][col_adj] == newStone)
                        my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
            }
        }
        //If it is at the bottom edge
        else if(input_row == SIZE)
        {
        //if at the Bottem left corner, has 3 edges
        if(input_col == 1)
        {
            row_adj = i;
            col_adj = j + 1;
            if(boardMap[row_adj][col_adj] == newStone)
                my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
            row_adj = i - 1;
            for(col_adj = j; col_adj <= j + 1; col_adj++)
                if(boardMap[row_adj][col_adj] == newStone)
                    my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        }
        //If at the bottom right corner, has 2 edges
        else if(input_col == SIZE)
        {
            row_adj = i - 1;
            col_adj = j;
            if(boardMap[row_adj][col_adj] == newStone)
                my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
            row_adj = i;
            col_adj = j - 1;
            if(boardMap[row_adj][col_adj] == newStone)
                my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        }
        //Anywhere between the two corners, has 4 edges
        else
        {
            row_adj = i;
            for(col_adj = j - 1; col_adj <= j + 1; col_adj += 2)
                if(boardMap[row_adj][col_adj] == newStone)
                    my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
            row_adj = i - 1;
            for(col_adj = j; col_adj <= j + 1; col_adj++)
                if(boardMap[row_adj][col_adj] == newStone)
                    my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        }
    }
        //If it is at the left or right edge, has 4 edges
        else if(input_col == 1 | input_col == SIZE)
        {
            row_adj = i - 1;
            col_adj = j;
            if(boardMap[row_adj][col_adj] == newStone)
                my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
            row_adj = i + 1;
            col_adj = j;
            if(boardMap[row_adj][col_adj] == newStone)
                my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
            //For Right edge look left
            if(input_col == SIZE)
            {
                col_adj = j - 1;
                for(row_adj = i - 1; row_adj <= i; row_adj++)
                    if(boardMap[row_adj][col_adj] == newStone)
                        my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
            }
            //For Left Edge look right
            else if(input_col == 1)
            {
                col_adj = j + 1;
                for(row_adj = i; row_adj <= i + 1; row_adj++)
                    if(boardMap[row_adj][col_adj] == newStone)
                        my_set.unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
            }
        }
    }
    
    //adjacent stones can be found in the 2D Matrix boardMap at:
    //row: i-1, cols: j, j+1
    //row: i, cols: j-1, j+1
    //row: i+1, cols: j, j-1
    else
    {
        //row: i-1, cols: j, j+1
        for(col_adj = j; col_adj < j + 2; col_adj ++)
        {
            row_adj = i - 1;
            if(boardMap[row_adj][col_adj] == newStone)
            {
                my_set.unionSets(i * SIZE + j, row_adj * SIZE + col_adj);
            }
        }
        //row: i, cols: j-1, j+1
        for(col_adj = j - 1 ; col_adj < j + 2; col_adj += 2)
        {
            row_adj = i;
            if(boardMap[row_adj][col_adj] == newStone)
            {
                my_set.unionSets(i * SIZE + j, row_adj * SIZE + col_adj);
            }
        }
        //row: i+1, cols: j-1, j
        for(col_adj = j - 1 ; col_adj <j + 1; col_adj++)
        {
            row_adj = i + 1;
            if(boardMap[row_adj][col_adj] == newStone)
            {
                my_set.unionSets(i * SIZE + j, row_adj * SIZE + col_adj);
            }
        }
    }
}

//Determine if the game is won
bool board::is_won (int input_row, int input_col)
{
    int i = input_row - 1;
    int j = input_col - 1;
    char newStone = boardMap[i][j]; 
    //If the player is Blue
    if(newStone == 'B')
    {
        //To store parents of left board edge
        vector<int> col1Parents;
        for(int count = 0; count <= SIZE - 1; count++)
        {
            //If the stone is Blue, then store its parent value
            if(boardMap[count][0] == 'B')
                col1Parents.push_back(my_set.findSet(count * SIZE));
        }
        //Compare parents of left board edge with each element in right board edge
        for(int count = 0; count <= SIZE - 1; count++)
        {
            if(boardMap[count][SIZE - 1] == 'B')
            {
                int currentHex = my_set.findSet(count * SIZE + SIZE - 1);
                for(auto& itr : col1Parents)
                {
                    if(itr == currentHex)
                        return true;
                }
            }  
        }                    
    }
    //If the player is Red
    if(newStone == 'R')
    {
        //To store parents of top board edge
        vector<int> row1Parents;
        for(int count = 0; count <= SIZE - 1; count++)
            //If that stone is Red, store its parent value
            if(boardMap[0][count] == 'R') 
            {
                row1Parents.push_back(my_set.findSet(count));
            }
        //Compare parents of top edge with each element in bottom edge
        for(int count = 0; count <= SIZE - 1; count++)
        {
            if(boardMap[SIZE - 1][count] == 'R')
            {
                int currentHex = my_set.findSet((SIZE-1) * SIZE + count);
                for(auto& itr : row1Parents)
                {
                    if(itr == currentHex)
                        return true;
                }
            }
            
        }                                  
    }
    return false;
}

//Play against the user, returns the move made
int board::play ()
{
    int rand_row, rand_col;
    //To assert that a play has been made
    bool done = false;
    while(done == false)
    {
        rand_row = distribution(e);
        rand_col = distribution(e);
        //If the randomly chosen spot is not empty, redo the loop
        if(boardMap[rand_row][rand_col] != '.') {continue;}
        else {boardMap[rand_row][rand_col] = 'R'; done = true;}    
    }
    return (rand_row * SIZE + rand_col);
}

//Function to print the board to the player
void board::display ()
{
    //Print out numbers of columns for user to count
    int t = 1;
    cout << string(6, ' ');
    for(int j = 1; j <= 2*SIZE-1; j++)
    {
        if(j % 2 == 0)
        {
            cout << " ";
        }
        else
        {
            if(t < 11)
                cout << "C" << "|" << t++;
            else
            {
                cout << "|" << "C" << t++;
            }
        }
    }
    cout << endl;
    //Each dot represents the center of a hex. Each dot has 6 edges!
    //For Each Row of the board 
    t = 1;
    for(int i = 1; i <= 2*SIZE-1; i++)
    {
        cout << string(7, ' ');
        if(i % 2 == 0)
        {   
            //For Each Column
            for(int j = 1; j <= 2*SIZE-1; j++)
            {   
                if(j % 2 == 0)
                    cout << " / ";
                else
                {
                    cout << "\\";
                } 
            }
        }
        else
        {
            for(int j = 1; j <= 2*SIZE-1; j++)
            {
                if(j % 2 == 0)
                {
                    cout << " - ";
                }
                else
                {
                    char hexValue = boardMap[(i - 1) / 2][(j - 1) / 2];
                    cout << hexValue;
                    if(j == 2 * SIZE - 1) {cout << " -R" << t++;}
                }
            }
        }
        //Shift the columns of each new row.
        string nspaces(i, ' ');
        cout << endl << nspaces;
    }
}

//Function to check whether player input is an integer
bool checkInt (string& input)
{
    for(int i = 0; i < input.length(); i++)
    {
        if(isdigit(input[i]) == false)
            return false;
    }
    return true;
}

int main()
{
    int row, col, i, j, count = 0; 
    //To store AI's play
    int ai_move, ai_move_row, ai_move_col;
    //To store user's input
    string input_col, input_row;
    //Initialize board
    board my_board(boardSize);
    bool endGame = false;
    
    while(endGame == false && count < boardSize * boardSize)
    {
        my_board.display(); 
        //Input from user for his next move
        cout << endl << "Specify the row number: ";;
        cin >> input_row;
        cout << endl << "Specify the column number: ";
        cin >> input_col;
        //Check that input is an integer
        if(checkInt(input_row) != true | checkInt(input_col) != true)
        {
            cout << endl << "Your input is not an integer, please specify an integer between 1 and " << boardSize << "\n\n";
            continue;
        }
        row = stoi(input_row);
        col = stoi(input_col);
        //Check that input within bounds of the board
        if(col < 1 | col > boardSize | row < 1 | row > boardSize) 
        {
            cout << endl << "Invalid: Your Input is out of bounds, please specify an integer between 1 and " << boardSize << "\n\n";
            continue;
        }
        cout << endl;
        i = row - 1;
        j = col - 1;
        if(my_board.stoneValue(row, col) == '.')
        {   
            count++;
            my_board.placeStone(row, col);
            my_board.connectStones(row, col);
            endGame = my_board.is_won(row, col);
            if(endGame == true) {my_board.display(); cout << endl << "You have won" << endl; break;}
        }
        else
        {
            cout << endl << "Invalid Input: This hex is already filled" << "\n\n";
            continue;
        }
        //Program will now play its move if there are spots left
        if(count < boardSize * boardSize)
        {
            count++;
            ai_move = my_board.play();
            ai_move_row = ai_move / boardSize;
            ai_move_col = ai_move - ai_move_row * boardSize;
            cout << "AI is playing..." << endl;
            my_board.connectStones(ai_move_row + 1, ai_move_col + 1);
            endGame = my_board.is_won(ai_move_row + 1, ai_move_col + 1);
            cout << endl << "AI played: " << ai_move_row + 1 << ", " << ai_move_col + 1 << "\n\n";
            if(endGame == true) {my_board.display(); cout << endl << "AI has won" << endl; break;}
        }
    }
    if(endGame == false) 
    {
        my_board.display();
        cout << endl << "The game is a tie" << endl; 
    }
}
