#!/bin/sh

PY_DIR="../../tests/correctness"

EXEC_DIR="../../experiments/mpi-hybrid"
if [ ! -d $EXEC_DIR ];
then
  echo "$EXEC_DIR doesn't exist"
else
  echo "Experiment Directory: $EXEC_DIR"
  cd $EXEC_DIR
fi

REF_DIR="../../experiments/serial-ref"
if [ ! -d $REF_DIR ];
then
  echo "$REF_DIR doesn't exist"
else
  echo "Reference Directory: $REF_DIR"
  cd $REF_DIR
fi

REPS=3
NODES=("1" "1" "1" "1" "2" "2")
PROCS=("1" "1" "1" "1" "2" "2")
THREADS=("1" "2" "3" "4" "3" "4")
DATAPOINTS="10000 100000 1000000 10000000"
DIMS="10"
outfile="${EXEC_DIR}/correctness-report.txt"

cd $PY_DIR

echo "Starting OMP Multi-GPU Correctness test..."
for N in $DATAPOINTS
do
  for dim in $DIMS
  do
    fref="${REF_DIR}/serial-${N}_$dim/1/progress.csv"
    for i in `seq -w 0 $((${#PROCS[@]}-1))`
    do
      for k in `seq -w 1 $REPS`
      do
        ftest="${EXEC_DIR}/mpi-hybrid-${N}_${dim}_${PROCS[$i]}_${THREADS[$i]}/$k/progress.csv"
        if [ -f $ftest ];
        then
          echo "Testing $ftest"
          python correctness.py -r $fref -t $ftest
        else
          echo "MISSING: $ftest"
        fi
      done
    done
  done
done
echo "MPI Multi-GPU Correctness test completed. Results in $outfile"
