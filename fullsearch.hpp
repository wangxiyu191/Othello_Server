//
//  fullsearch.hpp
//  Othello_Server
//
//  Created by 王悉宇 on 2017/11/7.
//  Copyright © 2017年 王悉宇. All rights reserved.
//

#ifndef fullsearch_hpp
#define fullsearch_hpp

#include <cstring>
#include <iostream>
#include <tuple>
#include <vector>
#include <algorithm>
#include <random>
#include <climits>
#include <cmath>
#include <limits>
#include "board.hpp"

class FullSearch
{
    const static int WIN = 1;
    const static int TIE = 0;
    const static int LOST = -1;
    
public:
    FullSearch();
    ~FullSearch();
    int search(Board &board);
    Position getNextAction(Board &board);
    
};
#endif /* fullsearch_hpp */
