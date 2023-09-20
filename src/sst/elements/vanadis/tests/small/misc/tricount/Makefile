ARCH=riscv64
CC=$(ARCH)-linux-musl-gcc
CXX=$(ARCH)-linux-musl-g++

CFLAGS:=$(CFLAGS) -O3 -fopenmp
CXXFLAGS=-O3
LDFLAGS=-static

all: tricount_clean tricount_clean_preload tricount_clean_with_gen task_parallel

%_preload : %_preload.cc
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@.$(ARCH) $< lutArrays.cc

%_parallel : %_parallel.cc
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@.$(ARCH) $< lutArrays.cc

% : %.cc
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@.$(ARCH) $<

clean:
	rm -r $(PROG).$(ARCH)
