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
#include "fullsearch.hpp"
#include "eval.h"
#include <thread>

class Node {
public:
    double wins = 0;//该节点获胜次数
    int visits = 0;//该节点访问次数
    Node *parentNode;//父节点
    Position choose;//该节点操作

    std::vector<Position> unexamined;//未被拓展的孩子节点
    std::vector<Node *> children;//已被拓展的孩子节点
    int nowPlayer;//当前玩家

    Node();

    Node(Node *parentNode, Board &board, Position choose, int player);

    ~Node();

    Node *addBackChild(Board &board, int player);

    Node *selectChild();

    void update(int blackNum, int whiteNum);

    Node *mostVisitedChild();

    bool operator<(const Node &rhs) const;

    bool operator>(const Node &rhs) const;

    bool operator<=(const Node &rhs) const;

    bool operator>=(const Node &rhs) const;
};


class UCT {
    FullSearch fullsearch;
public:
    Position getNextAction(Board &board);

    void MCTSThread(Node *root,Board &board,int total_count);
};

#endif /* uct_hpp */
