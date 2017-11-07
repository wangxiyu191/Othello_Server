//
//  uct.hpp
//  Othello_Server
//
//  Created by 王悉宇 on 2017/11/1.
//  Copyright © 2017年 王悉宇. All rights reserved.
//

#ifndef uct_hpp
#define uct_hpp

#include <stdio.h>
#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>
#include <algorithm>
#include <random>
#include <climits>
#include <cmath>
#include <limits>
#include "board.hpp"

class Node {
public:
    double wins = 0;
    int visits = 0;
    Node *parentNode;
    Position choose;

    std::vector<Position> unexamined;
    std::vector<Node *> children;
    int nowPlayer;

    Node();

    Node(Node *parentNode, Board &board, Position choose, int player);

    ~Node();

    Node *addBackChild(Board &board, int player);

    Node *selectChild();

    void update(int blackNum, int whiteNum);

    Node *mostVisitedChild();
};


class UCT {
public:
    Position getNextAction(Board &board);
};

#endif /* uct_hpp */
