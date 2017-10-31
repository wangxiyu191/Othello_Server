#include "uct.cpp"
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

    if (argc == 2) {
        while (!test.findPossibleChoose().empty()) {
            if (test.nowPlayer == Board::BLACK) {
                int row, col;
                scanf("%d%d", &row, &col);
                test.doChoose(Position(row, col));
            }else{
                Position choose = uct.getNextAction(test);
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
            Position choose = uct.getNextAction(test);
            test.doChoose(choose);
            test.printBoard();
        }
    }

    return 0;
}