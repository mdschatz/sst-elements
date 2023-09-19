#!/bin/bash

TARGET=tricount_clean_$1
ARGS="$2_$3_$4_$5_$6_$7"
HFILE=configs/input_$ARGS.h
CCFILE=configs/input_$ARGS.cc

if [ $1 = "preload" ]; then
  ARG_IND=2
elif [ $1 = "with_gen" ]; then
  ARG_IND=8
else
  echo "Unrecognized option. Available options: preload, with_gen"
  exit
fi

. run_setup.sh

export VANADIS_EXE=$PWD/$TARGET.riscv64
export VANADIS_EXE_ARGS=${@: ${ARG_IND}}

# Build executable
make $TARGET && { time sst perlmutter.py | tee outs/$1_scaling_$2.out ; } 2> outs/time_$1_scaling_$2.out

# Build executable with logging and detailed timing
#make CFLAGS=-DLOG tricount_clean_preload
#{ time sst perlmutter.py ; } 2> outs/time_log_$1_scaling_$2.out && mv stdout-100 outs/log_$1_scaling_$2.out
