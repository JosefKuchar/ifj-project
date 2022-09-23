# Compiler settings
CC = gcc
CFLAGS = -std=c11 -g -O2 -Wall -Wextra -Werror -pedantic

# Get all .c files
SRCS = $(wildcard *.c)
# Get corresponding .o files
OBJS := $(SRCS:%.c=%.o)
# Get corresponding .d files
DEPS := $(SRCS:%.c=.deps/%.d)

# These will run every time (not just when the files are newer)
.PHONY: run clean zip

# Main target
main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Dependecies
%.o: %.c .deps/%.d | .deps
	$(CC) -MT $@ -MMD -MP -MF .deps/$*.d $(CFLAGS) -c $(OUTPUT_OPTION) $<
.deps:
	@mkdir -p $@
$(DEPS):
include $(wildcard $(DEPS))

# Run the program
run: main
	./main

# Clean up
clean:
	rm -rf .deps *.o *.out xkucha28.zip

# Pack for submission
zip:
	zip xkucha28.zip *.c *.h Makefile rozdeleni
