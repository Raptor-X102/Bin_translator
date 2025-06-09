#include "Compile_func.h"

int main(int argc, char* argv[]) {

    /*char* read_files[2] = {};
    char* write_files[2] = {};
    file_types files = {};
    files.read_files = read_files;
    files.write_files = write_files;
    if(!Check_r_w_flags(CHECK_R_W, argv, argc, &files)) {

        DEBUG_PRINTF("ERROR: flags verification failed")
        return 0;
    }*/

    if(!Compile_func(argc, argv))
        return 1;
}
