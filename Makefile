CC ?= gcc
CFLAGS ?= -O2 -std=c11 -Wall -Wextra
LDLIBS ?= -lm

TARGET := numeric_diff_experiment
SRC := numeric_diff_experiment.c

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) numeric_diff_data.csv numeric_diff_summary.csv *.o
