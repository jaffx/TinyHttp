SRC=xServer.cpp
TARGET=xServer.o
LIBTARGET=libxServer.a
STD=-std=c++17 -Wc++11-extensions
LIBDIR=-L../xyq
LIBS=-lxyq
HEADER_DIR=-I../xyq
all: clean $(TARGET) $(LIBTARGET)


$(TARGET):$(SRC)
	$(CXX) -c -w $(STD) $^ -o $@

$(LIBTARGET):$(TARGET)
	$(AR) -cvr $(LIBTARGET) $(TARGET)

clean:
	$(RM) $(TARGET) $(LIBTARGET)