#include "uct.hpp"


using namespace std;
thread_local auto RNG = std::default_random_engine {std::random_device{}()};

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

bool Node::operator<(const Node &rhs) const {
    return choose < rhs.choose;
}

bool Node::operator>(const Node &rhs) const {
    return rhs < *this;
}

bool Node::operator<=(const Node &rhs) const {
    return !(rhs < *this);
}

bool Node::operator>=(const Node &rhs) const {
    return !(*this < rhs);
}

void UCT::MCTSThread(Node *root,Board &board,int total_count){
    int nodesVisited = 0;
    for (int iter = 0; iter < total_count; iter++) {
        //if(iter%100==0) fprintf(stderr,"%d\n",iter );
        //printf("%d\n", iter);
        Node *now;
        now = root;
        Board tmpBoard;
        tmpBoard = board;
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

        int gameResult;
        gameResult = FullSearch::TIE;
        int winner;
        winner = -1;

        while (!tmpBoard.gameEnd) {
            //printf("%d %p\n",this_thread::get_id(),&tmpBoard);
            int whiteNum,blackNum;
            tie(ignore, whiteNum, blackNum) = tmpBoard.countBoard();

            if (whiteNum + blackNum > 8 * 8 - 8) {
                gameResult = fullsearch.getGameResult(tmpBoard);
                if(gameResult==FullSearch::TIE) {
                    break;
                } else if(gameResult == FullSearch::WIN && tmpBoard.nowPlayer == Board::BLACK){
                    winner = Board::BLACK;
                }else if(gameResult == FullSearch::LOST && tmpBoard.nowPlayer == Board::WHITE){
                    winner = Board::BLACK;
                }else{
                    winner = Board::WHITE;
                }
                // printf("Winner:%d\n",winner);
                break;

            }

            vector<Position> possibleChoose = tmpBoard.findPossibleChoose();


            uniform_int_distribution<int> dis(0, static_cast<int>(possibleChoose.size() - 1));

            //printf("%d %d %p\n",this_thread::get_id(),possibleChoose.size(),&possibleChoose);
            //tmpBoard.printBoard();
            int random_num = dis(RNG);
            tmpBoard.doChoose(possibleChoose[random_num]);
            //tmpBoard.printBoard();
            //printf("[]%d %d %p\n",this_thread::get_id(),possibleChoose.size(),&possibleChoose);

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
}

Position UCT::getNextAction(Board &board) {
    //auto[_, tw, tb] = board.countBoard();
    const int THREAD_NUM = 2;

    Node *roots[10];
    thread *threads[10];
    for(int thread_id = 0 ;thread_id < THREAD_NUM ;thread_id++){
        roots[thread_id] = new Node(nullptr, board, Position(), -1);
        threads[thread_id] = new thread(&UCT::MCTSThread,this,roots[thread_id],ref(board),6000);
    }

    for(int thread_id = 0 ;thread_id < THREAD_NUM ;thread_id++) {
        threads[thread_id]->join();
    }


    sort(roots[0]->children.begin(),roots[0]->children.end(),[](Node *a, Node *b) -> bool {
        return a->choose<b->choose;
    });

    for(int thread_id = 1 ;thread_id < THREAD_NUM ;thread_id++) {
        sort(roots[thread_id]->children.begin(),roots[thread_id]->children.end(),[](Node *a, Node *b) -> bool {
            return a->choose<b->choose;
        });
        for(int i=0;i<roots[thread_id]->children.size();i++){
            roots[0]->children[i]->wins   += roots[thread_id]->children[i]->wins;
            roots[0]->children[i]->visits += roots[thread_id]->children[i]->visits;
//            printf("Child%d[%d,%d]  %lf/%d=%lf\n",thread_id,roots[thread_id]->children[i]->choose.row
//                    ,roots[thread_id]->children[i]->choose.col
//                    ,roots[thread_id]->children[i]->wins
//                    ,roots[thread_id]->children[i]->visits
//                    ,roots[thread_id]->children[i]->wins/ roots[thread_id]->children[i]->visits);

        }
    }

    Position finalChoose = roots[0]->mostVisitedChild()->choose;
//    for(auto child : roots[0]->children){
//        fprintf(stderr, "player:%d\n", child->nowPlayer );
//    }

    for(int thread_id = 0 ;thread_id < THREAD_NUM ;thread_id++) {
        delete(roots[thread_id]);
        delete(threads[thread_id]);
    }



    return finalChoose;
}

