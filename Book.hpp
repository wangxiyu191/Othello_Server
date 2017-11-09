//
// Created by 王悉宇 on 2017/11/8.
//

#ifndef OTHELLO_SERVER_BOOK_H
#define OTHELLO_SERVER_BOOK_H

#include <cstdio>
#include <cstring>
#include <fstream>
#include "board.hpp"
#include "uct.hpp"
#include <unordered_map>
#include <cstdlib>
#include <vector>
using namespace std;
struct HashFunc
{
    std::size_t operator()(const Board &board) const
    {
        using std::size_t;
        using std::hash;

        return ((hash<BitBoard>()(board.board) ^ (hash<BitBoard>()(board.used) << 1)));
    }
};

struct EqualKey
{
    bool operator () (const Board &lhs, const Board &rhs) const
    {
        return lhs.board  == rhs.board
               && lhs.used == rhs.used;
    }
};

class BookChoose{
public:

    BookChoose();

    static unsigned int bZobrist[65536];
    static unsigned int wZobrist[65536];
    unsigned int getZobKey(const BitBoard &my, const BitBoard &op);

    bool loadBook();

    static unordered_map<Board, Position, HashFunc, EqualKey> GoodChoose;
    static unordered_map<Board, Position, HashFunc, EqualKey> BadChoose;

    Position getNextAction(const Board &board);

    int hasNextAction(const Board &board);
};

#endif //OTHELLO_SERVER_BOOK_H
