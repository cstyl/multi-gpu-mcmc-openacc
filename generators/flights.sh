#!/bin/sh

# Default arguments
IN="./Airline/1995.csv"
OUT="../../data/flights/minmax"
RATIO="0.8"
SCALE="MinMax"
PREC="double"

cd ./flights

if [ ! -d "./Airline" ]
then
  echo "Dataset missing. Proceed to download..."
  # Download dataset and decompress
  wget http://www.jstatsoft.org/index.php/jss/article/downloadSuppFile/v055i14/Airline.tar.bz2
  tar xvf Airline.tar.bz2
  rm -rf Airline.tar.bz2
  echo "Dataset downloaded and decompressed."
fi

POSITIONAL=()
while [[ $# -gt 0 ]]
do
	key="$1"
  case $key in
    -i)
    IN="$2"
    shift # past argument
    shift # past value
    ;;
    -o)
    OUT="$2"
    OUT="../$OUT"
    shift # past argument
    shift # past value
    ;;
    -r)
    RATIO="$2"
    shift # past argument
    shift # past value
    ;;
    -s)
    SCALE="$2"
    shift # past argument
    shift # past value
    ;;
    -p)
    PREC="$2"
    shift # past argument
    shift # past value
    ;;
    *)    # unknown option
    echo "$1 is an invalid option and discarded"
    shift # past argument
    ;;
  esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters
echo "python process.py -i $IN -o $OUT -r $RATIO -s $SCALE -p $PREC"
python process.py -i $IN -o $OUT -r $RATIO -s $SCALE -p $PREC
