LIBS_DIR=-Lxyq -LxServer
LIBS=-lxyq -lxServer
HEADERS_DIR=-Ixyq -IxServer

STD=-std=c++17
SRC=main.cpp xhttp.cpp
TARGET=main
CLIENT=xclient
all:clean libs $(TARGET) $(CLIENT) test
	clear

$(TARGET):$(SRC)
	$(CXX) $(STD) $(HEADERS_DIR) $(LIBS_DIR) $(LIBS) -w  $^  -o $@

$(CLIENT):xclient.cpp
	$(CXX) $(STD) $(HEADERS_DIR) $(LIBS_DIR) $(LIBS) -w $^ -o $@

test:test.cpp
	$(CXX) $(STD) $(HEADERS_DIR) $(LIBS_DIR) $(LIBS) -w  $^ -o $@
libs:
		make -C xServer
clean:
	$(RM) $(TARGET) $(CLIENT) test
