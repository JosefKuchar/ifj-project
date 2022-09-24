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
.PHONY: run clean zip test pdf

# Main target
main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

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
	rm -rf .deps send_test *.o *.out *.aux *.log dokumentace.pdf main xkucha28.zip

# PDF documentation
pdf:
	pdflatex -interaction nonstopmode doc/dokumentace.tex

# Pack for submission
zip: pdf
	zip xkucha28.zip *.c *.h dokumentace.pdf Makefile rozdeleni

# Submission test
test: pdf zip
	./is_it_ok.sh xkucha28.zip send_test
