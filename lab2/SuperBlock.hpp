//
// Created by zijun on 16-10-23.
//

#ifndef ASSIGN2_SUPERBLOCK_H
#define ASSIGN2_SUPERBLOCK_H

#include "tools.hpp"

class SuperBlock {
public:
    u64 checksum;
    u32 current_generation;
    u32 start_log;
    u32 size_log;
    u32 block_count; //checkpoint
    //u64 empty[509];
};
#endif //ASSIGN2_SUPERBLOCK_H
