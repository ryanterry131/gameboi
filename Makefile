BINARY := gameboi
SRCS := $(shell find . -iname "*.c")
OBJS := $(addprefix build/,$(SRCS:.c=.o))
FRAMEWORKS := OpenGL CoreFoundation CoreGraphics CoreVideo IOKit AppKit

LD := clang
CC := $(LD)
override LDFLAGS += $(addprefix -framework ,$(FRAMEWORKS)) -lglfw3


all: build/$(BINARY)

build/$(BINARY): $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

build/%.o: %.c
	$(CC) -c $< -o $@


run:
	@build/$(BINARY)

clean:
	-rm -rf build/*
	-mkdir -p build/renderer
