#!/bin/sh

PY_DIR="../../tests/correctness"

EXEC_DIR="../../experiments/omp"
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
THREADS="1 2 4 8 10 15 17 21 24 28 32 36"
DATAPOINTS="10000 100000 1000000 10000000"
DIMS="10"
outfile="${EXEC_DIR}/correctness-report.txt"

cd $PY_DIR

echo "Starting OMP Correctness test..." 2>&1 | tee $outfile
for N in $DATAPOINTS
do
  for dim in $DIMS
  do
    fref="${REF_DIR}/serial-${N}_$dim/1/progress.csv"
    for t in $THREADS
    do
      for k in `seq -w 1 $REPS`
      do
        ftest="${EXEC_DIR}/omp-${N}_${dim}_${t}/$k/progress.csv"
        if [ -f $ftest ];
        then
          echo "Testing $ftest" 2>&1 | tee -a $outfile
          python correctness.py -r $fref -t $ftest 2>&1 | tee -a $outfile
        else
          echo "MISSING: $ftest" 2>&1 | tee -a $outfile
        fi
      done
    done
  done
done
echo "OMP Correctness test completed. Results in $outfile" 2>&1 | tee -a $outfile
