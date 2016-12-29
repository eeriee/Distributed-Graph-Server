//
// Created by zijun on 16-10-23.
//

#ifndef ASSIGN2_TOOLS_H
#define ASSIGN2_TOOLS_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <map>
#include <set>
#include <climits>
#include <vector>
#include <queue>
#include <inttypes.h>
#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include "mongoose.h"
#include "frozen.h"

typedef uint64_t u64;
typedef uint32_t u32;
typedef int64_t i64;

#define BLOCK_SIZE 4096
#define LOG_SIZE 2147483648
#define CHECKPOINT_SIZE 8589934592
#define TOTAL_LOG 524287
#define TOTAL_ENTRY 170

using namespace std;

struct thread_data{
    int  thread_id;
    char *message;
};

#endif //ASSIGN2_TOOLS_H
