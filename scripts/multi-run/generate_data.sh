#!/bin/sh

DATA_DIR="../../data/synthetic"
cd ../../generators/synthetic/

# Generate 500 synthetic points for dim=3
N="500"
dim="3"
BETA="-10 5 10"

outdir="$DATA_DIR/500_3"
mkdir -p $outdir

if [ -d $outdir ];
then
  python generator.py -dim $dim -Ntrain $N -Ntest 10 -beta $BETA
  echo "Moving data in $outdir"
  mv *.csv $outdir
else
  echo "WARNING: $outdir doesn't exist. Skipping generation."
fi
