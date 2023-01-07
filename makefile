LIBS_DIR=-Lsrc/library/ 
HEADERS_DIR=-Isrc/headers/
LIBS=-lpthread
STD=-std=c++17
SRC=main.cpp xhttp.cpp
TARGET=main
CLIENT=xclient
all:$(TARGET) $(CLIENT)

$(TARGET):$(SRC)
	$(CXX) $(STD)  $^  -o $@

$(CLIENT):xclient.cpp
	$(CXX)  $^ -o $@

test:test.cpp xhttp.cpp
	$(CXX)  $^ -o $@

clean:
	$(RM) $(TARGET) $(CLIENT) test
