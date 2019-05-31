## Generators
In this directory all the data generators to be used in the project are included as follows:
- `synthetic`: This is a synthetic generator that uses a linear model y = sign(b0 + b1*x1 + ... + bn*xn). The true parameters of the model **b** (beta) and data points **x** are determined randomly. As a consequence the labels **y** are generated as either **-1** or **1** effectively representing a binary classification problem. This generator is useful to determine if the algorithm used is able to converge to the correct model parameters.
- `infimnist`: The generator is an extension to the famous MNIST dataset. Using augmentations and transformations the MNIST data set can be extented into infinite number of data points representing digits from 0-9. For the purposes of this project we are only interested in a binary classification task therefore digits 7 and 9 are selected. Therefore labels valued at -1 and 1 represent digits 7 and 9 respectively.
- `flights`: The data consists of flight arrival and departure details for all commercial flights within the USA, from October 1987 to April 2008. Labels set to -1 represent a flight arrived at scheduled time while those with 1 the flights that have been delayed.

#### Synthetic Data:
To generate a synthetic data set, change the directory to be `synthetic/`. Then run:
```sh
$ python generator.py -dim <int> -N <int> -train <float> -precision <type>
```
where:
- `N`: Is the number of generated data points. Default is 1000.
- `dim`: Is the dimensionality of each data point. Default is 10.
- `train`: Is the percentage of train data from the whole data set. The remaining data are assigned to test set. Default is 0.8, i.e 80% of the generated data are the training set.
- `precision`: The type of the output data. Select between single or double precision. Default is double.

Once `generator.py` is executed, the following files are generated in `synthetic/`:
- `beta.csv`: Contains the model parameters.
- `X_train.csv`: Contains the training data points.
- `Y_train.csv`: Contains the training labels.
- `X_test.csv`: Contains the test data points.
- `Y_test.csv`: Contains the test labels.

The model parameters **b** are generated randomly from [-25, 25] while the data points **x** from [-100, 100]. Note that data points are standardized with zero mean and unit variance before exported in the file. Also dimension zero contains the bias.

#### infiMNIST Data:
To download and compile the generator:
```sh
$ cd infimnist
$ wget https://leon.bottou.org/_media/projects/infimnist.tar.gz
$ mkdir generator && tar xf infimnist.tar.gz -C generator --strip-components 1
$ cd generator
$ ls
COPYING  data  infimnist.c  infimnist.h  main.c  Makefile  NMakefile  README
$ make
cc -O3 -Wall -c main.c
cc -O3 -Wall -c infimnist.c
cc -O3 -Wall -o infimnist main.o infimnist.o
$ ls
COPYING  data  infimnist  infimnist.c  infimnist.h  infimnist.o  main.c  main.o  Makefile  NMakefile  README
```

To generate samples in standard MNIST binary format:
```sh
./infimnist [-d <datadir>] <format> <first> <last>
```
Option -d <datadir> can be used to specify the location of the reference data files. The default `data` directory is simply data in the current directory. Arguments <first> and <last> define the first and last index of the range of examples written to the standard output. Argument <format> describes the format of the produced data. Any unambiguous prefix of the following formats are recognized:
- `patterns`: produces an image file using the standard MNIST binary format.
- `labels`: produce a label files using the standard MNIST binary format.

Example:
```sh
$ ./infimnist labels 0 69999 > mnist70k-labels-idx1-ubyte
$ ./infimnist patterns 0 69999 > mnist70k-patterns-idx3-ubyte
$ ls
COPYING  data  infimnist  infimnist.c  infimnist.h  infimnist.o  main.c  main.o  Makefile  mnist70k-labels-idx1-ubyte  mnist70k-patterns-idx3-ubyte  NMakefile  README
```
Note that the first 10k samples represent the actual MNIST test dataset, while from 10k to 70k the training set. Further digits are generated using pseudo-random deformations and translations.

Once the data set is generated in binary form, change directory to `cleaner/` in order to generate the `.csv` version of the data. As the binary form contains all digits from 0 to 9 the `cleaner.py` will filter out only two of the classes of digits. To clean the data set run:
```sh
$ python cleaner.py -data <filename> -labels <filename> -classes <tuple> -dim <int> -N <int> -train <float> -precision <type> -pca <int> -shuffle <int>
```
where:
- `data`: Is the filename that contains the data points for all digits 0 to 9. Default is `../generator/mnist1k-patterns-idx3-ubyte`.
- `labels`: Is the filename that contains the labels of the data set. Default is `../generator/mnist1k-labels-idx1-ubyte`.
- `classes`: Is a tuple of ints that takes as inputs two arguments representing the classes to be selected for the binary classification task. Default is (4, 9).
- `dim`: Dimensionality of the data set. Default is 784 i.e a 28x28 pixels image.
- `N`: Number of generated data points in the data set. Default is 1000.
- `train`: Is the percentage of train data from the whole data set. The remaining data are assigned to test set. Default is 0.8, i.e 80% of the generated data are the training set.
- `precision`: The type of the output data. Select between single or double precision. Default is double.
- `pca`: If set principle component analysis is perform. Can take values from 1 to `dim`. Default is 0 meaning no pca is performed.
- `shuffle`: Indicates if data points are to be shuffled. Default is 0 meaning no shuffling is performed.


Once `cleaner.py` is executed, the following files are generated in `infimnist/`:
- `X_train_4_9.csv`: Contains the training data points for digits 4 and 9.
- `Y_train_4_9.csv`: Contains the training labels for digits 4 and 9.
- `X_test_4_9.csv`: Contains the test data points for digits 4 and 9.
- `Y_test_4_9.csv`: Contains the test labels for digits 4 and 9.

Note that the generated data points are standardized with zero mean and unit variance.

Example:
Generate 1k samples from infimnist and extract the ones labelled 4 or 9. Apply PCA and reduce data set in 10 dimensions:
```sh
$ cd infimnist/
$ cd generator/
$ ./infimnist labels 0 999 > mnist1k-labels-idx1-ubyte
$ ./infimnist patterns 0 999 > mnist1k-patterns-idx3-ubyte
$ cd ../cleaner/
$ python cleaner.py -data ../generator/mnist1k-patterns-idx3-ubyte -labels ../generator/mnist1k-labels-idx1-ubyte -classes 4 9 -dim 784 -N 1000 -train 0.8 -precision double -pca 10 -shuffle 1
```

