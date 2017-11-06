#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>
#include <algorithm>
#include <random>
#include <climits>
#include <cmath>
#include <limits>
#include "board.cpp"
using namespace std;


class FullSearch
{
    const static int WIN = 1;
    const static int TIE = 0; 
    const static int LOST = -1;
    
public:
    FullSearch()=default;
    ~FullSearch()=default;
    int search(Board &board){
        if(board.gameEnd){
            auto [isFull, whiteNum, blackNum ] = board.countBoard();
            if(whiteNum == blackNum){
                return TIE;
            }
            if(board.nowPlayer == Board::BLACK){
                return blackNum>whiteNum?WIN:LOST;
            }
            if(board.nowPlayer == Board::WHITH){
                return blackNum<whiteNum?WIN:LOST;
            }
        }
        bool isAbleToWin = false;
        bool isAbleToLost = false;

        vector<Position> possibleChoose = board.findPossibleChoose();
        for(Position choose : possibleChoose){
            Board tmpBoard = board;
            tmpBoard.doChoose(choose);
            int value = search(tmpBoard);

            //这里胜负正好相反，下一手赢了，说明当前手输了。
            if(tmpBoard.nowPlayer != board.nowPlayer){
                switch(value){
                    case WIN: value = LOST;break;
                    case LOST: value = WIN;break;
                    case TIE: value = TIE;break;
                }
            }
            // if(board.nowPlayer == Board::WHITH){
            //     printf("%d\n", value);
            // }

            if(value == WIN){
                isAbleToWin = true;
                break;
            }else if(value == LOST){
                isAbleToLost = true;
            }
        }
        if(isAbleToWin){
            return WIN;
        }else if(isAbleToLost){
            return LOST;
        }else{
            return TIE;
        }
    }
    Position getNextAction(Board &board){
        Board tmpBoard = board;
        vector<Position> possibleChoose = tmpBoard.findPossibleChoose();
        Position bestChoose;
        int bestValue = INT_MIN;
        for(Position choose : possibleChoose){
            tmpBoard = board;
            tmpBoard.doChoose(choose);
            int value = search(tmpBoard);

            if(tmpBoard.nowPlayer != board.nowPlayer){
                switch(value){
                    case WIN: value = LOST;break;
                    case LOST: value = WIN;break;
                    case TIE: value = TIE;break;
                }
            }
            if(value>bestValue){
                bestValue = value;
                bestChoose = choose;
            }

            fprintf(stderr,"value = %d\n",value );
            if(bestValue == WIN){
                fprintf(stderr,"%s think he will win\n",board.nowPlayer == Board::BLACK?"black":"white" );
                break;
            }
        }

        if(bestValue == LOST){
            fprintf(stderr,"%s think he will lose\n",board.nowPlayer == Board::BLACK?"black":"white" );
        }

        return bestChoose ;
    }
    
};