LIBS_DIR=-Lxyq -LxServer
LIBS=-lxyq -lxServer
HEADERS_DIR=-Ixyq -IxServer

STD=-std=c++17
SRC=main.cpp
TARGET=main
all:clean libs $(TARGET) test
	clear

$(TARGET):$(SRC)
	$(CXX) $(STD) $(HEADERS_DIR) $(LIBS_DIR) $(LIBS) -w  $^  -o $@


test:test.cpp
	$(CXX) $(STD) $(HEADERS_DIR) $(LIBS_DIR) $(LIBS) -w  $^ -o $@
libs:
		make -C xServer
		make -C xyq
clean:
	$(RM) $(TARGET) $(CLIENT) test
