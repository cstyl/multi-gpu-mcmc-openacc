#!/bin/sh

DATA_DIR="../../data/synthetic"
cd ../../generators/synthetic/

# Generate 100m synthetic points for dim=10
N=100000000
DIMS="10"

for dim in $DIMS
do
  outdir="$DATA_DIR/100M_$dim"
  mkdir -p $outdir

  if [ -d $outdir ];
  then
    python generator.py -dim $dim -Ntrain $N -Ntest 10
    echo "Moving data in $outdir"
    mv *.csv $outdir
  else
    echo "WARNING: $outdir doesn't exist. Skipping generation."
  fi
done
