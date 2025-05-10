#include "Frame_Table.h"

Frame_Table::Frame_Table() {
    frames.resize(TOTAL_FRAMES);
    for (int i = 0; i < TOTAL_FRAMES; ++i) {
        frames[i].pid = -1; // Initialize all frames to be free
        frames[i].page_number = -1;
        frames[i].dirty_bit = false;
        frames[i].last_access_time = 0;
    }
}

int Frame_Table::request_frame(int pid, int page_number, bool write) {
    for (int i = 0; i < TOTAL_FRAMES; ++i) {
        if (frames[i].pid == -1) { // Find a free frame
            frames[i].pid = pid;
            frames[i].page_number = page_number;
            frames[i].dirty_bit = write;
            frames[i].last_access_time = clock(); // get the current time
            return &frames[i] - &frames[0]; // Return the index of the frame
        }
    }

    // If all frames are full, find the LRU frame to replace
    int lru_index = find_LRU_frame();

    Frame &lru_frame = frames[lru_index];

    // Replace the LRU frame
    lru_frame.pid = pid;
    lru_frame.page_number = page_number;
    lru_frame.dirty_bit = write;
    lru_frame.last_access_time = clock(); // get the current time
    return lru_index; // Return the index of the replaced frame
}


int Frame_Table::find_LRU_frame() {
    int lru_index = 0;
    unsigned long long oldest_time = frames[0].last_access_time;
    for (int i = 1; i < TOTAL_FRAMES; ++i) {
        if (frames[i].last_access_time < oldest_time) {
            oldest_time = frames[i].last_access_time;
            lru_index = i;
        }
    }
    return lru_index; // Return the index of the LRU frame
}


void Frame_Table::release_frame(int pid) {
    for (int i = 0; i < TOTAL_FRAMES; ++i) {
        if (frames[i].pid == pid) { // Find the frame for the process
            frames[i].pid = -1; // Release the frame
            frames[i].page_number = -1;
            frames[i].dirty_bit = false;
            frames[i].last_access_time = 0;
        }
    }
}


void Frame_Table::display_frame_table() {
    cout << "Frame Table State:" << endl;
    for (int i = 0; i < frames.size(); ++i) {
        if (frames[i].pid != -1) {
            cout << "Frame " << i << ": PID = " << frames[i].pid
                 << ", Page Number = " << frames[i].page_number
                 << ", Dirty Bit = " << frames[i].dirty_bit
                 << ", Last Access Time = " << frames[i].last_access_time << endl;
        } else {
            cout << "Frame " << i << ": Free" << endl;
        }
    }
}