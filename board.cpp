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
    static const int EMPTY = -1;
    static const int WHITH = 0;
    static const int BLACK = 1;
    int nowPlayer;

    Board()
    {
        memset(board, -1, sizeof(board));
        board[3][3] = 0;
        board[3][4] = 1;
        board[4][3] = 1;
        board[4][4] = 0;

        nowPlayer = BLACK;
    }
    Board(const Board &b)
    {
        memcpy(board,b.board,sizeof(board));
        nowPlayer = b.nowPlayer;

    }
    char& operator[](Position pos)
    {
        return board[pos.row][pos.col];
    }
    const char& operator[](Position pos) const
    {
        return board[pos.row][pos.col];
    }

    std::tuple<bool, std::vector<Position>> isAbleToChoose(int row, int col)
    {
        return isAbleToChoose(Position(row, col));
    }

    std::tuple<bool, std::vector<Position>> isAbleToChoose(Position pos)
    {
        std::vector<Position> needToChange;
        if ((*this)[pos] != -1) {
            return { false, needToChange };
        }
        for (Position nowd : direction) {
            Position now = pos + nowd;
            //printf("%d %d\n", pos.row, pos.col);
            std::vector<Position> others;
            while (now.isVaild()) {
                if ((*this)[now] == nowPlayer) {
                    if (!others.empty()) {
                        needToChange.insert(needToChange.end(), others.begin(), others.end());
                    } else {
                        break;
                    }
                } else if ((*this)[now] == !nowPlayer) {
                    others.push_back(now);
                } else {
                    break;
                }
                now += nowd;
            }
        }
        return { !needToChange.empty(), needToChange };
    }

    std::vector<Position> findPossibleChoose()
    {
        std::vector<Position> possibleChoose;
        Position now(0, 0);
        for (now.col = 0; now.row < 8; now.row++) {
            for (now.col = 0; now.col < 8; now.col++) {
                auto[ableToChoose, needToChange] = isAbleToChoose(now);
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
        auto [ableToChoose, needToChange] = isAbleToChoose(pos);
        if (ableToChoose) {
            for (Position p : needToChange) {
                (*this)[p] = nowPlayer;
            }
        } else {
            return false;
        }
        (*this)[pos] = nowPlayer;
        nowPlayer = !nowPlayer;
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

    std::tuple<bool, int, int> updateGameState()
    {
        bool gameEnd = false;
        auto[isFull, whiteNum, blackNum] = countBoard();

        gameEnd = gameEnd || isFull;

        std::vector<Position> possibleChoose = findPossibleChoose();
        if (possibleChoose.size() == 0) {
            nowPlayer = !nowPlayer;
            possibleChoose = findPossibleChoose();
            if (possibleChoose.size() == 0) {
                gameEnd = true;
            }
        }

        // if(possibleChoose.size() != 0)
        // {
        //     possibleChoose.forEach(function(pos){
        //         //console.log("#"+pos[0]+"-"+pos[1]);
        //         $("#"+pos[0]+"-"+pos[1]).css('border-color','yellow');
        //     });
        // }
        return { gameEnd, whiteNum, blackNum };
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
