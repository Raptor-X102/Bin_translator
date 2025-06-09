/*#include "Exe_format_testing_funcs.h"
#include"Check_r_w_flags.h"
#include "Create_main.h"

int main(int argc, char* argv[]) {

    char* read_files[1] = {};
    char* write_files[1] = {};
    file_types files = {};
    files.read_files = read_files;
    files.write_files = write_files;
    if(!Check_r_w_flags(CHECK_R_W, argv, argc, &files)) {

        DEBUG_PRINTF("ERROR: flags verification failed");
        return 0;
    }

    Create_main(files.read_files[0], files.write_files[0], 256);

}
*/
