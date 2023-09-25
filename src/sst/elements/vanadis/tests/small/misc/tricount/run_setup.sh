# NOTE: Intended as helper script to bundle common routines

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
touch configs/input_$TASK_ARGS.h
ln -s configs/input_$TASK_ARGS.h lutArrays.h

if [ -L lutArrays.cc ]; then
  unlink lutArrays.cc
fi
touch configs/input_$TASK_ARGS.h
ln -s configs/input_$TASK_ARGS.cc lutArrays.cc

# Ensure output directory exists
if [ ! -d "./outs/" ]; then
  mkdir -p ./outs
fi
