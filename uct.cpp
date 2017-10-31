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
    double wins = 0;
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
            double uctValue = child->wins/child->visits + sqrt(1.96*log10(visits)/child->visits);
            //fprintf(stderr,"%lf=%lf+%lf\n",uctValue,child->wins/child->visits,sqrt(1.96*log(visits)/child->visits) );
            if(uctValue>maxValue){
                maxNode = child;
                maxValue = uctValue;
            }
        }
        //fprintf(stderr,"%lf",maxValue);
        return maxNode;
    }

    void update(int blackNum,int whiteNum){
        ++visits;
        //fprintf(stderr,"====%d %d====\n",blackNum,whiteNum );
        if(blackNum < whiteNum  && nowPlayer==Board::BLACK){
            //fprintf(stderr,"black");
            wins++;
        }
        if(blackNum > whiteNum  && nowPlayer==Board::WHITH){
            //fprintf(stderr,"white");
            wins++;
        }
        if(blackNum == whiteNum){
            //fprintf(stderr,"tie");
            wins+=0.5;
        }
    }
    Node* mostVisitedChild(){
        Node *maxNode = children[0];
        for(Node *child: children){
            fprintf(stderr,"%lf/%d\n", child->wins,child ->visits);
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
        //auto[_, tw, tb] = board.countBoard();
        Node root(nullptr,board,Position());
        int nodesVisited=0;
        for(int iter = 0;iter<1200;iter++){
            //if(iter%100==0) fprintf(stderr,"%d\n",iter );
            //printf("%d\n", iter);
            Node *now = &root;
            Board tmpBoard = board;
            /* Selection */
            while(now->unexamined.empty() && !now->children.empty()){
                now = now->selectChild();
                tmpBoard.doChoose(now->choose);
            } 
            /* Expansion */
            if(!now->unexamined.empty()){
                tmpBoard.doChoose(now->unexamined.back());
                now = now->addBackChild(tmpBoard);
            }
            /* Simulation */
            vector<Position> possibleChoose = tmpBoard.findPossibleChoose();
            while(!possibleChoose.empty()){
                tmpBoard.doChoose(possibleChoose[rand()%possibleChoose.size()]);
                ++nodesVisited;
                possibleChoose = tmpBoard.findPossibleChoose();
            }
            /* Backpropagation */
            auto[isFull, whiteNum, blackNum] = tmpBoard.countBoard();
            
            // if( (64-tw-tb)<4 ){
            //     fprintf(stderr,"w:%d b:%d\n",  whiteNum, blackNum);
            //     tmpBoard.printBoard();
            // }
            while(now) {
                now->update(blackNum,whiteNum);
                now = now->parentNode;
            }
        }
        fprintf(stderr,"Node Count:%d\n", nodesVisited);
        return root.mostVisitedChild()->choose;
    }
};