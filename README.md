# Multi-GPU MCMC Implementation
Implementation of Metropolis-MCMC using multi-GPUs approach on OpenACC as part of the MSc HPC with Data Science Dissertation in EPCC.

# Pre-requisites

- Argtable C command line parser
- GSL - GNU Scientific Library

See [here](./docs/installations/install.md) for installation directions.

# Progress

### Data Generators

[ ]

### Serial Code
[X] Parsing command line arguments

[X] Allocation of data and sample arrays

[X] Read data from files

[ ] Setup RNG

[ ] Initialise samples

[ ] Random walk & proposal step

[ ] Prior evaluation

[ ] Likelihood Implementation

[ ] Evaluation of posteriors

[ ] Accept Reject Sample

[ ] Time sampler

[ ] Write generated chain to file

[ ] Evaluate autocorrelation and ESS

[ ] Evaluate classification accuracy in test data


### Single-GPU code using OpenACC

### Multi-GPU code using OpenACC
