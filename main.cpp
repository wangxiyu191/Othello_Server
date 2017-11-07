#include "uct.hpp"
#include "fullsearch.hpp"
#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>

using namespace std;
int main(int argc, char const* argv[])
{
    Board test;
    
    // test.updateGameState();
    // test.doChoose(Position(2, 3));
    // test.printBoard();
    
    UCT uct;
    FullSearch fullsearch;
    
    if (argc == 2) {
        while (!test.findPossibleChoose().empty()) {
            if (test.nowPlayer == Board::BLACK) {
                int row, col;
                scanf("%d%d", &row, &col);
                test.doChoose(Position(row, col));
            }else{
                Position choose;
                auto[isFull, whiteNum, blackNum] = test.countBoard();
                if(whiteNum+blackNum>8*8-4-13){
                    choose = fullsearch.getNextAction(test);
                }else{
                    choose = uct.getNextAction(test);
                }
                printf("%d,%d\n", choose.row, choose.col);
                fflush(stdout);
                test.doChoose(choose);
            }
            
            //test.printBoard();
        }
        
    } else {
        test.printBoard();
        while (!test.gameEnd) {
            puts("");
            auto[isFull, whiteNum, blackNum] = test.countBoard();
            Position choose ;
            if(whiteNum+blackNum>8*8-4-13){
                choose = fullsearch.getNextAction(test);
            }else{
                choose = uct.getNextAction(test);
            }
            fprintf(stderr,"%s do [%d,%d]\n", test.nowPlayer==Board::BLACK?"black":"White",choose.row,choose.col);
            test.doChoose(choose);
            test.printBoard();
            tie(ignore, whiteNum, blackNum) = test.countBoard();
            fprintf(stderr,"[Count] black:%d white:%d \n", blackNum,whiteNum);
        }
        
        
    }
    
    return 0;
}
