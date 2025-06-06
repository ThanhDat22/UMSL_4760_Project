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

// Eviction information structure
struct EvictionInfo {
    int pid;
    int page_number;
    int frame_index;
};

struct Frame {
    int pid;
    int page_number;
    bool dirty_bit;
    unsigned long long last_access_time; // Time of last access
};

class Frame_Table {
public:
    Frame_Table();
    void set_shared_clock(Shared_Clock*); // Set the shared clock
    const Frame& get_frame(int index) const;
    int request_frame(int, int, bool); // Request a frame for a process
    void release_frame(int); // Release a frame for a process
    void display_frame_table(); // Display the frame table
    bool evict_occurred(); 
    EvictionInfo get_last_eviction();

private:
    vector<Frame> frames;
    int find_LRU_frame();
    bool eviction_flag;
    EvictionInfo last_eviction;
    Shared_Clock* shared_clock; // Pointer to the shared clock
};


#endif