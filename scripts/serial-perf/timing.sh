#!/bin/sh

EXEC_DIR="../../experiments/serial-perf"
if [ ! -d $EXEC_DIR ];
then
  echo "$EXEC_DIR doesn't exist"
else
  echo "Experiment Directory: $EXEC_DIR"
  cd $EXEC_DIR
fi

outfile="timing.csv"
DATAPOINTS="1000 10000 100000 1000000"
DIMS="3 10 50"

printf "Nodes,Procs,Threads,Datapoints,Dim,Reps," > $outfile
printf "Total,MCMC,lhood,mvmul,reduction,upd_data,upd_values,acc_init\n" >> $outfile

for N in $DATAPOINTS
do
  for dim in $DIMS
  do
    for k in `seq -w 1 $REPS`
    do
      report="./serial-${N}_${dim}/$k/out.txt"
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

      printf "1,1,1,$N,$dim,$k,$total,$mcmc,$lhood,$mvmul,$red," >> $outfile
      printf "$upd_data,$upd_values,$open_acc_init\n" >> $outfile
    done
  done
done
