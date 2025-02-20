#include "Worker.h"

int main(int argc, char** argv) {
    if(argc != 3) {
        cerr << "Usage: ./worker <terminate_second> <terminate_nano_second>" << endl;
        return 1;
    }

    int seconds = atoi(argv[1]);
    int nano = atoi(argv[2]);

    Worker worker(seconds, nano);
    worker.run();
    return 0;
}