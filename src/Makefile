CC = g++
AR = ar
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR  = ../include
OBJ_DIR  = ../obj
LIB_DIR  = ../lib
EXEC_DIR = ../exec

all: $(EXEC_DIR)/Cls

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(EXEC_DIR)/Cls

ClsI.h: $(INC_DIR)/Cls.h

Cls.cpp: ClsI.h

ClsFile.cpp: ClsI.h $(INC_DIR)/ClsFile.h

CGlob.cpp: CGlob.h

main.cpp: $(INC_DIR)/Cls.h

.SUFFIXES: .cpp

$(OBJ_DIR)/Cls.o: Cls.cpp CGlob.cpp
	$(CC) $(CDEBUG) -o $(OBJ_DIR)/Cls.o -c Cls.cpp -I$(INC_DIR) -I.

$(OBJ_DIR)/ClsFile.o: ClsFile.cpp
	$(CC) $(CDEBUG) -o $(OBJ_DIR)/ClsFile.o -c ClsFile.cpp -I$(INC_DIR) -I.

$(OBJ_DIR)/CGlob.o: CGlob.cpp
	$(CC) $(CDEBUG) -o $(OBJ_DIR)/CGlob.o -c CGlob.cpp -I$(INC_DIR) -I.

$(OBJ_DIR)/main.o: main.cpp
	$(CC) $(CDEBUG) -o $(OBJ_DIR)/main.o -c main.cpp -I$(INC_DIR) -I.

$(LIB_DIR)/libCls.a: $(OBJ_DIR)/Cls.o $(OBJ_DIR)/ClsFile.o $(OBJ_DIR)/CGlob.o
	$(AR) crv $(LIB_DIR)/libCls.a $(OBJ_DIR)/*.o

$(EXEC_DIR)/Cls: $(OBJ_DIR)/main.o $(LIB_DIR)/libCls.a
	$(CC) $(LDEBUG) -o $(EXEC_DIR)/Cls $(OBJ_DIR)/main.o -lCls -L$(LIB_DIR) -lncurses
