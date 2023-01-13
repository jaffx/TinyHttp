TARGET=xyq.o
SRC=xyq.cpp
STD=-std=c++11
LIBTARGET=libxyq.a

all:clean $(LIBTARGET)

$(LIBTARGET):$(TARGET)
	@$(AR) -cvr $@ $^

$(TARGET):$(SRC)
	$(CXX) -c $^ -o $@

clean:
	$(RM) $(TARGET) $(LIBTARGET)