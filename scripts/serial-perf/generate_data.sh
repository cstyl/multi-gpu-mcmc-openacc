#!/bin/sh

DATA_DIR="../../data/synthetic"
cd ../../generators/synthetic/

# Generate 1m synthetic points for each dimension
N=1000000
DIMS="3 10 50"

for dim in $DIMS
do
  outdir="$DATA_DIR/1M_$dim"
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
