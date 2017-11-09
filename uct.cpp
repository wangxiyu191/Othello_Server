#include "uct.hpp"


using namespace std;
auto RNG = std::default_random_engine {std::random_device{}()};


Node::Node(Node *parentNode, Board &board, Position choose, int player) : parentNode(parentNode), choose(choose),
                                                                          nowPlayer(player) {
    unexamined = board.findPossibleChoose();
    std::shuffle(unexamined.begin(), unexamined.end(), RNG);
}

Node::~Node() {
    for (Node *child:children) {
        delete child;
    }
}

Node *Node::addBackChild(Board &board, int player) {
    Node *now = new Node(this, board, unexamined.back(), player);
    unexamined.pop_back();
    children.push_back(now);
    return now;
}

Node *Node::selectChild() {
    Node *maxNode = nullptr;
    //fprintf(stderr, "in\n");
    double maxValue = -std::numeric_limits<double>::max();
    for (Node *child : children) {
        double uctValue = child->wins / child->visits + sqrt(1.96 * log10(visits) / child->visits);
        //fprintf(stderr, "%s\n",nowPlayer == Board::BLACK?"black":"white" );
        //fprintf(stderr,"%lf=%lf+%lf\n",uctValue,child->wins/child->visits,sqrt(1.96*log(visits)/child->visits) );
        if (uctValue > maxValue) {
            maxNode = child;
            maxValue = uctValue;
        }
    }
    //fprintf(stderr, "out\n");
    //fprintf(stderr,"%lf",maxValue);
    return maxNode;
}

void Node::update(int blackNum, int whiteNum) {
    ++visits;
    //fprintf(stderr,"====%d %d====\n",blackNum,whiteNum );
    if (blackNum > whiteNum && nowPlayer == Board::BLACK) {
        //fprintf(stderr,"black\n");
        wins++;
    }
    if (blackNum < whiteNum && nowPlayer == Board::WHITE) {
        //fprintf(stderr,"white\n");
        wins++;
    }
    if (blackNum == whiteNum) {
        //fprintf(stderr,"tie\n");
        wins += 0.5;
    }
}

Node *Node::mostVisitedChild() {
    Node *maxNode = children[0];
    for (Node *child: children) {
        fprintf(stderr, "[%d,%d] %lf/%d=%lf\n", child->choose.row, child->choose.col, child->wins, child->visits, child->wins/child->visits);
        if (child->visits > maxNode->visits) {
            maxNode = child;
        }
    }
    fprintf(stderr, "Posibilty:%lf\n", 1.0 * maxNode->wins / maxNode->visits);
    return maxNode;
}


Position UCT::getNextAction(Board &board) {

    //auto[_, tw, tb] = board.countBoard();
    Node root(nullptr, board, Position(), -1);



    int nodesVisited = 0;
    for (int iter = 0; iter < 1200; iter++) {
        //if(iter%100==0) fprintf(stderr,"%d\n",iter );
        //printf("%d\n", iter);
        Node *now = &root;
        Board tmpBoard = board;
        /* Selection */
        while (now->unexamined.empty() && !now->children.empty()) {
            now = now->selectChild();
            tmpBoard.doChoose(now->choose);
        }
        /* Expansion */
        if (!now->unexamined.empty()) {
            int player = tmpBoard.nowPlayer;
            tmpBoard.doChoose(now->unexamined.back());
            now = now->addBackChild(tmpBoard, player);
        }
        /* Simulation */

        int gameResult = FullSearch::TIE;
        int winner = -1;

        while (!tmpBoard.gameEnd) {
            int whiteNum,blackNum;
            tie(ignore, whiteNum, blackNum) = tmpBoard.countBoard();

            if (whiteNum + blackNum > 8 * 8 - 4 - 3) {
                tmpBoard.printBoard();
                gameResult = fullsearch.getGameResult(tmpBoard);
                printf("End:%d\n",gameResult);
                if(gameResult==FullSearch::TIE) {
                    break;
                } else if(gameResult == FullSearch::WIN && tmpBoard.nowPlayer == Board::BLACK){
                    winner = Board::BLACK;
                }else if(gameResult == FullSearch::LOST && tmpBoard.nowPlayer == Board::WHITE){
                    winner = Board::BLACK;
                }else{
                    winner = Board::WHITE;
                }
                break;

            }

            vector<Position> possibleChoose = tmpBoard.findPossibleChoose();
            tmpBoard.doChoose(possibleChoose[rand() % possibleChoose.size()]);

            ++nodesVisited;
        }
        /* Backpropagation */
        int whiteNum,blackNum;
        if(gameResult == FullSearch::TIE){
            blackNum = 1;
            whiteNum = 1;
        }else{
            if(winner == Board::BLACK){
                blackNum = 1;
                whiteNum = 0;
            }else{
                blackNum = 0;
                whiteNum = 1;
            }
        }


        // if( (64-tw-tb)<4 ){
        //     fprintf(stderr,"w:%d b:%d\n",  whiteNum, blackNum);
        //     tmpBoard.printBoard();
        // }
        while (now) {
            now->update(blackNum, whiteNum);
            now = now->parentNode;
        }
    }
    fprintf(stderr, "Node Count:%d\n", nodesVisited);
    // for(auto child : root.children){
    //     fprintf(stderr, "player:%d\n", child->nowPlayer );
    // }
    return root.mostVisitedChild()->choose;
}
