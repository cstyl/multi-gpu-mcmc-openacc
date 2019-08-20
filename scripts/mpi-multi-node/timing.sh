#!/bin/sh

EXEC_DIR="../../experiments/mpi-multi-node"
if [ ! -d $EXEC_DIR ];
then
  echo "$EXEC_DIR doesn't exist"
else
  echo "Experiment Directory: $EXEC_DIR"
  cd $EXEC_DIR
fi

REPS=3
outfile="timing.csv"
NODES=("1" "2" "3" "4" "6" "8")
DATAPOINTS="10000 100000 1000000 10000000"
DIMS="10"

printf "Nodes,Procs,Threads,Datapoints,Dim,Reps," > $outfile
printf "Total,MCMC,lhood,mvmul,reduction,upd_data,upd_values,acc_init\n" >> $outfile

for N in $DATAPOINTS
do
  for dim in $DIMS
  do
    for i in `seq -w 0 $((${#NODES[@]}-1))`
    do
      for k in `seq -w 1 $REPS`
      do
        report="./mpi-multi-node-${N}_${dim}_${NODES[$i]}/$k/out.txt"
        # parse input file
        total=$(awk '/Total:/ {printf "%s",$4}' $report)
        mcmc=$(awk '/MCMC Metropolis:/ {printf "%s",$5}' $report)
        lhood=$(awk '/Likelihood:/ {printf "%s",$4}' $report)
        mvmul=$(awk '/MatVec Mult Kernel:/ {printf "%s",$6}' $report)
        red=$(awk '/Reduction Kernel:/ {printf "%s",$5}' $report)
        upd_data=$(awk '/Dev Update Data:/ {printf "%s",$6}' $report)
        if [ -z $upd_data ]; then upd_data="0.0"; fi

        upd_values=$(awk '/Dev Update Values:/ {print $6}' $report)
        if [ -z $upd_values ]; then upd_values="0.0"; fi

        open_acc_init=$(awk '/OpenACC Initialisation:/ {printf "%s",$5}' $report)
        if [ -z $open_acc_init ]; then open_acc_init="0.0"; fi

        printf "${NODES[$i]},36,1,$N,$dim,$k,$total,$mcmc,$lhood,$mvmul,$red," >> $outfile
        printf "$upd_data,$upd_values,$open_acc_init\n" >> $outfile
      done
    done
  done
done
