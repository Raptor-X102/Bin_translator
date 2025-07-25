#include "Processor_Funcs.h"
#include "Check_r_w_flags.h"

int main(int argc, char* argv[]) {

    char* input_file_name = NULL;

    if (!Check_r_w_flags(CHECK_R, argv, argc, &input_file_name)) {

        DEBUG_PRINTF("ERROR: flags check failed\n");
        return 0;
    }

    SPU_data processor = {};

    SPU_Ctor(&processor, input_file_name);
    DEBUG_PRINTF("\n1\n");

    DEBUG_PRINTF("&processor = %p", &processor);
    SPU_Run(&processor);
    DEBUG_PRINTF("\n2\n");

    SPU_Dump(&processor, Printf_Format_Function);
    SPU_Dtor(&processor);
    DEBUG_PRINTF("\n3\n");
}
