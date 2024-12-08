SRC_FILES = InstructionTable.h		\
			HLASMCompiler.c 		\
			E_format.c				\
			I_format.c				\
			IE_format.c				\
			MII_format.c			\
			RI_format.c				\
			RIE_format.c			\
			RIL_format.c			\
			RIS_format.c			\
			RR_format.c				\
			RRD_format.c			\
			RRE_format.c			\
			RRF_format.c			\
			RRS_format.c			\
			RS_format.c				\
			RSI_format.c			\
			RSL_format.c			\
			RSY_format.c			\
			RX_format.c				\
			RXE_format.c			\
			RXF_format.c			\
			RXY_format.c			\
			S_format.c				\
			SI_format.c				\
			SIL_format.c			\
			SIY_format.c			\
			SMI_format.c			\
			SS_format.c				\
			VRI_format.c			\
			VRR_format.c			\
			VRS_format.c			\
			VRV_format.c			\
			VRX_format.c			\
			VSI_format.c			\
			Main.c

OBJ_FILES = HLASMCompiler.o 		\
			E_format.o				\
			I_format.o				\
			IE_format.o				\
			MII_format.o			\
			RI_format.o				\
			RIE_format.o			\
			RIL_format.o			\
			RIS_format.o			\
			RR_format.o				\
			RRD_format.o			\
			RRE_format.o			\
			RRF_format.o			\
			RRS_format.o			\
			RS_format.o				\
			RSI_format.o			\
			RSL_format.o			\
			RSY_format.o			\
			RX_format.o				\
			RXE_format.o			\
			RXF_format.o			\
			RXY_format.o			\
			S_format.o				\
			SI_format.o				\
			SIL_format.o			\
			SIY_format.o			\
			SMI_format.o			\
			SS_format.o				\
			VRI_format.o			\
			VRR_format.o			\
			VRS_format.o			\
			VRV_format.o			\
			VRX_format.o			\
			VSI_format.o			\
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

