# Компилятор и флаги
CXX = g++
XASM = nasm
CXXFLAGS = -D_USE_MATH_DEFINES -mavx2 -msse4.2 -O3 -Wall -Wextra -std=c++17 -MMD -MP -Wformat=2 -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal \
-Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual \
-Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual \
-Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE
XASMFLAGS = -f elf64 -ggdb3 -F dwarf

DEBUG = NO_DEBUG
# Директории исходников и объектных файлов
LANGUAGE = Programminng_language
LIBS = libs
BINARY_TREES = $(LANGUAGE)/Binary_Trees
PROCESSOR = $(LANGUAGE)/Processor
STACK = $(BINARY_TREES)/Stack
SRC_DIRS = src $(LIBS)/Get_file_size $(LIBS)/Check_flags $(LIBS)/Dynamic_array $(LIBS)/Memcpy_upgrade $(LIBS)/Memdup $(LANGUAGE)/src $(BINARY_TREES)/src $(PROCESSOR)/src $(STACK)/src
SRC_CPP = $(wildcard $(addsuffix /*.cpp,$(SRC_DIRS)))
SRC_ASM = $(wildcard $(addsuffix /*.asm,$(SRC_DIRS)))

OBJ_DIR = obj
OBJ_DIR_ASM = $(OBJ_DIR)/asm
OUTDIR = build
ASM_DIR = asm

# Правильное формирование путей к объектным файлам
OBJ_CPP = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_CPP))
OBJ_ASM = $(patsubst %.asm,$(OBJ_DIR_ASM)/%.o,$(SRC_ASM))

# Выходной файл
OUT = $(OUTDIR)/RXcc.exe

# Цель по умолчанию
all: $(OUT)

# Включение зависимостей
-include $(OBJ_CPP:.o=.d)

# Очистка
clean:
	rm -rf $(OBJ_DIR) $(OUTDIR)

# Универсальное правило для .cpp файлов
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@$(CXX) -D $(DEBUG) $(CXXFLAGS) -Iheaders -I $(LIBS)/Get_file_size -I $(LIBS)/Check_flags -I $(LIBS)/Dynamic_array -I $(LIBS)/Memcpy_upgrade -I $(LIBS)/Memdup -I$(LANGUAGE)/headers -I$(BINARY_TREES)/headers -I$(PROCESSOR)/headers -I$(STACK)/headers -c $< -o $@
	@echo "Compiled $<"

# Компиляция .asm файлов
$(OBJ_DIR_ASM)/%.o: %.asm
	@mkdir -p $(@D)
	@$(XASM) $(XASMFLAGS) $< -o $@
	@echo "Assembled $<"

# Линковка
$(OUT): $(OBJ_CPP)
	@mkdir -p $(@D)
	@$(CXX) -o $@ $^
	@echo "Linking completed: $@"

compile_asm:
	$(CXX) -D $(DEBUG) -S -masm=intel -c $(SRC_CPP) $(CXXFLAGS) -Iheaders -Ilist/headers -I $(LIBS)/Get_file_size -I $(LIBS)/Check_flags
	mv *.s $(ASM_DIR)

NO_ASLR_FLAG = -Wl,--disable-dynamicbase

create_main:
	$(CXX) -Wall -Wextra $(NO_ASLR_FLAG) $(FILE) -o $(TARGET)

compile_x64:
	./build/RXcc.exe -rf0 Programminng_language/other/Input_data/Solve_quad_eq.txt -rf1 src/In_out_modules/In_out_modules.cpp -wf0 Programminng_language/other/Debug/Lang_debug.dot -wf1 First_exe.cpp

compile_nasm:
	./build/RXcc.exe -rf0 Programminng_language/other/Input_data/Solve_quad_eq.txt -rf1 In_out_modules/In_out_modules.cpp -wf0 Programminng_language/other/Debug/Lang_debug.dot -wf1 First_exe.asm

nasm:
	nasm -f win64 First_exe.asm
	$(CXX) -O0 First_exe.obj -o First_exe_nasm

compile_all:
	./build/RXcc.exe -s Programminng_language/other/Input_data/Solve_quad_eq.txt -a First_exe.asm -d Programminng_language/other/Debug/Lang_debug.dot -o First_exe -Ma Programminng_language/other/asm_files/My_asm1.asm

compile_my_asm:
	./build/RXcc.exe -s Programminng_language/other/Input_data/Code_example.txt -Ma Programminng_language/other/asm_files/My_asm1.asm -d Programminng_language/other/Debug/Lang_debug.dot
