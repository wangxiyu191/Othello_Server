#pragma once
#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>
class Position {
public:
    Position(int row, int col)
        : row(row)
        , col(col)
    {
    }
    Position()
        : row(0)
        , col(0)
    {
    }
    Position operator+=(const Position& rhs)
    {
        row += rhs.row;
        col += rhs.col;
        return *this;
    }

    int row, col;

    bool isVaild()
    {
        return row >= 0 && row < 8 && col >= 0 && col < 8;
    }
};
Position operator+(const Position& lhs, const Position& rhs)
{
    Position sum = lhs;
    sum += rhs;
    return sum;
}
class Board {
    
    const Position direction[8] = {
        { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 }, { 1, 1 }, { -1, -1 }, { 1, -1 }, { -1, 1 }
    };
    char board[8][8];
   
public:
    bool gameEnd = false;
    static const int EMPTY = -1;
    static const int WHITH = 0;
    static const int BLACK = 1;
    int nowPlayer;

    std::vector<Position> lastFlipped;
    Position lastChoose;
    bool isAbleToUndo = false;

    Board()
    {
        memset(board, -1, sizeof(board));
        board[3][3] = 0;
        board[3][4] = 1;
        board[4][3] = 1;
        board[4][4] = 0;

        nowPlayer = BLACK;
    }
    Board(const Board &rhs)
    {
        memcpy(board,rhs.board,sizeof(board));
        nowPlayer = rhs.nowPlayer;
        lastFlipped = rhs.lastFlipped;
        lastChoose = rhs.lastChoose;
        gameEnd = rhs.gameEnd;
        isAbleToUndo = rhs.isAbleToUndo;
    }
    char& operator[](Position pos)
    {
        return board[pos.row][pos.col];
    }
    const char& operator[](Position pos) const
    {
        return board[pos.row][pos.col];
    }
    Board& operator=(const Board &rhs){
        memcpy(board,rhs.board,sizeof(board));
        nowPlayer = rhs.nowPlayer;
        lastFlipped = rhs.lastFlipped;
        lastChoose = rhs.lastChoose;
        gameEnd = rhs.gameEnd;
        isAbleToUndo = rhs.isAbleToUndo;
        return *this;
    }
   

    std::tuple<bool, std::vector<Position>> doChooseAnalysis(Position pos,bool requestChange)
    {
        std::vector<Position> needToChange;
        bool ableToChoose = false;
        if ((*this)[pos] != -1) {
            return { ableToChoose, needToChange };
        }
        std::vector<Position> others;
        for (Position nowd : direction) {
            Position now = pos + nowd;
            //printf("%d %d\n", pos.row, pos.col);
            if(requestChange){
                others.clear();
            }
            bool hasOthers = false;
            while (now.isVaild()) {
                //printf("%d %d\n", now.row, now.col);
                if ((*this)[now] == nowPlayer) {
                    //find another self
                    if(requestChange && !others.empty()){
                        needToChange.insert(needToChange.end(), others.begin(), others.end());
                    }
                    if(!requestChange && hasOthers){
                        ableToChoose = true;
                    }
                    break;
                } else if ((*this)[now] == !nowPlayer) {
                    //find the other player
                    if(requestChange){
                        others.push_back(now);
                    }else{
                        hasOthers = true;
                    }
                } else {
                    //find space
                    break;
                }
                now += nowd;
            }
        }
        if(requestChange){
            return { !needToChange.empty(), needToChange };
        }else{
            return {ableToChoose,needToChange};
        }
        
    }

    bool isAbleToChoose(int row, int col)
    {
        return isAbleToChoose(Position(row, col));
    }

    bool isAbleToChoose(Position pos)
    {
        auto [ableToChoose,needToChange] = doChooseAnalysis(pos,false);
        return ableToChoose;
    }



    std::tuple<bool, std::vector<Position>> isAbleToChooseWithChange(Position pos)
    {
        return doChooseAnalysis(pos,true);
    }

    std::vector<Position> findPossibleChoose()
    {
        std::vector<Position> possibleChoose;
        Position now(0, 0);
        for (now.col = 0; now.row < 8; now.row++) {
            for (now.col = 0; now.col < 8; now.col++) {
                if((*this)[now] != -1){
                    continue;
                }
                bool ableToChoose  = isAbleToChoose(now);
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

    bool doChoose(Position pos)
    {
        auto [ableToChoose, needToChange] = isAbleToChooseWithChange(pos);
        if (ableToChoose) {
            lastFlipped.clear();
            for (Position p : needToChange) {
                lastFlipped.push_back(p);
                (*this)[p] = nowPlayer;
            }
        } else {
            return false;
        }
        lastChoose = pos;
        isAbleToUndo = true;

        (*this)[pos] = nowPlayer;
        nowPlayer = !nowPlayer;
        std::vector<Position> possibleChoose = findPossibleChoose();
        if (possibleChoose.size() == 0) {
            nowPlayer = !nowPlayer;
            possibleChoose = findPossibleChoose();
            if (possibleChoose.size() == 0) {
                nowPlayer = !nowPlayer;
                gameEnd = true;
            }
        }


        auto[isFull, whiteNum, blackNum] = countBoard();
        gameEnd = gameEnd || isFull;
        
        return true;
    }

    bool undoLastChoose(){
        if(!isAbleToUndo){
            return false;
        }

        for(Position p : lastFlipped){
            (*this)[p] = nowPlayer; //被吃掉的棋子肯定是现在执子的
        }
        (*this)[lastChoose] = EMPTY;

        nowPlayer = !nowPlayer;

        isAbleToUndo = false;
        gameEnd = false;

        return true;
    }

    std::tuple<bool, int, int> countBoard()
    {
        bool isFull = true;
        int whiteNum = 0;
        int blackNum = 0;
        Position now;
        for (now.row = 0; now.row < 8; now.row++) {
            for (now.col = 0; now.col < 8; now.col++) {
                if ((*this)[now] == EMPTY) {
                    isFull = false;
                }
                if ((*this)[now] == BLACK) {
                    blackNum++;
                }
                if ((*this)[now] == WHITH) {
                    whiteNum++;
                }
            }
        }

        return { isFull, whiteNum, blackNum };
    }

    void printBoard()
    {
        Position now;
        for (now.row = 0; now.row < 8; now.row++) {
            for (now.col = 0; now.col < 8; now.col++) {
                if ((*this)[now] == EMPTY) {
                    printf("*");
                }
                if ((*this)[now] == BLACK) {
                    printf("1");
                }
                if ((*this)[now] == WHITH) {
                    printf("0");
                }
            }
            printf("\n");
        }
    }
};
