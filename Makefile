
TARGET = example
READER = tscreader
OBJS = tscplugin.o example.o
READEROBJS = tscreader.o
CFLAGS = -g -std=gnu99
LDFLAGS = -lrt

%.o : %.c
	$(CC) -c $(CFLAGS) -o $@ $^

.PHONY: all clean

all: $(TARGET) $(READER)

$(TARGET) : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
$(READER) : $(READEROBJS)
	$(CC) $(LDFLAGS) -o $@ $^

tscplugin.o : tscplugin.c


clean:
	rm -f $(TARGET) $(READER)
	rm -f *.o
