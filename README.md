# lexiconFALCON

This repository contains the codes for the *Lexicon-FALCON (LF)* algorithm proposed in 

Ampazis, N., and Sakketou, F. (2019) *A Constrained Optimization Algorithm for Learning GloVe Embeddings with Semantic Lexicons, Knowledge-based Systems (Under review)*

## Requirements

## Installation

## Usage
Create a directory `embeddings` by running:
```
mkdir embeddings
```


### Instructions to create the input_files and database_files:

Clone the following [Github repo](https://github.com/flo3003/glove-python) in your home directory and run

```
pip install glove_python
```

Go to the `glove-python` directory and run

```
python setup.py cythonize
pip install -e .
```

Once these steps are completed, you are ready to go.

