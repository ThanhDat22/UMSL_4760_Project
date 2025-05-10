#ifndef FRAME_TABLE_H
#define FRAME_TABLE_H

#include "Shared_Clock.h"

#include <vector>
#include <map>
#include <iostream>

using std::vector;
using std::map;

const int TOTAL_FRAMES = 256;
const int PAGE_SIZE = 512;
const int MAX_PROCESSES = 18;

struct Frame {
    int pid;
    int page_number;
    bool dirty_bit;
    unsigned long long last_access_time; // Time of last access
};

class Frame_Table {
public:
    Frame_Table();
    int request_frame(int, int, bool); // Request a frame for a process
    void release_frame(int); // Release a frame for a process
    void display_frame_table(); // Display the frame table 

private:
    vector<Frame> frames;
    int find_LRU_frame();
};


#endif