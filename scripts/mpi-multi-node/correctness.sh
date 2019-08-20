#!/bin/sh

PY_DIR="../../tests/correctness"

EXEC_DIR="../../experiments/mpi-multi-node"
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
NODES=("1" "2" "3" "4" "6" "8")
DATAPOINTS="10000 100000 1000000 10000000"
DIMS="10"
outfile="${EXEC_DIR}/correctness-report.txt"

cd $PY_DIR

echo "Starting MPI Multi-Node Correctness test..."  > $outfile
for N in $DATAPOINTS
do
  for dim in $DIMS
  do
    fref="${REF_DIR}/serial-${N}_$dim/1/progress.csv"
    for i in `seq -w 0 $((${#NODES[@]}-1))`
    do
      for k in `seq -w 1 $REPS`
      do
        ftest="${EXEC_DIR}/mpi-multi-node-${N}_${dim}_${NODES[$i]}/$k/progress.csv"
        if [ -f $ftest ];
        then
          echo "Testing $ftest" >> $outfile
          python correctness.py -r $fref -t $ftest  >> $outfile
        else
          echo "MISSING: $ftest"  >> $outfile
        fi
      done
    done
  done
done
echo "MPI Multi-Node Correctness test completed. Results in $outfile" >> $outfile
