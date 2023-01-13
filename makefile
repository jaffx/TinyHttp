LIBS_DIR=-Lxyq
LIBS=-lxyq
HEADERS_DIR=-Ixyq

STD=-std=c++17
SRC=main.cpp xhttp.cpp
TARGET=main
CLIENT=xclient
all:clean $(TARGET) $(CLIENT)
	clear

$(TARGET):$(SRC)
	$(CXX) $(STD) $(HEADERS_DIR) $(LIBS_DIR) $(LIBS) -w  $^  -o $@

$(CLIENT):xclient.cpp
	$(CXX) $(STD) $(HEADERS_DIR) $(LIBS_DIR) $(LIBS) -w $^ -o $@

test:test.cpp 
	$(CXX) $(STD) $(HEADERS_DIR) $(LIBS_DIR) $(LIBS) -w  $^ -o $@

clean:
	$(RM) $(TARGET) $(CLIENT) test
