CC = g++
ifeq ($(std), c++11)
    CFLAGS = -g -c -Wall -std=c++11
else
    CFLAGS = -g -c -Wall -std=c++0x
endif
OBJ = lab4.o MemoryTester.o Memory.o Tools.o RegisterFile.o \
RegisterFileTester.o ConditionCodes.o ConditionCodesTester.o
.C.o:
	$(CC) $(CFLAGS) $< -o $@

lab4: $(OBJ)
	$(CC) $(OBJ) -o lab4

lab4.o: Memory.h RegisterFile.h MemoryTester.h RegisterFileTester.h ConditionCodes.h ConditionCodesTester.h

MemoryTester.o: Memory.h MemoryTester.h

Memory.o: Memory.h Tools.h

Tools.o: Tools.h

RegisterFile.o: RegisterFile.h RegisterFileTester.h

RegisterFileTester.o: RegisterFile.h RegisterFileTester.h

ConditionCodes.o: ConditionCodes.h

ConditionCodesTester.o: ConditionCodes.h ConditionCodesTester.h

clean:
	rm $(OBJ) lab4

run:
	make clean
	make lab4
	./run.sh
