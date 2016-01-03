SRC_FILES = transaction.cpp transaction_test.cpp
LIB_SRC_FILES = transaction.cpp lazy.cpp
CC_FLAGS = -Wall -std=c++14
CC = clang++
TARGET = nstd

all:
	${CC} ${SRC_FILES} ${CC_FLAGS} -o ${TARGET}

library:
	${CC} ${SRC_FILES} ${CC_FLAGS} -o ${TARGET}

clean:
	rm -f ${TARGET}