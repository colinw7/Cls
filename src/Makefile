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

SRC = \
Cls.cpp \
ClsFile.cpp

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

LIBS = -lCls -lCOS -lCStrUtil -lCGlob -lncurses

CPPFLAGS = -I$(INC_DIR) -I.

LFLAGS = -L$(LIB_DIR)

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCls.a
	$(RM) -f $(EXEC_DIR)/Cls

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

.SUFFIXES: .cpp

$(OBJ_DIR)/main.o: main.cpp
	$(CC) $(CDEBUG) -o $(OBJ_DIR)/main.o -c main.cpp -I$(INC_DIR) -I.

$(LIB_DIR)/libCls.a: $(OBJS)
	$(AR) crv $(LIB_DIR)/libCls.a $(OBJS)

$(EXEC_DIR)/Cls: $(OBJ_DIR)/main.o $(LIB_DIR)/libCls.a
	$(CC) $(LDEBUG) -o $(EXEC_DIR)/Cls $(OBJ_DIR)/main.o $(LFLAGS) $(LIBS)
