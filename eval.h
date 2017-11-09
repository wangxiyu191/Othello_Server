//
// Created by 王悉宇 on 2017/11/8.
//

#ifndef OTHELLO_SERVER_EVAL_H
#define OTHELLO_SERVER_EVAL_H

#include "board.hpp"
#include <fstream>
#include <cmath>
#include <tuple>
using namespace std;


class Eval {
public:

    static const int HEIGHT = 8;
    static const int MAXSTEP = 8 * 8;
    static int evaluate_direct(const BitBoard &my, const BitBoard &op,int empties);

    static const int RULER = 10000;
    static const int INFINITE = 1000000000;

    static const int STAGES = 17;
    static const int PATTERNS = 11;
    static const int ACTUAL_PATTERNS = 46;
    static const int MAX_PATTERN_COUNT = 59049;
    static const int TOTAL_PATTERN_COUNT = 167265;
    static const int ALL_STAGE_PATTERN_COUNT = STAGES * TOTAL_PATTERN_COUNT;

    static const int patternPtr[ACTUAL_PATTERNS];
    static const int rstage[MAXSTEP + 1];


    static int *patternValues;
    static float *deltaSum;
    static int *occurance;
    static int patternCorrection[STAGES];
    static float correctionDeltaSum[STAGES];
    static int correctionOccurance[STAGES];


    static unsigned short myPV[256][8][ACTUAL_PATTERNS]; // max 59049 < 65535
    //static int opPV[256][8][ACTUAL_PATTERNS]; // since opPV = 2 * myPV
    static unsigned char pPtr[256][8][ACTUAL_PATTERNS];
    static unsigned char pVCount[256][8];
    static int posDepend[MAXSTEP][ACTUAL_PATTERNS];
    static int patternDependCount[ACTUAL_PATTERNS];
    static int patternOffset[STAGES][ACTUAL_PATTERNS];
    static const int patternCount[PATTERNS];

    // fastest-first heuristic
    static const int MID_WIPEOUT_MOBILITY_WEIGHT = (int)(3.0 * RULER);
    static const int MID_WIPEOUT_THRESHOLD = 70 * RULER;
    static const int MID_VERY_HIGH_EVAL_BONUS = 10 * RULER;
    static const int END_MOBILITY_WEIGHT = (int)(3.0 * RULER);
    static const int END_WIPEOUT_MOBILITY_WEIGHT = (int)(6.0 * RULER);
    static const int END_WIPEOUT_THRESHOLD = 60 * RULER;
    static const int END_VERY_HIGH_EVAL_BONUS = 20 * RULER;
    static const int END_FASTEST_FIRST_DEPTH = 20;

    static inline int endToMid(int value);
    static bool initPatterns();
    static void addDependency(int pos, int pInd);
    static void multipleAdd(int pos, int step, int count, int pInd);
    static bool loadPatterns();

    static const int pow3[10];

    static int bits(const BitBoard &bb);

    static int getResult(const BitBoard &my, int emtpies);

    static int weighted_mobility_bits(const BitBoard &mobility);

    static int evaluate(Board &board,int player);

    static BitBoard mobility(const BitBoard &b1, const BitBoard &b2);

    static int getEndSortSearchLowerBound1(int alpha);

    static int getEndSortSearchUpperBound(int beta);
};


#endif //OTHELLO_SERVER_EVAL_H
