/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-16
 */

#include "global/util.h"

#include <stdio.h>
#include <string.h>


/*是否是大端字节序列*/
bool TimePass::Util::IsBigEndian() {
    union EndianData {
        char data[4];
        int  value;
    };

    union EndianData d;
    d.value = 0x12345678;
    return 0x12 == d.data[0];
}

/*将数组清零*/
void TimePass::Util::Zero(void* p_addr, size_t size) {
    memset(reinterpret_cast<char*>(p_addr), 0, size);
}

