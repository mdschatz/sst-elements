#!/bin/bash

export VANADIS_EXE=$PWD/tricount_clean_with_gen.riscv64
export VANADIS_EXE_ARGS=$*

# Build executable
make tricount_clean_with_gen
{ time sst perlmutter.py | tee outs/with_gen_scaling_$2.out ; } 2> outs/time_with_gen_scaling_$2.out

# Build executable with logging and detailed timing
#make CFLAGS=-DLOG tricount_clean_with_gen
#{ time sst perlmutter.py ; } 2> outs/time_log_with_gen_scaling_$2.out && mv stdout-100 outs/log_with_gen_scaling_$2.out
