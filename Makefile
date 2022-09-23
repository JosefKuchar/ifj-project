CC = gcc
CFLAGS = -std=c11 -g -O2 -Wall -Wextra -Werror -pedantic
SRCS = $(wildcard *.c)

.PHONY: all run clean zip

all: main

run: main
	./main

clean:
	rm -f *.o *.out xkucha28.zip

zip:
	zip xkucha28.zip *.c *.h Makefile rozdeleni

COMPILE = $(CC) -MT $@ -MMD -MP -MF .deps/$*.d $(CFLAGS) -c

%.o: %.c
%.o: %.c .deps/%.d | .deps
	$(COMPILE) $(OUTPUT_OPTION) $<

.deps: ; @mkdir -p $@

DEPS := $(SRCS:%.c=.deps/%.d)
$(DEPS):

include $(wildcard $(DEPS))
