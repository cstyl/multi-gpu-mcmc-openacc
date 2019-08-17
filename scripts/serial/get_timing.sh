#!/bin/sh

EXEC_DIR="../../experiments/serial"
if [ ! -d $EXEC_DIR ];
then
  echo "$EXEC_DIR doesn't exist"
else
  echo "Experiment Directory: $EXEC_DIR"
  cd $EXEC_DIR
fi

outfile="timing.csv"
DATAPOINTS="4000 40000 400000 4000000"
DIMS="3 10 50"

printf "Datapoints,Dim,Total,MCMC,lhood,mvmul,reduction,upd_data,upd_values,acc_init\n" > $outfile

for N in $DATAPOINTS
do
  for dim in $DIMS
  do
    report="./serial-${N}_$dim/out.txt"
    printf "$N,$dim," >> $outfile
    # parse input file
    total=$(awk '/Total:/ {printf "%s",$4}' $report)
    mcmc=$(awk '/MCMC Metropolis:/ {printf "%s",$5}' $report)
    lhood=$(awk '/Likelihood:/ {printf "%s",$4}' $report)
    mvmul=$(awk '/MatVec Mult Kernel:/ {printf "%s",$6}' $report)
    red=$(awk '/Reduction Kernel:/ {printf "%s",$5}' $report)
    upd_data=$(awk '/Dev Update Data:/ {printf "%s",$6}' $report)
    if [ $upd_data -eq ""]; then upd_data="0.0"; fi

    upd_values=$(awk '/Dev Update Values:/ {print $6}' $report)
    if [ $upd_values -eq ""]; then upd_values="0.0"; fi

    open_acc_init=$(awk '/OpenACC Initialisation:/ {printf "%s",$5}' $report)
    if [ $open_acc_init -eq ""]; then open_acc_init="0.0"; fi
    printf "$total,$mcmc,$lhood,$mvmul,$red,$upd_data,$upd_values,$open_acc_init\n" >> $outfile
  done
done
