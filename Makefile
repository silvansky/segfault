CC=g++
OPTS=-Wall -pedantic
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.c=.o)

TARGET=segfault

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^

.cpp.o:
	$(CC) -c $(OPTS) $<

clean:
	rm $(TARGET) $(OBJECTS)

install:
	cp $(TARGET) /usr/local/bin/

uninstall:
	rm /usr/local/bin/$(TARGET)

