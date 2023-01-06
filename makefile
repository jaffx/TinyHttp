LIBS_DIR=-Lsrc/library/ 
HEADERS_DIR=-Isrc/headers/
LIBS=-l
SRC=main.cpp xhttp.cpp
TARGET=main
CLIENT=xclient
all:$(TARGET) $(CLIENT)

$(TARGET):$(SRC)
	$(CXX)  $^  -o $@

$(CLIENT):xclient.cpp
	$(CXX)  $^ -o $@

test:test.cpp xhttp.cpp
	$(CXX)  $^ -o $@

clean:
	$(RM) $(TARGET) $(CLIENT) test
