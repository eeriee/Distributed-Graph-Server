//
// Created by zijun on 16-10-23.
//

// Edited by Xi Wang on Dec 18
#ifndef TOOLS_H
#define TOOLS_H

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

#define NUM_THREADS 2
#define NUM_PARTITION 3

using namespace std;

struct thread_data{
    int  thread_id;
    char *message;
};

#endif //ASSIGN2_TOOLS_H
