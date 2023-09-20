#!/bin/bash

TARGET=task_$1
TASK_ARGS="$2_$3_$4_$5_$6_$7_$8"
HFILE=configs/input_$TASK_ARGS.h
CCFILE=configs/input_$TASK_ARGS.cc

if [ ! $1 = "serial" ] && [ ! $1 = "parallel" ]; then
  echo "Unrecognized option. Available options: serial, parallel"
  exit
fi

. run_setup.sh

export VANADIS_EXE=$PWD/$TARGET.riscv64
export VANADIS_EXE_ARGS=${@: 9}

# Build executable
make $TARGET && { time sst perlmutter.py | tee outs/$1_scaling_$2.out ; } 2> outs/time_$1_scaling_$2.out

# Build executable with logging and detailed timing
#make CFLAGS=-DLOG tricount_clean_preload
#{ time sst perlmutter.py ; } 2> outs/time_log_$1_scaling_$2.out && mv stdout-100 outs/log_$1_scaling_$2.out
