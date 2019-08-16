# lexiconFALCON

This repository contains the codes for the *Lexicon-FALCON (LF)* algorithm proposed in 

Ampazis, N., and Sakketou, F. (2019) *A Constrained Optimization Algorithm for Learning GloVe Embeddings with Semantic Lexicons, Knowledge-based Systems (Under review)*

## Requirements

## Installation

Clone this repository in your home directory.

### Instructions to create the input_files and database files:

Clone the following [Github repo](https://github.com/flo3003/glove-python) in your home directory and run
```
pip install glove_python
```

Go to the `glove-python` directory
```
cd ~/glove-python
```

and run
```
python setup.py cythonize
pip install -e .
```

Then run
```
ipython -i -- examples/get_database_files.py -c directory_to_corpus.txt -o ~/LexiconFALCON/lexicons/lexicon.txt -d 100
```

The following files will be constructed:
- `coo_matrix.csv` contains the co-occurrence matrix of `directory_to_corpus.txt` 
- `word_mapping.csv` contains the mapping of each **word** to an **Id**
- `lexicon.csv` contains the Ids of the words that are semantically related
- `corpus.model` and `glove.model` are the saved corpus and glove models
- `random_initial_vectors.txt` contains the embeddings' initialization 

The directory `~/LexiconFALCON/lexicons/` contains the lexicons that were used in the paper. 

If you need to construct only the `lexicon.csv` and `random_initial_vectors.txt` from the saved corpus and glove models run
```
ipython -i -- examples/get_database_files.py -l 1 -o ~/LexiconFALCON/lexicons/lexicon.txt -d 100
```

The argument `-d` refers to the embedding dimensions. The default is 100.

#### Start the training from random embeddings:

Copy the `random_initial_vectors.txt` in the `lexiconFALCON/input_files` directory by running:
```
cp ~/glove-python/random_initial_vectors.txt ~/lexiconFALCON/input_files
```

Comment the lines 

#### Start the training from pretrained embeddings:

Download the pretrained embeddings in the `lexiconFALCON` directory. 


Create a directory `embeddings` by running:
```
mkdir embeddings
```


