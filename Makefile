# Compiler settings
CC = gcc
CFLAGS = -std=c11 -g -O2 -Wall -Wextra -Werror -pedantic

# Get all .c files
SRCS = $(wildcard *.c)
# Get corresponding .o files
OBJS := $(SRCS:%.c=%.o)
# Get corresponding .d files
DEPS := $(SRCS:%.c=%.d)

# These will run every time (not just when the files are newer)
.PHONY: run clean zip test pdf

# Main target
main: $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ -lm

# Dependecies
%.o: %.c %.d
	$(CC) -MT $@ -MMD -MP -MF $*.d $(CFLAGS) $(CPPFLAGS) -c $(OUTPUT_OPTION) $<
$(DEPS):
include $(wildcard $(DEPS))

# Run the program
run: main
	./main < php/test.php > test.ifjc22
	./ic22int test.ifjc22

# Clean up
clean:
	rm -rf send_test *.d *.o *.a *.out *.aux *.log dokumentace.pdf main run_tests xkucha28.zip

# PDF documentation
pdf:
	pdflatex -interaction nonstopmode doc/dokumentace.tex

# Pack for submission
zip: pdf
	zip xkucha28.zip *.c *.h dokumentace.pdf Makefile rozdeleni

# Submission test
submission_test: pdf zip
	./is_it_ok.sh xkucha28.zip send_test

# Tests
GTEST_DIR=test/lib/googletest
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)
CPPFLAGS_T += -isystem $(GTEST_DIR)/include
CXXFLAGS_T += -g -Wall -Wextra -pthread

TEST_OBJS := $(filter-out main.o, $(OBJS))

gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS_T) -I$(GTEST_DIR) $(CXXFLAGS_T) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS_T) -I$(GTEST_DIR) $(CXXFLAGS_T) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

tests.o: $(GTEST_HEADERS) test/tests.cpp
	$(CXX) $(CPPFLAGS_T) $(CXXFLAGS_T) -c test/tests.cpp

test: tests.o gtest_main.a $(TEST_OBJS)
	$(CXX) $(CPPFLAGS_T) $(CXXFLAGS_T) -lm -lpthread $^ -o run_tests && ./run_tests
