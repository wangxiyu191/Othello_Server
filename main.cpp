#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>
#include "uct.cpp"
using namespace std;

int main()
{
    Board test;
    test.printBoard();
    // test.updateGameState();
    // test.doChoose(Position(2, 3));
    // test.printBoard();

    UCT uct;

    // while(!test.findPossibleChoose().empty()){
    //     if(test.nowPlayer == Board::BLACK){
    //         int row,col;
    //         scanf("%d%d",&row,&col);
    //         test.doChoose(Position(row,col));
    //         test.updateGameState();
    //     }
    //     if(test.nowPlayer == Board::WHITH){
    //         Position choose = uct.getNextAction(test);
    //         printf("%d,%d\n", choose.row,choose.col);
    //         test.doChoose(choose);
    //         test.updateGameState();
    //     }
        
    //     //test.printBoard();
    // }

    while(!test.findPossibleChoose().empty()){
        puts("");
        Position choose = uct.getNextAction(test);
        test.doChoose(choose);
        test.updateGameState();
        test.printBoard();

    }
    return 0;
}