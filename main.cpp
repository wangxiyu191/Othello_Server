#include "uct.hpp"
#include "fullsearch.hpp"

using namespace std;

int main(int argc, char const *argv[]) {
    Board test;

    // test.updateGameState();
    // test.doChoose(Position(2, 3));
    // test.printBoard();

    UCT uct;
    FullSearch fullsearch;
    srand(time(NULL));

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
                if (whiteNum + blackNum > 8 * 8 - 4 - 10 + 1000 ) {
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
        int whiteNum, blackNum;
        tie(ignore, whiteNum, blackNum) = test.countBoard();
        if(whiteNum>blackNum) {
            printf("WHITEWIN\n");
        } else if (whiteNum<blackNum) {
            printf("BLACKWIN\n");
        }else{
            printf("NONEWIN\n");
        }
        fflush(stdout);

    } else {
        test.printBoard();
        while (!test.gameEnd) {
            puts("");
            int whiteNum, blackNum;
            tie(ignore, whiteNum, blackNum) = test.countBoard();
            Position choose;
            if (whiteNum + blackNum > 8 * 8 - 4 - 13) {
                choose = fullsearch.getNextAction(test);
            } else {
                choose = uct.getNextAction(test);
            }
            fprintf(stderr, "%s do [%d,%d]\n", test.nowPlayer == Board::BLACK ? "black" : "White", choose.row,
                    choose.col);
            test.doChoose(choose);
            test.printBoard();
            tie(ignore, whiteNum, blackNum) = test.countBoard();
            fprintf(stderr, "[Count] black:%d white:%d \n", blackNum, whiteNum);
        }


    }

    return 0;
}
