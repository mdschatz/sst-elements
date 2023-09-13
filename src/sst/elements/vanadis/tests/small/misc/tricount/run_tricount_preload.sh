#!/bin/bash

ARGS="$1_$2_$3_$4_$5_$6"
HFILE=configs/input_$ARGS.h
CCFILE=configs/input_$ARGS.cc

# Remove initial template file if they exist
if [ -f lutArrays.h ] && [ ! -L lutArrays.h ]; then
  rm lutArrays.h
fi

if [ -f lutArrays.cc ] && [ ! -L lutArrays.cc ]; then
  rm lutArrays.cc
fi

# Ensure compilation files exist
if [ ! -f $HFILE ]; then
  echo "Header $HFILE does not exist"
  exit
fi

if [ ! -f $CCFILE ]; then
  echo "Source $CCFILE does not exist"
  exit
fi

# Point symlinks to the correct input file
if [ -L lutArrays.h ]; then
  unlink lutArrays.h
fi
ln -s configs/input_$ARGS.h lutArrays.h

if [ -L lutArrays.cc ]; then
  unlink lutArrays.cc
fi
ln -s configs/input_$ARGS.cc lutArrays.cc

# Ensure output directory exists
if [ ! -d "./outs/" ]; then
  mkdir -p ./outs
fi


export VANADIS_EXE=$PWD/tricount_clean_preload.riscv64
export VANADIS_EXE_ARGS=$ARGS

# Build executable
make tricount_clean_preload
{ time sst perlmutter.py | tee outs/scaling_$2.out ; } 2> outs/time_scaling_$2.out

# Build executable with logging and detailed timing
#make CFLAGS=-DLOG tricount_clean_preload
#{ time sst perlmutter.py ; } 2> outs/time_log_scaling_$2.out && mv stdout-100 outs/log_scaling_$2.out
