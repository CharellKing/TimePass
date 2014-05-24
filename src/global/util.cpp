/* COPYRIGHT:   Copyright 2014 CharellkingQu
 * LICENSE:     GPL
 * AUTHOR:      CharellkingQu
 * DATE :       2014-05-16
 */

#include "global/util.h"

#include <stdio.h>
#include <string.h>


/**
 * justy whether the system is big endian or not
 * @return true for yes, false for no
 */
bool TimePass::Util::IsBigEndian() {
    union EndianData {
        char data[4];
        int  value;
    };

    union EndianData d;
    d.value = 0x12345678;
    return 0x12 == d.data[0];
}

/**
 * zero a data block
 * @param p_addr the block's initial address
 * @param size the data block's byte-size
 */
void TimePass::Util::Zero(void* p_addr, size_t size) {
    memset(reinterpret_cast<char*>(p_addr), 0, size);
}

