CC = g++
AR = ar
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/Cls

SRC = \
Cls.cpp \
ClsFile.cpp

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

LIBS = -lCls -lCFile -lCOS -lCStrUtil -lCGlob -lncurses

CPPFLAGS = \
-I. \
-I$(INC_DIR) \
-I../../CFile/include \
-I../../COS/include \
-I../../CStrUtil/include \
-I../../CGlob/include \
-I../../CUtil/include \

LFLAGS = \
-L$(LIB_DIR) \
-L../../CFile/lib \
-L../../COS/lib \
-L../../CStrUtil/lib \
-L../../CGlob/lib \
-L../../CUtil/lib \

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCls.a
	$(RM) -f $(BIN_DIR)/Cls

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

.SUFFIXES: .cpp

$(OBJ_DIR)/main.o: main.cpp
	$(CC) -c main.cpp -o $(OBJ_DIR)/main.o $(CPPFLAGS)

$(LIB_DIR)/libCls.a: $(OBJS)
	$(AR) crv $(LIB_DIR)/libCls.a $(OBJS)

$(BIN_DIR)/Cls: $(OBJ_DIR)/main.o $(LIB_DIR)/libCls.a
	$(CC) $(LDEBUG) -o $(BIN_DIR)/Cls $(OBJ_DIR)/main.o $(LFLAGS) $(LIBS)
