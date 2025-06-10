#ifndef COMPILE_FUNC
#define COMPILE_FUNC

#include "Lang_funcs.h"
#include "Check_r_w_flags.h"
#include "Binary_tree_graph_debug.h"

#define SRC_FLAG "-s"
#define SRC_FLAG_LEN sizeof(SRC_FLAG) - 1
#define ASM_FLAG "-a"
#define ASM_FLAG_LEN sizeof(ASM_FLAG) - 1
#define EXE_FLAG "-o"
#define EXE_FLAG_LEN sizeof(EXE_FLAG) - 1
#define DBG_FLAG "-d"
#define DBG_FLAG_LEN sizeof(DBG_FLAG) - 1
#define MY_ASM_FLAG "-Ma"
#define MY_ASM_FLAG_LEN sizeof(MY_ASM_FLAG) - 1

const int Min_argc = 5; // call compiler + src flag + src file + option flag + output file

const int File_types_total = 5;
enum File_type_index {

    Src_file_index = 0x0,
    Asm_file_index = 0x1,
    Exe_file_index = 0x2,
    Dbg_file_index = 0x3,
    My_asm_file_index = 0x4
};

bool Compile_func(int argc, char* argv[]);
char** Check_flags(int argc, char* argv[]);

#endif
