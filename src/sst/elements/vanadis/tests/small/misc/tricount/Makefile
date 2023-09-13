ARCH=riscv64
CC=$(ARCH)-linux-musl-gcc
CXX=$(ARCH)-linux-musl-g++

CFLAGS:=$(CFLAGS) -O3
CXXFLAGS=-O3
LDFLAGS=-static

all: tricount_clean tricount_clean_preload tricount_clean_with_gen

% : %.cc
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@.$(ARCH) $< lutArrays.cc

clean:
	rm -r $(PROG).$(ARCH)
