#include "Frame_Table.h"

Frame_Table::Frame_Table() {
    frames.resize(TOTAL_FRAMES);
    eviction_flag = false;

    // Initialize all frames as free
    for (int i = 0; i < TOTAL_FRAMES; ++i) {
        frames[i].pid = -1; // -1 indicates a free frame
        frames[i].page_number = -1;
        frames[i].dirty_bit = false;
        frames[i].last_access_time = 0;
    }
}

const Frame& Frame_Table::get_frame(int index) const {
    if (index >= 0 && index < (int)(frames.size())) {
        return frames[index];
    } else {
        cerr << "Error: Frame index out of bounds: " << index << endl;
        exit(1);
    }
}

int Frame_Table::request_frame(int pid, int page_number, bool write) {

    //std::cout << "[DEBUG] Requesting frame for PID " << pid << " at page number " << page_number << "\n";

    if (page_number < 0 || page_number >= TOTAL_FRAMES) {
        cerr << "ERROR: Page number " << page_number << " is out of bounds!\n";
        exit(1);
    }

    if (!shared_clock) {
        //std::cerr << "ERROR: shared_clock is NULL when accessing Frame_Table!\n";
        exit(1);
    }


    for (int i = 0; i < TOTAL_FRAMES; ++i) {
        if (frames[i].pid == -1) {
            frames[i].pid = pid;
            frames[i].page_number = page_number;
            frames[i].dirty_bit = write;
            frames[i].last_access_time = (this->shared_clock->seconds * 1000000000ULL) + this->shared_clock->nanoseconds;
            eviction_flag = false;
            //std::cout << "[DEBUG] Frame " << i << " allocated to page " << page_number << "\n";
            return i;
        }
    }

    // If all frames are full, find the LRU frame to replace
    int lru_index = find_LRU_frame();
    Frame &lru_frame = frames[lru_index];

    // Check boundary
    if (page_number >= TOTAL_FRAMES) {
        //std::cerr << "ERROR: Page number " << page_number << " is out of bounds!\n";
        exit(1);
    }

    // Replace the LRU frame
    last_eviction.pid = lru_frame.pid;
    last_eviction.page_number = lru_frame.page_number;
    last_eviction.frame_index = lru_index;

    lru_frame.pid = pid;
    lru_frame.page_number = page_number;
    lru_frame.dirty_bit = write;
    lru_frame.last_access_time = (this->shared_clock->seconds * 1000000000ULL) + this->shared_clock->nanoseconds;
    eviction_flag = true;

    return lru_index;
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
    return lru_index;
}


void Frame_Table::release_frame(int pid) {
    for (int i = 0; i < TOTAL_FRAMES; ++i) {
        if (frames[i].pid == pid) {
            frames[i].pid = -1; // Free the frame
            frames[i].page_number = -1;
            frames[i].dirty_bit = false;
            frames[i].last_access_time = 0;
        }
    }
}


void Frame_Table::display_frame_table() {
    cout << "------------- Frame Table State --------------\n";
    for (int i = 0; i < (int)(frames.size()); ++i) {
        if (frames[i].pid != -1) {
            cout << "Frame " << i 
                      << " | PID: " << frames[i].pid 
                      << " | Page: " << frames[i].page_number 
                      << " | Dirty: " << frames[i].dirty_bit 
                      << " | Last Access Time: " << frames[i].last_access_time << "\n";
        } else {
            cout << "Frame " << i << ": Free\n";
        }
    }
    cout << "---------------------------------------------\n";
}

bool Frame_Table::evict_occurred() {
    return eviction_flag;
}

EvictionInfo Frame_Table::get_last_eviction() {
    eviction_flag = false; // Reset the flag after querying
    return last_eviction;
}

void Frame_Table::set_shared_clock(Shared_Clock* shared_clock) {
    if (shared_clock == nullptr) {
        //std::cerr << "ERROR: Attempting to set shared_clock to NULL!\n";
        exit(1);
    }
    this->shared_clock = shared_clock;
    //std::cout << "[DEBUG] Frame_Table linked to shared clock at address: " << this->shared_clock << "\n";
}