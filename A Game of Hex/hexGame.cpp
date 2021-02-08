//A Game of Hex, with an AI using Monte Carlo simulations
//Author: Omar Safwat
//Date: 8th of February, 2021
#include<iostream>
#include<iterator>
#include<vector>
#include<string>
#include<utility>   //For pair<>
#include<algorithm>
#include<random>
#include<ctime>    
#include<stdlib.h> //For Clearing the screen after every move
using namespace std;

const unsigned short SIZE = 11;
const unsigned short NSIM = 1000;
//Random seed for the move by the program
default_random_engine e(time(nullptr));
uniform_int_distribution<int> distribution(0, SIZE - 1);

//Define the board
class Board
{
    public:
    Board() : board(SIZE, vector<char>(SIZE, '.')), emptyHex(SIZE*SIZE) 
    {
        for(unsigned short i = 0; i < emptyHex.size(); i++)
            emptyHex[i] = i;
    }

    //Method to place a stone on the board
    void placeStone (unsigned short input_row, unsigned short input_col) {board[input_row - 1][input_col - 1] = 'B';}
    //Get stone value occupying a certain position
    char stoneValue (unsigned short input_row, unsigned short input_col) {return board[input_row - 1][input_col - 1];}
    //Display board
    void display ();

    //Check to see if new stone is on one of the four corners
    bool isTopRight (unsigned short input_row, unsigned short input_col) {return (input_row == 1 && input_col == SIZE) ? true : false;}
    bool isTopleft (unsigned short input_row, unsigned short input_col) {return (input_row == 1 && input_col == 1) ? true : false;}
    bool isLowRight (unsigned short input_row, unsigned short input_col) {return (input_row == SIZE && input_col == SIZE) ? true : false;}
    bool isLowleft (unsigned short input_row, unsigned short input_col) {return (input_row == SIZE && input_col == 1) ? true : false;}
    //Check for which edge
    bool isTopEdge (unsigned short input_row) {return (input_row == 1) ? true : false;}
    bool isLowEdge (unsigned short input_row) {return (input_row == SIZE) ? true : false;}
    bool isRightEdge (unsigned short input_col) {return (input_col == SIZE) ? true : false;}
    bool isLeftEdge (unsigned short input_col) {return (input_col == 1) ? true : false;}
    bool isSideEdge (unsigned short input_col) {return (isLeftEdge(input_col) == true || isRightEdge(input_col) == true) ? true : false;}
    bool isTop_Low (unsigned short input_row) {return (isTopEdge(input_row) == true || isLowEdge(input_row) == true) ? true : false;}

    protected:
    //board: '.' for empty hex, 'B' for Blue hex, 'R' for Red hex
    vector<vector<char>> board;
    //Vector keeps track of locations of empty hexes on the board. 
    vector<unsigned short> emptyHex;
};

//A Disjoint "union find" set routine
class DisjSet : public Board
{
    public:
    //Create a set of n items "Number of Hexes available on the board" 
    DisjSet() : Board(), treeSize(SIZE * SIZE), parent(SIZE * SIZE) {makeSet();}

    //create a tree for each hex on the board
    void makeSet()
    {
        for (unsigned short i = 0; i < parent.size(); i++)
        {
            parent[i] = i;
            treeSize[i] = 1;
        }
    }
    unsigned short findSet(unsigned short v)
    {
        if (v == parent[v])
            return v;  
        return parent[v] = findSet(parent[v]);
    }
    void unionSets(unsigned short adj, unsigned short newNode);

    protected:
    vector<unsigned short> treeSize, parent;
};

//CLass to manange the game
class HexGame : public DisjSet
{
    public:
    HexGame() : DisjSet() {}
    //A copy constructor
    HexGame(const HexGame& game1) : DisjSet()
    {
        this -> board = game1.board;
        this -> parent = game1.parent;
        this -> emptyHex = game1.emptyHex;
        this -> treeSize = game1.treeSize;
        this -> parent = game1.parent;
    }
    //Connect adjacet stones to form a bridge
    void connectStones (unsigned short input_row, unsigned short input_col);
    //Check if either player won
    char is_won (unsigned short input_row, unsigned short input_col, char player);
    unsigned short AI (HexGame& game);
    //Erase filled hexes from vector emptyHex
    void eraseFilled (unsigned short input_row, unsigned short input_col)
    {
        unsigned short i = input_row - 1, j = input_col - 1;
        //Find the location of the new placed hex in emptyHex
        pair<vector<unsigned short>::iterator, vector<unsigned short>::iterator> sub_range = equal_range(emptyHex.begin(), emptyHex.end(), i * SIZE + j);
        //Remove the new filled position from emptyHex vector
        emptyHex.erase(sub_range.first);
    }
};

void DisjSet::unionSets(unsigned short adj, unsigned short newNode)
{
    //Map them back to rows and columns With a +1
    unsigned short adj_row = adj / SIZE;
    unsigned short adj_col = (adj - adj_row++ * SIZE) + 1;
    unsigned short new_row = newNode / SIZE;
    unsigned short new_col = newNode - new_row * SIZE;
    char player = board[new_row++][new_col++];

    //Find parents of each node
    unsigned short root_adj = findSet(adj);
    unsigned short root_adjRow = root_adj / SIZE;
    unsigned short root_adjCol = (root_adj - root_adjRow++ * SIZE) + 1;
    
    unsigned short root_new = findSet(newNode);
    unsigned short root_newRow = root_new / SIZE;
    unsigned short root_newCol = (root_new - root_newRow++ * SIZE) + 1; 
    
    //cout << "adj_row: " << adj_row << "\nadj_col: " << adj_col << "\nnew_row" << new_row << "\nnew_col" << new_col << "newNode: " << newNode << "\nisLeftEdge: " << isLeftEdge(new_col) << "\nnotSideEdge: " << notSideEdge(root_adjCol); 
    //A hex on the edge has priority of being the parent
    //If newNode is Blue, on either of the left or right edges, it should be the parent
    if(player == 'B' && isSideEdge(root_newCol) == true && isSideEdge(root_adjCol) == false) 
    {
        parent[root_adj] = root_new; 
        //cout << "Parent" <<"[" << root_adj << "]: " << root_new << endl;
        treeSize[root_new] += treeSize[root_adj]; 
        return;
    }
    //If adj node was Blue and on either left or right edges, it should be the parent
    if(player == 'B' && isSideEdge(root_adjCol) == true && isSideEdge(root_newCol) == false) 
    {
        parent[root_new] = root_adj; 
        //cout << "Parent" <<"[" << root_new << "]: " << root_adj << endl;
        treeSize[root_adj] += treeSize[root_new]; 
        return;
    }
   
    //If newNode is Red, on either of Top or bottom edges, it should be the parent
    if(player == 'R' && isTop_Low(root_newRow) == true && isTop_Low(root_adjRow) == false) 
    {
        parent[root_adj] = root_new; 
        //cout << "Parent" <<"[" << root_adj << "]: " << root_new << endl;
        treeSize[root_new] += treeSize[root_adj]; 
        return;
    }
    //if adj node is Red on either top or bottom edges, it should be parent
    if(player == 'R' && isTop_Low(root_adjRow) == true && isTop_Low(root_newRow) == false) 
    {
        parent[root_new] = root_adj; 
        //cout << "Parent" <<"[" << root_new << "]: " << root_adj << endl;
        treeSize[root_adj] += treeSize[root_new]; 
        return;
    }
    
    //If none of the above qualifies, join sets according to size
    if(treeSize[root_adj] < treeSize[root_new])
    {
        parent[root_adj] = root_new;
        treeSize[root_new] += treeSize[root_adj];
        //cout << "Parent" <<"[" << root_adj << "]: " << root_new << endl;
    }
    else
    {
        parent[root_new] = root_adj;
        treeSize[root_adj] += treeSize[root_new];
        //cout << "Parent" <<"[" << root_new << "]: " << root_adj << endl;
    }
}

//Function to print the board to the player
void Board::display ()
{
    //Print out numbers of columns for user to validMove
    unsigned short t = 1;
    cout << string(6, ' ');
    for(unsigned short j = 1; j <= 2*SIZE-1; j++)
    {
        if(j % 2 == 0)
        {
            cout << " ";
        }
        else
        {
            if(t < SIZE)
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
    for(unsigned short i = 1; i <= 2*SIZE-1; i++)
    {
        cout << string(7, ' ');
        if(i % 2 == 0)
        {   
            //For Each Column
            for(unsigned short j = 1; j <= 2*SIZE-1; j++)
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
            for(unsigned short j = 1; j <= 2*SIZE-1; j++)
            {
                if(j % 2 == 0)
                {
                    cout << " - ";
                }
                else
                {
                    char hexValue = board[(i - 1) / 2][(j - 1) / 2];
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

//Associate stones on the board of the same colour to make a path with DisjSet.
void HexGame::connectStones (unsigned short input_row, unsigned short input_col)
{
    //let (i,j) be the matrix coordinates of the new hex input by the player
    //subtract an additional 1, because C++ starts index from 0
    unsigned short i = input_row - 1, j = input_col - 1;
    char newStone = board[i][j];
    //Column and row of adjacent hex to connect with.
    unsigned short row_adj, col_adj;
    
    //First check if the node is on one of the corners
    if(isTopleft(input_row, input_col) == true)
    {
        row_adj = i + 1;
        col_adj = j;
        if(board[row_adj][col_adj] == newStone)
            unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        row_adj = i;
        col_adj = j + 1;
        if(board[row_adj][col_adj] == newStone)
            unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        return;
    }
    if(isTopRight(input_row, input_col) == true)
    {
        row_adj = i;
        col_adj = j - 1;
        if(board[row_adj][col_adj] == newStone)
            unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        row_adj = i + 1;
        for(col_adj = j - 1; col_adj <= j; col_adj++)
            if(board[row_adj][col_adj] == newStone)
                unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        return;
    }
    if(isLowleft(input_row, input_col) == true)
    {
        row_adj = i;
        col_adj = j + 1;
        if(board[row_adj][col_adj] == newStone)
            unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        row_adj = i - 1;
        for(col_adj = j; col_adj <= j + 1; col_adj++)
            if(board[row_adj][col_adj] == newStone)
                unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        return;
    }
    if(isLowRight(input_row, input_col) == true)
    {
        row_adj = i - 1;
        col_adj = j;
        if(board[row_adj][col_adj] == newStone)
            unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        row_adj = i;
        col_adj = j - 1;
        if(board[row_adj][col_adj] == newStone)
            unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        return; 
    }
    
    //If the new stone is at the edges of the board
    if(isTopEdge(input_row) == true)
    {
        row_adj = i;
        for(col_adj = j - 1; col_adj <= j + 1; col_adj += 2)
            if(board[row_adj][col_adj] == newStone)
                unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        row_adj = i + 1;
        for(col_adj = j - 1; col_adj <= j; col_adj++)
            if(board[row_adj][col_adj] == newStone)
                unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        return;
    }
    if(isLowEdge(input_row) == true)
    {
        row_adj = i;
        for(col_adj = j - 1; col_adj <= j + 1; col_adj += 2)
            if(board[row_adj][col_adj] == newStone)
                unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        row_adj = i - 1;
        for(col_adj = j; col_adj <= j + 1; col_adj++)
            if(board[row_adj][col_adj] == newStone)
                unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        return;
    }
    if(isSideEdge(input_col) == true)
    {
        row_adj = i - 1;
        col_adj = j;
        if(board[row_adj][col_adj] == newStone)
            unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        row_adj = i + 1;
        col_adj = j;
        if(board[row_adj][col_adj] == newStone)
            unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        //For Right edge look left
        if(isRightEdge(input_col) == true)
        {
            col_adj = j - 1;
            for(row_adj = i - 1; row_adj <= i; row_adj++)
                if(board[row_adj][col_adj] == newStone)
                    unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
            return;
        }
        //For Left Edge look right
        else if(isLeftEdge(input_col) == true)
        {
            col_adj = j + 1;
            for(row_adj = i; row_adj <= i + 1; row_adj++)
                if(board[row_adj][col_adj] == newStone)
                    unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
            return;
        }
    }
    
    //If newStone is in the middle of the board 
    //adjacent stones can be found in the 2D Matrix board at:
    //row: i-1, cols: j, j+1
    for(col_adj = j; col_adj < j + 2; col_adj ++)
    {
        row_adj = i - 1;
        if(board[row_adj][col_adj] == newStone)
        {
            unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        }
    }
    //row: i, cols: j-1, j+1
    for(col_adj = j - 1 ; col_adj < j + 2; col_adj += 2)
    {
        row_adj = i;
        if(board[row_adj][col_adj] == newStone)
        {
            unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
        }
    }
    //row: i+1, cols: j-1, j
    for(col_adj = j - 1 ; col_adj <j + 1; col_adj++)
        {
            row_adj = i + 1;
            if(board[row_adj][col_adj] == newStone)
            {
                unionSets(row_adj * SIZE + col_adj, i * SIZE + j);
            }
        }
}

//Determine if the game is won. Returns with the char of the winner
char HexGame::is_won (unsigned short input_row, unsigned short input_col, char player = 'n')
{
    unsigned short i = input_row - 1;
    unsigned short j = input_col - 1;
    //If the player is not prespecified, go find it
    char newStone = (player == 'n') ? board[i][j] : player;

    //If the player is Blue
    if(newStone == 'B')
    {
        //Check parents of all BLUE elements on the left edge
        for(unsigned short elem = 0; elem < SIZE; elem++)
        {
            if(board[elem][0] == 'B')
            {
                unsigned short parent = findSet(elem * SIZE);
                unsigned short parent_row = parent / SIZE;
                unsigned short parent_col = parent - parent_row * SIZE;
                //If parent is on the right edge return true
                if(parent_col == SIZE - 1) {return 'B';}
            }
        } 
        //Check parents of all BLUE elements on the right row                                 
        for(unsigned short elem = 0; elem < SIZE; elem++)
        {
            if(board[elem][SIZE - 1] == 'B')
            {
                unsigned short parent = findSet(elem * SIZE + (SIZE - 1));
                unsigned short parent_row = parent / SIZE;
                unsigned short parent_col = parent - parent_row * SIZE;
                //If parent is on the left edge return true
                if(parent_col == 0) {return 'B';}
            } 
        }
    }
    //If the player is Red
    if(newStone == 'R')
    {   
        //Check parents of all RED elements in the top row
        for(unsigned short elem = 0; elem < SIZE; elem++)
        {
            if(board[0][elem] == 'R')
            {
                unsigned short parent = findSet(elem);
                unsigned short parent_row = parent / SIZE;
                //If parent is on the bottom edge return true
                if(parent_row == SIZE - 1) {return 'R';}
            }
        } 
        //Check parents of all RED elements in the bottom row                                 
        for(unsigned short elem = 0; elem < SIZE; elem++)
        {
            if(board[SIZE - 1][elem] == 'R')
            {
                unsigned short parent = findSet((SIZE - 1) * SIZE + elem);
                unsigned short parent_row = parent / SIZE;
                //If parent is on the top edge return true
                if(parent_row == 0) {return 'R';}
            }
        }
    }
    return 'n';
}

//AI using Monte Carlo Simulations
unsigned short HexGame::AI (HexGame& game)
{
    //A Copy version of the game (With new Board and DisjSet ) for 1st loop of simulation
    HexGame gameCpy1(game);
    //A 3rd Copy of HexGame to be used in 2nd loop of simulation
    HexGame gameCpy2(game);
    //A pair to store win/lose ratio for every fixed stone move
    vector<pair<double, unsigned short>> ratio;

    //Number of Blue plays remaining
    unsigned short B_plays = gameCpy1.emptyHex.size() / 2;
    //Subtract by 1, because of the fixedValue
    unsigned short R_plays = gameCpy1.emptyHex.size() - B_plays - 1;

    //simulations represents the random choices for 'R' and 'B', after being shuffled.
    vector<char> simulations(B_plays, 'B');
    simulations.insert(simulations.end(), R_plays, 'R');

    //for(unsigned short k = 0; k < simulations.size(); k++) {cout << simulations[k] << ", ";}
    //cout << endl;

    //For every valid move on the board
    vector<unsigned short>::iterator iterFix = gameCpy1.emptyHex.begin();
    for(unsigned short validMove = 0; validMove < emptyHex.size(); validMove++, iterFix++)
    {
        //Restore DisjSet of gameCpy1 back to the original game
        gameCpy1.parent = game.parent;
        gameCpy1.treeSize = game.treeSize;

        //cout << "validMove: " << validMove << endl;
        unsigned short win = 0;
        //Fix the AI first move on this position:
        unsigned short fixHex = *iterFix;
        unsigned short fix_row = fixHex / SIZE;
        unsigned short fix_col = fixHex - fix_row * SIZE;

        gameCpy1.board[fix_row][fix_col] = 'R';
        gameCpy2.board[fix_row][fix_col] = 'R';
        gameCpy1.connectStones(fix_row + 1, fix_col + 1);

        //Perform NSIM simulations for every valid position
        for(unsigned short sim = 0; sim < NSIM; sim++)
        {
            //Restore gameCpy2 DisjSet back to DisjSet of gameCpy1
            gameCpy2.parent = gameCpy1.parent;
            gameCpy2.treeSize = gameCpy1.treeSize;

            //Make a random move using the shuffled vector
            random_shuffle(simulations.begin(), simulations.end(), [](int i) {return (i * distribution(e)) % SIZE;});
            for(unsigned short index = 0, index2 = 0; index < simulations.size(); index++, index2++)
            {
                unsigned short mappedValue;
                //Map the random player move with an empty Hex
                if(gameCpy2.emptyHex[index2] == fixHex) {mappedValue = gameCpy2.emptyHex[++index2];}
                else {mappedValue = gameCpy2.emptyHex[index2];}
                unsigned short row = mappedValue / SIZE;
                unsigned short col = mappedValue - row * SIZE;
                //Assign the random move to the empty Hex
                gameCpy2.board[row][col] = simulations[index];
                /*if(sim > 4) {gameCpy2.display();
                cout << "Move of '" << simulations[index] << "':\t" << row << "," << col << "\n\n";}*/
                gameCpy2.connectStones(row + 1, col + 1);
            }
            if(gameCpy2.is_won(2,2, 'R') == 'R') 
                win++;
            //clean up the randomly filled positions for the next simulation
            for(unsigned short index = 0, index2 = 0; index < simulations.size(); index++, index2++)
            {
                unsigned short mappedValue;
                //Map the random player move with an empty Hex
                if(gameCpy2.emptyHex[index2] == fixHex) {mappedValue = gameCpy2.emptyHex[++index2];}
                else {mappedValue = gameCpy2.emptyHex[index2];}
                unsigned short row = mappedValue / SIZE;
                unsigned short col = mappedValue - row * SIZE;
                //Restore each random move to an empty Hex
                gameCpy2.board[row][col] = '.';
            }
        }
        
        ratio.push_back(make_pair(static_cast<double> (win) / NSIM, fixHex)); 
        //cout << "\nWin: " << win << endl;
        
        //Replace the 'R' with a '.', before switching to another fixHex value
        gameCpy1.board[fix_row][fix_col] = '.';
        gameCpy2.board[fix_row][fix_col] = '.';
    }

    //Sort the vector in descending order. The node with highest ratio is the first one
    sort(ratio.rbegin(), ratio.rend());
    unsigned short bestMove = ratio[0].second;
    unsigned short bestMove_row = bestMove / SIZE;
    unsigned short bestMove_col = bestMove - bestMove_row * SIZE;
    //Implement the new move
    game.board[bestMove_row][bestMove_col] = 'R';
    game.eraseFilled(bestMove_row + 1, bestMove_col + 1);
    game.connectStones(bestMove_row + 1, bestMove_col + 1);
    return(bestMove);
}

//Function to check whether player input is an integer
bool checkInt (string& input)
{
    for(unsigned short i = 0; i < input.length(); i++)
    {
        if(isdigit(input[i]) == false)
            return false;
    }
    return true;
}

int main()
{
    HexGame game;
    unsigned short row, col, i, j, validMove = 0; 
    //To store AI's play
    unsigned short ai_move, ai_move_row, ai_move_col;
    //To store user's input
    string input_col, input_row;
    char endGame = 'n';
    cout << "\t\t\t\tA GAME OF HEX";
    cout << "\n\t\t\t\t=============";
    cout << "\n\n\t\tWelcome to my Hex Game, I hope you enjoy playing against my AI.";
    cout << "\n\n1- You'll be playing with the BLUE stones, symboled as \"B\" and the AI as the RED stones, symboled with \"R\".";
    cout << "\n2- You'll get to be the first player because the AI is really tough.";
    cout <<"\n3- I've labeled the rows and columns for you to help you navigate the board. When asked for input make sure you specify your row and column of choice as an integer (That's a whole number for non-programmers).";
    cout << "\nEnjoy!\n\n";
    while(endGame == 'n' && validMove < SIZE * SIZE)
    {
        game.display();
        //Input from user for his next move
        cout << endl << "Specify the row number: ";;
        cin >> input_row;
        cout << endl << "Specify the column number: ";
        cin >> input_col;
        //Check that input is an integer
        if(checkInt(input_row) != true || checkInt(input_col) != true)
        {
            cout << endl << "Your input is not an integer, please specify an integer between 1 and " << SIZE << "\n\n";
            system("CLS");
            continue;
        }
        row = stoi(input_row);
        col = stoi(input_col);
        //Check that input within bounds of the board
        if(col < 1 || col > SIZE || row < 1 || row > SIZE) 
        {
            cout << endl << "Invalid: Your Input is out of bounds, please specify an integer between 1 and " << SIZE << "\n\n";
            system("CLS");
            continue;
        }
        cout << endl;
        i = row - 1;
        j = col - 1;
        if(game.stoneValue(row, col) == '.')
        {   
            validMove++;
            game.placeStone(row, col);
            game.eraseFilled(row, col);
            game.connectStones(row, col);
            endGame = game.is_won(row, col);
            if(endGame == 'B') {game.display(); cout << endl << "You have won" << endl; break;}
        }
        else
        {
            cout << endl << "Invalid Input: This hex is already filled" << "\n\n";
            system("CLS");
            continue;
        }
        //Program will now play its move if there are spots left
        if(validMove < SIZE * SIZE)
        {
            validMove++;
            cout << "AI is playing...";
            ai_move = game.AI(game);
            ai_move_row = ai_move / SIZE;
            ai_move_col = ai_move - ai_move_row * SIZE;
            endGame = game.is_won(ai_move_row + 1, ai_move_col + 1);
            system("CLS");
            cout << endl << "AI played: " << ai_move_row + 1 << ", " << ai_move_col + 1 << "\n\n";
            if(endGame == 'R') {game.display(); cout << endl << "AI has won" << endl; break;}
        }
    }
    if(endGame == 'n') 
    {
        game.display();
        cout << endl << "The game is a tie" << endl; 
    }
}
