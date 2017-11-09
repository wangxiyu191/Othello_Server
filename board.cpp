
#include <cstring>
#include <iostream>
#include <vector>
#include "board.hpp"

Position::Position(int row, int col)
        : row(row), col(col) {
}

Position::Position()
        : row(0), col(0) {
}

inline Position Position::operator+=(const Position &rhs) {
    row += rhs.row;
    col += rhs.col;
    return *this;
}

bool Position::operator<(const Position &rhs) const {
    if (row < rhs.row)
        return true;
    if (rhs.row < row)
        return false;
    return col < rhs.col;
}

bool Position::operator>(const Position &rhs) const {
    return rhs < *this;
}

bool Position::operator<=(const Position &rhs) const {
    return !(rhs < *this);
}

bool Position::operator>=(const Position &rhs) const {
    return !(*this < rhs);
}


inline Position operator+(const Position &lhs, const Position &rhs) {
//    Position sum = lhs;
//    sum += rhs;
//    return sum;
    return Position(lhs.row+rhs.row,lhs.col+rhs.col);
}


Board::Board() {
    memset(&board, 0, sizeof(board));
    memset(&used, 0, sizeof(used));

    //board[3][3] = 0;
    used |= U64_1 << (3*8+3);
    //board[3][4] = 1;
    board |= U64_1 << (3*8+4);
    used |= U64_1 << (3*8+4);
    //board[4][3] = 1;
    board |= U64_1 << (4*8+3);
    used |= U64_1 << (4*8+3);
    //board[4][4] = 0;
    used |= U64_1 << (4*8+4);
    nowPlayer = BLACK;

}

Board::Board(const Board &rhs) {
    board = rhs.board;
    used = rhs.used;
    nowPlayer = rhs.nowPlayer;
    gameEnd = rhs.gameEnd;
}

inline int Board::readBoard(const Position &pos){
    if(used & (U64_1 << ( (pos.row)*8+(pos.col) ))){
        if(board & (U64_1 << ( (pos.row)*8+(pos.col) ))){
            return BLACK;
        }else{
            return WHITE;
        }
    }else{
        return EMPTY;
    }
}

inline void Board::setBoard(const Position &pos,int chess){
    if(chess == EMPTY) {
        used &= ~(U64_1 << ((pos.row ) * 8 + (pos.col )));
    }else{
        used |= U64_1 << ((pos.row ) * 8 + (pos.col ));
        if(chess == BLACK){
            board |= (U64_1 << ((pos.row) * 8 + (pos.col )));
        }else{
            board &= ~(U64_1 << ((pos.row ) * 8 + (pos.col )));
        }

    }
}

inline void Board::setEmpty(const Position &pos){
    used &= ~(U64_1 << ((pos.row ) * 8 + (pos.col )));
}

inline bool Board::isEmpty(const Position &pos){
    return  !(used & U64_1 << ( (pos.row)*8+(pos.col) ) );
}

//char &Board::operator[](Position pos) {
//    return board[pos.row][pos.col];
//}
//
//const char &Board::operator[](Position pos) const {
//    return board[pos.row][pos.col];
//}

Board &Board::operator=(const Board &rhs) {
    board = rhs.board;
    used = rhs.used;
    nowPlayer = rhs.nowPlayer;
    gameEnd = rhs.gameEnd;
    return *this;
}


std::tuple<bool, std::vector<Position>> Board::doChooseAnalysis(Position pos, bool requestChange) {

    //for speed
     thread_local static std::vector<Position> needToChange;
    needToChange.reserve(18);
    needToChange.clear();

    bool ableToChoose = false;
    if (!isEmpty(pos)) {
        return {ableToChoose, needToChange};
    }

    //for speed
     thread_local static std::vector<Position> others;
    others.reserve(9);
    others.clear();
    Position now;
    for (const Position &nowd : direction) {
        now.row = pos.row + nowd.row;
        now.col = pos.col + nowd.col;
        //printf("%d %d\n", pos.row, pos.col);
        if (requestChange) {
            others.clear();
        }
        bool hasOthers = false;
        while (now.row >= 0 && now.row < 8 && now.col >= 0 && now.col < 8) {

            int posState = readBoard(now);
            if ( posState == nowPlayer) {
                //find another self
                if (requestChange && !others.empty()) {
                    needToChange.insert(needToChange.end(), others.begin(), others.end());
                }
                if (!requestChange && hasOthers) {
                    ableToChoose = true;
                }
                break;
            } else if ( posState == !nowPlayer) {
                //find the other player
                if (requestChange) {
                    others.push_back(now);
                } else {
                    hasOthers = true;
                }
            } else {
                //find space
                break;
            }
            now.row = now.row + nowd.row;
            now.col = now.col + nowd.col;
        }
    }
    if (requestChange) {
        return {!needToChange.empty(), needToChange};
    } else {
        return {ableToChoose, needToChange};
    }

}

bool Board::isAbleToChoose(int row, int col) {
    return isAbleToChoose(Position(row, col));
}

bool Board::isAbleToChoose(Position pos) {
    auto[ableToChoose, needToChange] = doChooseAnalysis(pos, false);
    return ableToChoose;
}


std::tuple<bool, std::vector<Position>> Board::isAbleToChooseWithChange(Position pos) {
    return doChooseAnalysis(pos, true);
}

std::vector<Position> Board::findPossibleChoose() {
    //for speed
    thread_local static std::vector<Position> possibleChoose;
    possibleChoose.reserve(12);
    possibleChoose.clear();
    Position now(0, 0);
    for (now.row = 0; now.row < 8; now.row++) {
        for (now.col = 0; now.col < 8; now.col++) {
            if (!isEmpty(now)) {
                continue;
            }
            bool ableToChoose = isAbleToChoose(now);
            // for (Position p : needToChange) {
            //     printf("%d %d\n", p.row, p.col);
            // }
            if (ableToChoose) {
                possibleChoose.push_back(now);
            }
        }
    }
    return possibleChoose;
}

bool Board::doChoose(Position pos) {
    bool ableToChoose;
    std::vector<Position> needToChange;
    tie(ableToChoose, needToChange) = isAbleToChooseWithChange(pos);
    if (ableToChoose) {
        // lastFlipped.clear();
        for ( Position p : needToChange) {
            // lastFlipped.push_back(p);
            setBoard(p,nowPlayer);
        }
    } else {
        return false;
    }
    // lastChoose = pos;
    // isAbleToUndo = true;

    setBoard(pos,nowPlayer);
    nowPlayer = !nowPlayer;
    std::vector<Position> possibleChoose;
    possibleChoose  = findPossibleChoose();
    if (possibleChoose.empty()) {
        nowPlayer = !nowPlayer;
        possibleChoose = findPossibleChoose();
        if (possibleChoose.empty()) {
            nowPlayer = !nowPlayer;
            gameEnd = true;
        }
    }

     bool isFull;
    tie(isFull,std::ignore,std::ignore) = countBoard();
    gameEnd = gameEnd || isFull;

    return true;
}

// bool undoLastChoose(){
//     if(!isAbleToUndo){
//         return false;
//     }

//     for(Position p : lastFlipped){
//         (*this)[p] = nowPlayer; //被吃掉的棋子肯定是现在执子的
//     }
//     (*this)[lastChoose] = EMPTY;

//     nowPlayer = !nowPlayer;

//     isAbleToUndo = false;
//     gameEnd = false;

//     return true;
// }

std::tuple<bool, int, int> Board::countBoard() {
    //printBoard();
    int blackNum =  __builtin_popcountll(board&used);
    int whiteNum =  __builtin_popcountll( (~board)&used );
    //printf("%d,%d\n",whiteNum,blackNum);
//
//    for (now.row = 0; now.row < 8; now.row++) {
//        for (now.col = 0; now.col < 8; now.col++) {
//            if (isEmpty(now)) {
//                isFull = false;
//            }
//            else if (readBoard(now) == BLACK) {
//                blackNum++;
//            }
//            else if (readBoard(now) == WHITE) {
//                whiteNum++;
//            }
//        }
//    }
//
//    if(blackNum!= blackNum1){
//        printBoard();
//        printf("B %d %d\n",blackNum,blackNum1);
//        exit(1);
//    }
//    if(whiteNum!= whiteNum1){
//        printBoard();
//        printf("W %d %d\n",whiteNum,whiteNum1);
//        exit(1);
//    }
    return {whiteNum+blackNum == 8*8, whiteNum,blackNum};
}

void Board::printBoard() {
    Position now;
    for (now.row = 0; now.row < 8; now.row++) {
        for (now.col = 0; now.col < 8; now.col++) {
            if (isEmpty(now)) {
                printf("*");
            }
            else if (readBoard(now) == BLACK) {
                printf("1");
            }
            else if (readBoard(now) == WHITE) {
                printf("0");
            }
        }
        printf("\n");
    }
    fflush(stdout);
}

void Board::turnLeft(){
    Board new_board = *this;

    Position now;
    for (now.row = 0; now.row < 8; now.row++) {
        for (now.col = 0; now.col < 8; now.col++) {
            new_board.setBoard(Position(7-now.col,now.row),readBoard(now));
        }
    }
    *this = new_board;
}

void Board::inverse(){
    board = (~board)&used;
}

