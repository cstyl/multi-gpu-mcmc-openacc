#!/bin/sh

SCRIPT_DIR=$(pwd)
ACCOUNT="d167-s1887443"

TIME="walltime=00:20:00"
PLACE="place=scatter:excl"
SELECT="select=1:ncpus=36"
RESOURCES="$SELECT,$TIME,$PLACE"

QSUB="qsub -A $ACCOUNT -l ${RESOURCES}"

EXEC_DIR="../../experiments/serial"
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

DATAPOINTS="4000 40000 400000 4000000"
DIMS="3 10 50"
burn="1000"
postburn="1000"
max_lag="499"
infr="0"

for N in $DATAPOINTS
do
  for dim in $DIMS
  do
    cd $EXEC_DIR
    indir="$DATA_DIR/4M_$dim"
    outdir="./serial-${N}_$dim"
    # generate input file for the test
    infile="serial-${N}_$dim.dat"
    outfile="serial-${N}_$dim.txt"
    echo "train_x       $indir/X_train.csv" > $infile
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
    echo "tune_sd       0" >> $infile
    echo "lhood         logistic_regression" >> $infile
    echo "max_lag       $max_lag" >> $infile
    echo "lag_threshold 0.1" >> $infile
    echo "ess           all" >> $infile
    echo "inference     $infr" >> $infile
    echo "mc_integ      logistic_regression" >> $infile
    echo "freq_burn     1000" >> $infile
    echo "freq_postburn 1000" >> $infile
    echo "freq_autocorr 2000" >> $infile
    echo "freq_ess      2000" >> $infile
    echo "freq_mc_integ 100" >> $infile
    echo "outdir        $outdir" >> $infile
    echo "random_seed   7361237" >> $infile

    if [ -d $indir ];
    then
      cd $SCRIPT_DIR
      echo "Submitting ./mcmc.exe for $N points of dim $dim"
      $QSUB -N "mcmc_$N_$dim" \
            -v INFILE="$infile",OUTFILE="$outfile",OUTDIR="$outdir",EXECDIR="$EXEC_DIR" \
            submit.pbs
    else
      echo "WARNING: $indir doesn't exist. Skipping test."
    fi
  done
done
