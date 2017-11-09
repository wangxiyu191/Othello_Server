//
// Created by 王悉宇 on 2017/11/8.
//

#include "eval.h"



const int Eval::patternPtr[ACTUAL_PATTERNS] = {
        0,0,0,0,     // line 1 + 2X
        1,1,1,1,     // line 2
        2,2,2,2,     // line 3
        3,3,3,3,     // line 4
        4,4,         // diag 8
        5,5,5,5,     // diag 7
        6,6,6,6,     // diag 6
        7,7,7,7,     // diag 5
        8,8,8,8,     // diag 4
        9,9,9,9,9,9,9,9, // 2x5 square
        10,10,10,10      // 3*3 square
};
int *Eval::patternValues;
int Eval::patternCorrection[STAGES];
unsigned short Eval::myPV[256][8][ACTUAL_PATTERNS];
unsigned char Eval::pPtr[256][8][ACTUAL_PATTERNS];
unsigned char Eval::pVCount[256][8];
int Eval::posDepend[MAXSTEP][ACTUAL_PATTERNS];
int Eval::patternDependCount[ACTUAL_PATTERNS];
int Eval::patternOffset[STAGES][ACTUAL_PATTERNS];
float *Eval::deltaSum;
int *Eval::occurance;

const int  Eval::rstage[MAXSTEP + 1] = {
        16, 16, 16, 16, 15, 15, 15, 14, 14, 14, 13, 13, 13,
        12, 12, 12, 11, 11, 11, 10, 10, 10, 9, 9, 9,
        8, 8, 8, 7, 7, 7, 6, 6, 6, 5, 5, 5,
        4, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


const int Eval::pow3[10] = {
        1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683
};


const int Eval::patternCount[PATTERNS] = {
        pow3[9] * 3, // line 1 + 2X
        pow3[8],     // line 2
        pow3[8],     // line 3
        pow3[8],     // line 4
        pow3[8],     // diag 8
        pow3[7],     // diag 7
        pow3[6],     // diag 6
        pow3[5],     // diag 5
        pow3[4],     // diag 4
        pow3[9] * 3, // 2x5 square
        pow3[9]      // 3*3 square
};




inline int Eval::bits(const BitBoard& bb) {
    const unsigned long long c55 = 0x5555555555555555ull;
	const unsigned long long c33 = 0x3333333333333333ull;
	const unsigned long long c0f = 0x0f0f0f0f0f0f0f0full;
	const unsigned long long h01 = 0x0101010101010101ull;
	BitBoard x = bb;
	x -= (x >> 1) & c55;
	x = (x & c33) + ((x >> 2) & c33);
	x = (x + (x >> 4)) & c0f;
	return (x * h01) >> 56;
}

inline int Eval::getResult(const BitBoard& my,int empties) {
    int diff = (bits(my) << 1) + empties - MAXSTEP;
    if (diff > 0)
        diff += empties;
    else if (diff < 0)
        diff -= empties;
    return diff;
}


inline int Eval::endToMid(int value){
    if (value > 0) return INFINITE - MAXSTEP + value;
    else if (value < 0) return -INFINITE + MAXSTEP + value;
    else return 0;
}



// count the mobility of b1 over b2
BitBoard Eval::mobility(const BitBoard& b1, const BitBoard& b2) {
    //01111110
	//01111110
	//01111110
	//01111110
	//01111110
	//01111110
	//01111110
	//01111110
	//for directions 1 and 5
	const unsigned long long mask1 = 0x7e7e7e7e7e7e7e7eull;
	//00000000
	//01111110
	//01111110
	//01111110
	//01111110
	//01111110
	//01111110
	//00000000
	//for directions 2,4,6,8
	const unsigned long long mask2 = 0x007e7e7e7e7e7e00ull;
	//00000000
	//11111111
	//11111111
	//11111111
	//11111111
	//11111111
	//11111111
	//00000000
	//for directions 3 and 7
	const unsigned long long mask3 = 0x00ffffffffffff00ull;
	BitBoard b3 = b1;
	BitBoard b4 = b2;
	BitBoard result = 0;

	BitBoard b2m = b4 & mask1;
	BitBoard m = ((b2m << 1) & b3) >> 2;
	result |= m; m &= b2m; m >>= 1;
	result |= m; m &= b2m; m >>= 1;
	result |= m; m &= b2m; m >>= 1;
	result |= m; m &= b2m; m >>= 1;
	result |= m; m &= b2m; m >>= 1;
	result |= m;
	m = ((b2m >> 1) & b3) << 2;
	result |= m; m &= b2m; m <<= 1;
	result |= m; m &= b2m; m <<= 1;
	result |= m; m &= b2m; m <<= 1;
	result |= m; m &= b2m; m <<= 1;
	result |= m; m &= b2m; m <<= 1;
	result |= m;

	b2m = b4 & mask2;
	m = ((b2m >> 7) & b3) << 14;
	result |= m; m &= b2m; m <<= 7;
	result |= m; m &= b2m; m <<= 7;
	result |= m; m &= b2m; m <<= 7;
	result |= m; m &= b2m; m <<= 7;
	result |= m; m &= b2m; m <<= 7;
	result |= m;
	m = ((b2m >> 9) & b3) << 18;
	result |= m; m &= b2m; m <<= 9;
	result |= m; m &= b2m; m <<= 9;
	result |= m; m &= b2m; m <<= 9;
	result |= m; m &= b2m; m <<= 9;
	result |= m; m &= b2m; m <<= 9;
	result |= m;
	m = ((b2m << 7) & b3) >> 14;
	result |= m; m &= b2m; m >>= 7;
	result |= m; m &= b2m; m >>= 7;
	result |= m; m &= b2m; m >>= 7;
	result |= m; m &= b2m; m >>= 7;
	result |= m; m &= b2m; m >>= 7;
	result |= m;
	m = ((b2m << 9) & b3) >> 18;
	result |= m; m &= b2m; m >>= 9;
	result |= m; m &= b2m; m >>= 9;
	result |= m; m &= b2m; m >>= 9;
	result |= m; m &= b2m; m >>= 9;
	result |= m; m &= b2m; m >>= 9;
	result |= m;

	b2m = b4 & mask3;
	m = ((b2m >> 8) & b3) << 16;
	result |= m; m &= b2m; m <<= 8;
	result |= m; m &= b2m; m <<= 8;
	result |= m; m &= b2m; m <<= 8;
	result |= m; m &= b2m; m <<= 8;
	result |= m; m &= b2m; m <<= 8;
	result |= m;
	m = ((b2m << 8) & b3) >> 16;
	result |= m; m &= b2m; m >>= 8;
	result |= m; m &= b2m; m >>= 8;
	result |= m; m &= b2m; m >>= 8;
	result |= m; m &= b2m; m >>= 8;
	result |= m; m &= b2m; m >>= 8;
	result |= m;

	return result & ~(b3 | b4);
}


inline int Eval::getEndSortSearchLowerBound1(int alpha) {
    return -INFINITE + MAXSTEP + ((alpha >= 0) ? 0 : alpha);
}

inline int Eval::getEndSortSearchUpperBound(int beta) {
    return INFINITE - MAXSTEP + ((beta <= 0) ? 0 : beta);
}


int Eval::evaluate(Board &board,int player){
    int result;
    int whiteNum, blackNum;
    tie(ignore, whiteNum, blackNum) = board.countBoard();
    BitBoard my;
    BitBoard op;
    int empties = 8*8-whiteNum-blackNum;

    if(player== Board::BLACK){
        my = board.board;
        op = (~board.board)&board.used;

    } else{
        my = (~board.board)&board.used;
        op = board.board;
    }
    result = Eval::evaluate_direct(my,op,empties);


    if (result >= END_WIPEOUT_THRESHOLD)
        result += END_VERY_HIGH_EVAL_BONUS;
    if (empties <= END_FASTEST_FIRST_DEPTH) {
        if (result < getEndSortSearchUpperBound(0) && result > getEndSortSearchLowerBound1(0)) {
            result -= weighted_mobility_bits(mobility(op, my)) *
                              ( (result >= END_WIPEOUT_THRESHOLD) ? END_WIPEOUT_MOBILITY_WEIGHT : END_MOBILITY_WEIGHT);
        }
    } else if (result >= END_WIPEOUT_THRESHOLD) {
        result -= weighted_mobility_bits(mobility(op, my)) * (END_WIPEOUT_MOBILITY_WEIGHT - END_MOBILITY_WEIGHT);
    }
    printf("value= %d\n",result);

    return result;

}

int Eval::evaluate_direct(const BitBoard& my, const BitBoard& op,int empties) {
    if (my && op) { // wipeout detection
        int pattern[ACTUAL_PATTERNS];
        unsigned char* ucmy = (unsigned char*)&my;
        unsigned char* ucop = (unsigned char*)&op;
        int stage = rstage[empties];
        memcpy(pattern, patternOffset[stage], sizeof(pattern));
        int mybyte, opbyte, pCount, pCountM1;
        for (int i = 0; i < 8; i++) {
            mybyte = ucmy[i];
            pCount = pVCount[mybyte][i];
            pCountM1 = pCount - 1;
            unsigned char* pPtrMybyte = pPtr[mybyte][i];
            unsigned short* myPVMybyte = myPV[mybyte][i];
            for (int j = 0; j < pCountM1; j += 2) {
                pattern[pPtrMybyte[j]] += myPVMybyte[j];
                pattern[pPtrMybyte[j + 1]] += myPVMybyte[j + 1];
            }
            if (pCount & 1) {
                pattern[pPtrMybyte[pCountM1]] += myPVMybyte[pCountM1];
            }
            opbyte = ucop[i];
            pCount = pVCount[opbyte][i];
            pCountM1 = pCount - 1;
            unsigned char* pPtrOpbyte = pPtr[opbyte][i];
            unsigned short* opPVOpbyte = myPV[opbyte][i];
            for (int j = 0; j < pCountM1; j += 2) {
                pattern[pPtrOpbyte[j]] += (opPVOpbyte[j] << 1);
                pattern[pPtrOpbyte[j + 1]] += (opPVOpbyte[j + 1] << 1);
            }
            if (pCount & 1) {
                pattern[pPtrOpbyte[pCountM1]] += (opPVOpbyte[pCountM1] << 1);
            }
        }
        int sum = 0;
        for (int i = 0; i < ACTUAL_PATTERNS; i += 2) {
            sum += patternValues[pattern[i]] + patternValues[pattern[i + 1]];
        }
        if (empties & 1) sum += patternCorrection[stage];
        return sum;
    } else {
        return endToMid(getResult(my,empties));
    }
}

bool Eval::loadPatterns() {
    std::ifstream in("./pattern.bin", std::ios::binary | std::ios::in);
    if (!in) return false;
    float value;
    for (int i = 0; i < ALL_STAGE_PATTERN_COUNT; i++) {
        in.read((char*)&value, sizeof(value));
        if (!in) {
            in.close();
            return false;
        }
        patternValues[i] = (int)round(value * RULER);
    }
    for (int i = 0; i < STAGES; i++) {
        in.read((char*)&value, sizeof(value));
        if (!in) {
            in.close();
            return false;
        }
        patternCorrection[i] = (int)round(value);
    }
    in.close();
    return true;

}

bool Eval::initPatterns() {
    deltaSum = 0;
    occurance = 0;
    patternValues = new int[ALL_STAGE_PATTERN_COUNT];

    memset(myPV, 0, sizeof(myPV));
    //memset(opPV, 0, sizeof(opPV));
    memset(pPtr, 0, sizeof(pPtr));
    memset(pVCount, 0, sizeof(pVCount));
    memset(posDepend, 0, sizeof(posDepend));
    memset(patternDependCount, 0, sizeof(patternDependCount));
    int tempOffset[PATTERNS];
    int offset = 0;
    for (int i = 0; i < PATTERNS; i++) {
        tempOffset[i] = offset;
        offset += patternCount[i];
    }
    for (int i = 0; i < STAGES; i++) {
        offset = i * TOTAL_PATTERN_COUNT;
        for (int j = 0; j < ACTUAL_PATTERNS; j++) {
            patternOffset[i][j] = offset + tempOffset[patternPtr[j]];
        }
    }
    if (!loadPatterns()) {
        delete[] patternValues;
        patternValues = nullptr;
        return false;
    };
    {
        // line 1 + 2X
        addDependency(9, 0);
        multipleAdd(0, 1, 8, 0);
        addDependency(14, 0);

        addDependency(49, 1);
        multipleAdd(56, -8, 8, 1);
        addDependency(9, 1);

        addDependency(54, 2);
        multipleAdd(63, -1, 8, 2);
        addDependency(49, 2);

        addDependency(14, 3);
        multipleAdd(7, 8, 8, 3);
        addDependency(54, 3);

        // line 2
        multipleAdd(8, 1, 8, 4);
        multipleAdd(57, -8, 8, 5);
        multipleAdd(55, -1, 8, 6);
        multipleAdd(6, 8, 8, 7);

        // line 3
        multipleAdd(16, 1, 8, 8);
        multipleAdd(58, -8, 8, 9);
        multipleAdd(47, -1, 8, 10);
        multipleAdd(5, 8, 8, 11);

        // line 4
        multipleAdd(24, 1, 8, 12);
        multipleAdd(59, -8, 8, 13);
        multipleAdd(39, -1, 8, 14);
        multipleAdd(4, 8, 8, 15);

        // diag 8
        multipleAdd(0, 9, 8, 16);
        multipleAdd(56, -7, 8, 17);

        // diag 7
        multipleAdd(8, 9, 7, 18);
        multipleAdd(57, -7, 7, 19);
        multipleAdd(55, -9, 7, 20);
        multipleAdd(6, 7, 7, 21);

        // diag 6
        multipleAdd(16, 9, 6, 22);
        multipleAdd(58, -7, 6, 23);
        multipleAdd(47, -9, 6, 24);
        multipleAdd(5, 7, 6, 25);

        // diag 5
        multipleAdd(24, 9, 5, 26);
        multipleAdd(59, -7, 5, 27);
        multipleAdd(39, -9, 5, 28);
        multipleAdd(4, 7, 5, 29);

        // diag 4
        multipleAdd(32, 9, 4, 30);
        multipleAdd(60, -7, 4, 31);
        multipleAdd(31, -9, 4, 32);
        multipleAdd(3, 7, 4, 33);

        // 2x5 square
        multipleAdd(0, 1, 5, 34);
        multipleAdd(8, 1, 5, 34);

        multipleAdd(56, -8, 5, 35);
        multipleAdd(57, -8, 5, 35);

        multipleAdd(63, -1, 5, 36);
        multipleAdd(55, -1, 5, 36);

        multipleAdd(7, 8, 5, 37);
        multipleAdd(6, 8, 5, 37);

        multipleAdd(0, 8, 5, 38);
        multipleAdd(1, 8, 5, 38);

        multipleAdd(56, 1, 5, 39);
        multipleAdd(48, 1, 5, 39);

        multipleAdd(63, -8, 5, 40);
        multipleAdd(62, -8, 5, 40);

        multipleAdd(7, -1, 5, 41);
        multipleAdd(15, -1, 5, 41);

        // 3x3 square
        multipleAdd(0, 1, 3, 42);
        multipleAdd(8, 1, 3, 42);
        multipleAdd(16, 1, 3, 42);

        multipleAdd(56, -8, 3, 43);
        multipleAdd(57, -8, 3, 43);
        multipleAdd(58, -8, 3, 43);

        multipleAdd(63, -1, 3, 44);
        multipleAdd(55, -1, 3, 44);
        multipleAdd(47, -1, 3, 44);

        multipleAdd(7, 8, 3, 45);
        multipleAdd(6, 8, 3, 45);
        multipleAdd(5, 8, 3, 45);
    }

    for (int i = 0; i < 256; i++)
        for (int j = 0; j < 8; j++)
            if (i & (1 << (7 - j)))
                for (int k = 0; k < 8; k++)
                    for (int l = 0; l < ACTUAL_PATTERNS; l++) {
                        myPV[i][k][l] += (unsigned short)posDepend[k * HEIGHT + j][l];
                    }
    for (int i = 0; i < 256; i++)
        for (int j = 0; j < 8; j++)
            for (int k = 0; k < ACTUAL_PATTERNS; k++)
                if (myPV[i][j][k] != 0) {
                    pPtr[i][j][pVCount[i][j]] = (unsigned char)k;
                    //opPV[i][j][pVCount[i][j]] = myPV[i][j][k] << 1;
                    myPV[i][j][pVCount[i][j]++] = myPV[i][j][k];
                }
    return true;
}

void Eval::addDependency(int pos, int pInd) {
    int value = pow3[patternDependCount[pInd]++];
    posDepend[pos][pInd] = value;
}

void Eval::multipleAdd(int pos, int step, int count, int pInd) {
    for (int i = 0; i < count; i++) {
        addDependency(pos, pInd);
        pos += step;
    }
}

inline int Eval::weighted_mobility_bits(const BitBoard& mobility) {
    const BitBoard CORNER_MASK = 0x8100000000000081ull;
    return bits(mobility) + bits(mobility & CORNER_MASK);
}