//
//  board.hpp
//  Othello_Server
//
//  Created by 王悉宇 on 2017/11/1.
//  Copyright © 2017年 王悉宇. All rights reserved.
//

#ifndef board_hpp
#define board_hpp

#include <stdio.h>
#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>

class Position {
public:
    Position(int row, int col);

    Position();

    Position operator+=(const Position &rhs);

    int row, col;

    bool isVaild();
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
    char board[8][8];

public:
    bool gameEnd = false;
    static const int EMPTY = -1;
    static const int WHITH = 0;
    static const int BLACK = 1;
    int nowPlayer;

    Board();

    Board(const Board &b);

    char &operator[](Position pos);

    const char &operator[](Position pos) const;

    Board &operator=(const Board &rhs);


    std::tuple<bool, std::vector<Position>> doChooseAnalysis(Position pos, bool requestChange);

    bool isAbleToChoose(int row, int col);

    bool isAbleToChoose(Position pos);

    std::tuple<bool, std::vector<Position>> isAbleToChooseWithChange(Position pos);

    std::vector<Position> findPossibleChoose();

    bool doChoose(Position pos);

    std::tuple<bool, int, int> countBoard();

    void printBoard();
};

#endif /* board_hpp */
