SRC_FILES = InstructionTable.h	\
			HLASMCompiler.c 	\
			E_format.c			\
			I_format.c			\
			IE_format.c			\
			MII_format.c		\
			RI_format.c			\
			RIE_format.c		\
			RIL_format.c		\
			RX_format.c			\
			Main.c

OBJ_FILES = HLASMCompiler.o 	\
			E_format.o			\
			I_format.o			\
			IE_format.o			\
			MII_format.o		\
			RI_format.o			\
			RIE_format.o		\
			RIL_format.o		\
			RX_format.o			\
			Main.o

FILE=""

all: compile link

debug: compile_debug link
	
compile:
	gcc -c $(SRC_FILES)

single:
	gcc -c -g $(FILE)

compile_debug:
	gcc -c -g $(SRC_FILES)

link:
	gcc -o compiler $(OBJ_FILES)

clean:
	rm *.o *.gch compiler

