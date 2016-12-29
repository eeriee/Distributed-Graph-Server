//
// Created by zijun on 16-10-23.
//

#ifndef ASSIGN2_LOG_H
#define ASSIGN2_LOG_H

#include "tools.hpp"

class Entry {
public:
    u32 opcode;
    u64 node_a_id;
    u64 node_b_id;
    //24-byte

    Entry() = default;
    Entry(u32 opcode, u64 node_a_id, u64 node_b_id) {
        this->opcode = opcode;
        this->node_a_id = node_a_id;
        this->node_b_id = node_b_id;
    }
};

class LogBlock {
public:
    u64 checksum;
    u32 generation_number;
    u32 entry_count;

    Entry entries[170];

    LogBlock() = default;

    LogBlock(u64 checksum, u32 generation_number, u32 entry_count) {
        this->checksum = checksum;
        this->generation_number = generation_number;
        this->entry_count = entry_count;
    }
};
#endif //ASSIGN2_LOG_H
