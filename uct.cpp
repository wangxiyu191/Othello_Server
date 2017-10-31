#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>
#include <algorithm>
#include <random>
#include <climits>
#include <cmath>
#include <limits>
#include "board.cpp"
using namespace std;
auto RNG = std::default_random_engine {};
class Node{
public:
    int wins = 0;
    int visits = 0;
    Node *parentNode;
    Position choose;
    
    vector<Position> unexamined;
    vector<Node*> children;
    int nowPlayer ;
    Node(Node *parentNode,Board &board,Position choose):parentNode(parentNode),choose(choose),nowPlayer(board.nowPlayer){
        unexamined = board.findPossibleChoose();
        std::shuffle(unexamined.begin(), unexamined.end(), RNG);
    }
    ~Node(){
        for(Node *child:children){
            delete child;
        }
    }
    Node* addBackChild(Board &board){
        Node *now = new Node(this,board,unexamined.back());
        unexamined.pop_back();
        children.push_back(now);
        return now;
    }
    Node* selectChild(){
        Node* maxNode = nullptr;
        double maxValue = -std::numeric_limits<double>::max();
        for(Node *child : children){
            double uctValue = 1.0*child->wins/child->visits + sqrt(2*log(visits)/child->visits);
            if(uctValue>maxValue){
                maxNode = child;
                maxValue = uctValue;
            }
        }
        return maxNode;
    }

    void update(int blackNum,int whiteNum){
        ++visits;
        wins += (nowPlayer==Board::BLACK) ? blackNum:whiteNum;
    }
    Node* mostVisitedChild(){
        Node *maxNode = children[0];
        for(Node *child: children){
            if(child->visits > maxNode->visits){
                maxNode = child;
            }
        }
        return maxNode;
    }
};

class UCT{
public:
    Position getNextAction(Board &board){
        Node root(nullptr,board,Position());
        int nodesVisited=0;
        for(int iter = 0;iter<12000;iter++){
            //printf("%d\n", iter);
            Node *now = &root;
            Board tmpBoard = board;
            /* Selection */
            while(now->unexamined.size() == 0 && !now->children.empty()){
                now = now->selectChild();
                tmpBoard.doChoose(now->choose);
            } 
            /* Expansion */
            if(now->unexamined.size() > 0){
                tmpBoard.doChoose(now->unexamined.back());
                now = now->addBackChild(tmpBoard);
            }
            /* Simulation */
            vector<Position> possibleChoose = tmpBoard.findPossibleChoose();
            while(!possibleChoose.empty()){
                tmpBoard.doChoose(possibleChoose[rand()%possibleChoose.size()]);
                ++nodesVisited;
                tmpBoard.updateGameState();
                possibleChoose = tmpBoard.findPossibleChoose();
            }
            /* Backpropagation */
            auto[isFull, whiteNum, blackNum] = tmpBoard.countBoard();
            while(now) {
                now->update(blackNum,whiteNum);
                now = now->parentNode;
            }

        }
        return root.mostVisitedChild()->choose;
    }
};