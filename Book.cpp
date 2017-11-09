//
// Created by 王悉宇 on 2017/11/8.
//

#include "Book.hpp"

unordered_map<Board,Position,HashFunc,EqualKey> BookChoose::GoodChoose;
unordered_map<Board,Position,HashFunc,EqualKey> BookChoose::BadChoose;

unsigned int BookChoose::bZobrist[0x10000];
unsigned int BookChoose::wZobrist[0x10000];


inline unsigned int BookChoose::getZobKey(const BitBoard& my, const BitBoard& op){
    unsigned short* rmy = (unsigned short*)&my;
    unsigned short* rop = (unsigned short*)&op;
    return bZobrist[*rmy] ^ bZobrist[*(rmy + 1)]
           ^ bZobrist[*(rmy + 2)] ^ bZobrist[*(rmy + 3)]
           ^ wZobrist[*rop] ^ wZobrist[*(rop + 1)]
           ^ wZobrist[*(rop + 2)] ^ wZobrist[*(rop + 3)];
}

BookChoose::BookChoose() {
    loadBook();
    for (int i = 0; i < 0x10000; i++) {
        rand_r(&bZobrist[i]);
        rand_r(&wZobrist[i]);
    }
}

bool BookChoose::loadBook() {
    std::ifstream in("./book.bin", std::ios::binary | std::ios::in);
    if (!in) {
        fprintf(stderr, "cannot find Opening Book!\n");
    }

    int bookSize;
    in.read((char *) &bookSize, sizeof(bookSize));
    if (!in || bookSize <= 0) {
        fprintf(stderr, "Opening Book size error !\n");
        in.close();
        return true;
    }

    //book = new Book(bookSize * 3 / 2);
    fprintf(stderr,"bookSize:%d\n",bookSize);
    UCT uct;
    for (int i = 0; i < bookSize; i++) {
        Board tmpBoard;
        //BookNode node;
        BitBoard my,op;
        char ch, mov;
        int I;
        in.read((char *) &my, sizeof(my));
        //node.setMy(bb);
        in.read((char *) &op, sizeof(op));
        //node.setOp(bb);
        tmpBoard.used = my|op;
        tmpBoard.board = my;

        //tmpBoard.printBoard();
        //uct.getNextAction(tmpBoard);
        int black,white ;
        tie(ignore,black,white) = tmpBoard.countBoard();
        in.read(&mov, sizeof(mov));
        //node.setMoveCount(ch);
        //printf("mov:%d\n",mov);
        bool hasGood = false;
        for (int j = 0; j < mov; j++) {
            tmpBoard.used = my|op;
            tmpBoard.board = my;
            in.read(&ch, sizeof(ch));
            in.read((char *) &I, sizeof(I));
            int row= 7-ch/8 , col = 7-ch%8;
//
//            int whiteNum,blackNum;
//            tie(ignore, whiteNum, blackNum) = tmpBoard.countBoard();

            if(!tmpBoard.isAbleToChoose(Position(row,col))){
                fprintf(stderr,"errror!!!");
                exit(1);
            }

            if (I<-99999){
                BadChoose[tmpBoard]=Position(row,col);
                tmpBoard.turnLeft();
                BadChoose[tmpBoard]=Position(7-col,row);
                tmpBoard.turnLeft();
                BadChoose[tmpBoard]=Position(7-row,7-col);
                tmpBoard.turnLeft();
                BadChoose[tmpBoard]=Position(col,7-row);
                continue;
            }
            if (!hasGood && I>0){
                GoodChoose[tmpBoard]=Position(row,col);
                tmpBoard.turnLeft();
                GoodChoose[tmpBoard]=Position(7-col,row);
                tmpBoard.turnLeft();
                GoodChoose[tmpBoard]=Position(7-row,7-col);
                tmpBoard.turnLeft();
                GoodChoose[tmpBoard]=Position(col,7-row);
                hasGood = true;
                continue;
            }




            //node.setMove(j, ch);
            //node.setEval(j, I);
//        }
//        if (!in) {
//            delete book;
//            book = nullptr;
//            in.close();
//            return false;
//        }
//        book->insert(node);
        }

    }
    fprintf(stderr,"Good_size:%lu\n",GoodChoose.size());
    fprintf(stderr, "Bad_size:%lu\n",BadChoose.size());
    in.close();
    return true;
}

Position BookChoose::getNextAction(const Board &board){
    Board tmpBoard = board;
    tmpBoard.inverse();
    if(GoodChoose.count(board) <=0){
        return GoodChoose[tmpBoard];
    }
    return GoodChoose[board];
}
int BookChoose::hasNextAction(const Board &board){
    Board tmpBoard = board;
    tmpBoard.inverse();
    return static_cast<bool>( GoodChoose.count(board) ) || static_cast<bool>( GoodChoose.count(tmpBoard) );
}