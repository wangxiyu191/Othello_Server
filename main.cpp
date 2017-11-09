#include "uct.hpp"
#include "fullsearch.hpp"
#include "Book.hpp"
#include "eval.h"

using namespace std;

int main(int argc, char const *argv[]) {
    Board test;

    // test.updateGameState();
    // test.doChoose(Position(2, 3));
    // test.printBoard();

    UCT uct;
    FullSearch fullsearch;
    BookChoose book;
    if(!Eval::initPatterns()){
        printf("Patterns Error!\n");
        exit(1);
    }


    if (argc == 2) {
        while (!test.findPossibleChoose().empty()) {
            if (test.nowPlayer == Board::BLACK) {
                int row, col;
                scanf("%d%d", &row, &col);
                test.doChoose(Position(row, col));
            } else {
                Position choose;
                int whiteNum, blackNum;
                tie(ignore, whiteNum, blackNum) = test.countBoard();
                if (whiteNum + blackNum > 8 * 8 - 4 - 13 +1000) {
                    choose = fullsearch.getNextAction(test);
                } else {
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
            int whiteNum, blackNum;
            tie(ignore, whiteNum, blackNum) = test.countBoard();
            Position choose;
            if(book.hasNextAction(test)){
                choose = book.getNextAction(test);
                puts("test");
            } /*else if (whiteNum + blackNum > 8 * 8 - 17) {
                choose = fullsearch.getNextAction(test);
            }*/ else {


                choose = uct.getNextAction(test);
//                if(test.nowPlayer  == Board::WHITE){
//                    int bestValue = INT_MIN;
//                    Position bestChoice;
//                    puts("=================begin!");
//                    auto possibleChoose = test.findPossibleChoose();
//                    for(auto p:possibleChoose){
//                        printf("[%d,%d]\n",p.row,p.col);
//                        Board tmptmpBoard = test;
//                        tmptmpBoard.doChoose(p);
//                        int value = Eval::evaluate(tmptmpBoard,test.nowPlayer);
//
//                        if(value>bestValue){
//                            bestValue = value;
//                            bestChoice = p;
//                        }
//                    }
//                    choose = bestChoice;
//                    puts("=================end!");
//                }

            }
            fprintf(stderr, "%s do [%d,%d]\n", test.nowPlayer == Board::BLACK ? "black" : "White", choose.row,
                    choose.col);
            test.doChoose(choose);
            test.printBoard();
            tie(ignore, whiteNum, blackNum) = test.countBoard();
            fprintf(stderr, "[Count] black:%d white:%d \n", blackNum, whiteNum);
            fflush(stderr);
            fflush(stdout);
        }


    }

    return 0;
}
