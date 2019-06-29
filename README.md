# Multi-GPU MCMC Implementation
Implementation of Metropolis-MCMC using multi-GPUs approach on OpenACC as part of the MSc HPC with Data Science Dissertation in EPCC.

# Pre-requisites

- Argtable C command line parser
- GSL - GNU Scientific Library

See [here](./docs/installations/install.md) for installation directions.

# Progress

- [ ] **`Data Generators`**

  * [X] Synthetic Generator

    * [X] Set betas through command line or generate them in random

    * [X] Generate data points

    * [X] Normalise data points

    * [X] Produce Binary Labels

  * [X] Infimnist Generator

    * [X] Compile generator & generate datasets

    * [X] Preprocess data sets(normalise, choose classes, generate binary labels)

    * [X] Generate new data and label files

  * [ ] Flights Dataset

    * [ ] Download and clean

    * [ ] Generate new features

    * [ ] Generate new data and label files

- [ ] **`Serial Code`**
  * [X] Parsing command line arguments

  * [X] Allocation of data and sample arrays

  * [X] Read data from files

  * [X] Setup RNG

  * [X] Initialise samples

  * [X] Random walk & proposal step

  * [X] Prior evaluation - Non informative prior

  * [X] Likelihood Implementation

  * [X] Evaluation of posteriors

  * [X] Accept Reject Sample

  * [X] Time sampler

  * [X] Write generated chain to file

  * [X] Evaluate autocorrelation and ESS

  * [X] Evaluate classification accuracy in test data (MC integration)

  * [ ] Write autocorrelation, burn-in and post burn-in chain in files

  * [ ] Switch to input file rather than command line (?)


- [ ] **`Single-GPU code using OpenACC`**

- [ ] **`Multi-GPU code using OpenACC`**
