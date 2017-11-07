//
//  board.hpp
//  Othello_Server
//
//  Created by 王悉宇 on 2017/11/1.
//  Copyright © 2017年 王悉宇. All rights reserved.
//

#ifndef board_hpp
#define board_hpp


#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>
#include <cstdlib>
#include <cstdio>


class Position {
public:
    Position(int row, int col);

    Position();

    Position operator+=(const Position &rhs);

    int row, col;
};

class Board {

    const Position direction[8] = {
            {0,  1},
            {1,  0},
            {0,  -1},
            {-1, 0},
            {1,  1},
            {-1, -1},
            {1,  -1},
            {-1, 1}
    };
    uint64_t board;
    uint64_t used;
    static const uint64_t U64_1=1;

public:
    bool gameEnd = false;
    static const int EMPTY = -1;
    static const int WHITE = 0;
    static const int BLACK = 1;
    int nowPlayer;

    Board();

    Board(const Board &b);

    Board &operator=(const Board &rhs);


    std::tuple<bool, std::vector<Position>> doChooseAnalysis(Position pos, bool requestChange);

    bool isAbleToChoose(int row, int col);

    bool isAbleToChoose(Position pos);

    std::tuple<bool, std::vector<Position>> isAbleToChooseWithChange(Position pos);

    std::vector<Position> findPossibleChoose();

    bool doChoose(Position pos);

    std::tuple<bool, int, int> countBoard();

    void printBoard();

    int readBoard(const Position &pos);

    void setBoard(const Position &pos, int chess);

    void setEmpty(const Position &pos);

    bool isEmpty(const Position &pos);
};

#endif /* board_hpp */
