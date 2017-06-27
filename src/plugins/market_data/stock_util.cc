//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2016骞�10鏈�13鏃� Author: zjc

#include "stock_util.h"

namespace stock_price {

StockUtil* StockUtil::instance_ = NULL;

int StockUtil::MONTH_DAY[] = {
    // Dec,Jan, ... ,Nov
    31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30
};

} /* namespace stock_logic */
