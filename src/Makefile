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
ClsFile.cpp \
ClsFilterData.cpp \
ClsMain.cpp \
CTime.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-std=c++14 \
-I. \
-I$(INC_DIR) \
-I../../CFileUtil/include \
-I../../CFile/include \
-I../../COS/include \
-I../../CStrUtil/include \
-I../../CGlob/include \
-I../../CUtil/include \

LFLAGS = \
-L$(LIB_DIR) \
-L../../CFileUtil/lib \
-L../../CFile/lib \
-L../../CRegExp/lib \
-L../../COS/lib \
-L../../CStrUtil/lib \
-L../../CGlob/lib \
-L../../CUtil/lib \

LIBS = \
-lCls -lCFileUtil -lCFile -lCOS -lCRegExp -lCStrUtil -lCGlob -lCUtil \
-ltre -lncurses

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCls.a
	$(RM) -f $(BIN_DIR)/Cls

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) $(CDEBUG) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

.SUFFIXES: .cpp

$(LIB_DIR)/libCls.a: $(OBJS)
	$(AR) crv $(LIB_DIR)/libCls.a $(OBJS)

$(BIN_DIR)/Cls: $(OBJ_DIR)/ClsMain.o $(LIB_DIR)/libCls.a
	$(CC) $(LDEBUG) -o $(BIN_DIR)/Cls $(OBJ_DIR)/ClsMain.o $(LFLAGS) $(LIBS)
