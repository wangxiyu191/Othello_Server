//
//  fullsearch.cpp
//  Othello_Server
//
//  Created by 王悉宇 on 2017/11/7.
//  Copyright © 2017年 王悉宇. All rights reserved.
//

#include "fullsearch.hpp"

using namespace std;


FullSearch::FullSearch() = default;

FullSearch::~FullSearch() = default;
//搜索函数
int FullSearch::search(Board &board) {
    if (board.gameEnd) {
        int whiteNum,blackNum;
        tie(ignore, whiteNum, blackNum) = board.countBoard();
        if (whiteNum == blackNum) {
            return TIE;
        }
        if (board.nowPlayer == Board::BLACK) {
            return blackNum > whiteNum ? WIN : LOST;
        }
        if (board.nowPlayer == Board::WHITE) {
            return blackNum < whiteNum ? WIN : LOST;
        }
    }
    bool isAbleToTie = false;
    bool isAbleToWin = false;

    vector<Position> possibleChoose = board.findPossibleChoose();
    for (Position choose : possibleChoose) {
        Board tmpBoard = board;
        tmpBoard.doChoose(choose);
        int value = search(tmpBoard);

        //这里胜负正好相反，下一手赢了，说明当前手输了。
        if (tmpBoard.nowPlayer != board.nowPlayer) {
            switch (value) {
                case WIN:
                    value = LOST;
                    break;
                case LOST:
                    value = WIN;
                    break;
                case TIE:
                    value = TIE;
                    break;
            }
        }
        // if(board.nowPlayer == Board::WHITE){
        //     printf("%d\n", value);
        // }

        if (value == WIN) {
            isAbleToWin = true;
            //已经找到赢的路了，剪枝
            break;
        } else if (value == TIE) {
            isAbleToTie = true;
        }
    }
    if (isAbleToWin) {
        return WIN;
    } else if (isAbleToTie) {
        return TIE;
    } else {
        return LOST;
    }
}

Position FullSearch::getNextAction(const Board &board) {
    Board tmpBoard = board;
    vector<Position> possibleChoose = tmpBoard.findPossibleChoose();
    Position bestChoose;
    int bestValue = INT_MIN;
    for (const Position &choose : possibleChoose) {
        tmpBoard = board;
        tmpBoard.doChoose(choose);
        int value = search(tmpBoard);

        if (tmpBoard.nowPlayer != board.nowPlayer) {
            switch (value) {
                case WIN:
                    value = LOST;
                    break;
                case LOST:
                    value = WIN;
                    break;
                case TIE:
                    value = TIE;
                    break;
            }
        }
        if (value > bestValue) {
            bestValue = value;
            bestChoose = choose;
        }

        //fprintf(stderr, "value = %d\n", value);
        if (bestValue == WIN) {
            //fprintf(stderr, "%s think he will win\n", board.nowPlayer == Board::BLACK ? "black" : "white");
            break;
        }
    }

    if (bestValue == LOST) {
        //fprintf(stderr, "%s think he will lose\n", board.nowPlayer == Board::BLACK ? "black" : "white");
    }

    return bestChoose;
}

//查询当前盘面最终情况
int FullSearch::getGameResult(const Board &board){
    Board tmpBoard = board;
    vector<Position> possibleChoose = tmpBoard.findPossibleChoose();

    int bestValue = INT_MIN;
    for (const Position &choose : possibleChoose) {
        tmpBoard = board;
        tmpBoard.doChoose(choose);
        int value = search(tmpBoard);

        if (tmpBoard.nowPlayer != board.nowPlayer) {
            switch (value) {
                case WIN:
                    value = LOST;
                    break;
                case LOST:
                    value = WIN;
                    break;
                case TIE:
                    value = TIE;
                    break;
            }
        }
        if (value > bestValue) {
            bestValue = value;
        }

        //fprintf(stderr, "value = %d\n", value);
        if (bestValue == WIN) {
            //fprintf(stderr, "%s think he will win\n", board.nowPlayer == Board::BLACK ? "black" : "white");
            return WIN;
        }
    }

    if (bestValue == LOST) {
        return LOST;
    }

    return TIE;
}
