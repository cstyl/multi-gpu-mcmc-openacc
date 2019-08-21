#!/bin/sh

SCRIPT_DIR=$(pwd)

EXEC_DIR="../../experiments/multi-run"
if [ ! -d $EXEC_DIR ];
then
  echo "$EXEC_DIR doesn't exist"
else
  echo "Experiment Directory: $EXEC_DIR"
fi

DATA_DIR="../../data/synthetic"
if [ ! -d $DATA_DIR ];
then
  echo "$DATA_DIR doesn't exist"
else
  echo "Data Directory: $DATA_DIR"
fi

RUNS=1000
seed=0
seed_incr=5
N="500"
dim="3"
burn="5000"
postburn="25000"
max_lag="12499"
infr="1"
random_init="0"

cd $EXEC_DIR

for run in `seq -w 1 $RUNS`
do
  indir="$DATA_DIR/${N}_${dim}"
  outdir="./run-$run"
  # generate input file for the test
  infile="run-$run.dat"
  outfile="run-$run.txt"

  seed=$(( $seed + $seed_incr ))

  echo "nnodes        1" > $infile
  echo "nprocs        1" >> $infile
  echo "nthreads      1" >> $infile
  echo "train_x       $indir/X_train.csv" >> $infile
  echo "train_y       $indir/Y_train.csv" >> $infile
  echo "test_x        $indir/X_test.csv" >> $infile
  echo "test_y        $indir/Y_test.csv" >> $infile
  echo "train_dimx    $dim" >> $infile
  echo "train_dimy    1" >> $infile
  echo "train_N       $N" >> $infile
  echo "test_dimx     $dim" >> $infile
  echo "test_dimy     1" >> $infile
  echo "test_N        10" >> $infile
  echo "data_format   CSV" >> $infile
  echo "algorithm     metropolis" >> $infile
  echo "sample_dim    $dim" >> $infile
  echo "random_init   0" >> $infile
  echo "burn_N        $burn" >> $infile
  echo "postburn_N    $postburn" >> $infile
  echo "kernel        mvn_block" >> $infile
  echo "tune_sd       11" >> $infile
  echo "lhood         logistic_regression" >> $infile
  echo "max_lag       $max_lag" >> $infile
  echo "lag_threshold 0.1" >> $infile
  echo "ess           all" >> $infile
  echo "inference     $infr" >> $infile
  echo "mc_integ      logistic_regression" >> $infile
  echo "freq_burn     100" >> $infile
  echo "freq_postburn 100" >> $infile
  echo "freq_autocorr 2000" >> $infile
  echo "freq_ess      2000" >> $infile
  echo "freq_mc_integ 100" >> $infile
  echo "outdir        $outdir" >> $infile
  echo "random_seed   $seed" >> $infile

  echo "Run $run"
  ./mcmc.exe $infile > $outfile

  mv $infile $outdir/input.dat
  mv $outfile $outdir/out.txt
done
