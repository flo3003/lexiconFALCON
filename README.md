# lexiconFALCON

This repository contains the codes for the *Lexicon-FALCON (LF)* algorithm proposed in 

Ampazis, N., and Sakketou, F. (2019) *A Constrained Optimization Algorithm for Learning GloVe Embeddings with Semantic Lexicons, Knowledge-based Systems (Under review)*

## Requirements

## Installation

Obviously first clone this repository.

Change into the cloned lexiconFALCON directory:

---
cd lexiconFALCON
---

Now you will need to clone the following [Github repo](https://github.com/flo3003/glove-python) in lexiconFALCON's directory, and then to run the following commands in order:

```
cd glove-python
python setup.py cythonize
pip install -e .
```

### Instructions to create the necessary files:

In the glove-python directory run

```
ipython -i -- examples/get_database_files.py -c /path/to/some/corpustextfile -o /path/to/somelexiconfile -d 100
```
The argument `-d` refers to the embedding dimensions. The default is 100.

The following files will be constructed:
- `coo_matrix.csv` which contains the co-occurrence matrix of `corpustextfile` 
- `word_mapping.csv` contains the mapping of each **word** to an **Id**
- `lexicon.csv` contains the Ids of the words that are semantically related
- `corpus.model` and `glove.model` are the saved corpus and glove models
- `random_initial_vectors.txt` contains the embeddings' initialization 

If you need to construct **only** the `lexicon.csv` file from previously saved corpus and glove models then run

```
ipython -i -- examples/get_database_files.py -l 1 -o /path/to/somelexiconfile -d 100
```

**Note that every time you run this command the `random_initial_vectors.txt` file will change.**

### Instructions to create a MySQL database and load the csv files:

Change to the parent `lexiconFALCON` directory.
```
cd ../lexiconFALCON
```

Create the database schema by running

```
mysql < LexiconFALCON_db.sql
```

Load the csv files into the corresponding database tables by running the following command:

```
mysql < load_data.sql
```


#### To start training:

Copy the `random_initial_vectors.txt` in the `lexiconFALCON/input_files` directory by running:
```
cp glove-python/random_initial_vectors.txt input_files
```

Create a directory `embeddings`
```
mkdir embeddings
```

Compile `lexiconfalcon.c` with

```
gcc -O3 lexiconfalcon.c -o lexiconfalcon -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -lm
```

Edit the `run_algos.sh` script to select the correct parameters and run

```
./run_algos.sh
```

If you want to start the training from pretrained embeddings:

Download the pretrained embeddings in the `lexiconFALCON` directory. 

Run
```
./map_ids.sh  input_files/random_initial_vectors.txt pretrained_embeddings_file.txt
```

This will create the following files:
- `pretrained_embeddings_file_weights.txt` which contains only the words' vectors
- `mapped_ids.txt` which is the mapping file maps the indices of the vocabulary words to the indices of the corresponding words in the pretrained vector file. For example word "the" has index "3" in the vocabulary and index "56" in the pretrained vector file. The mapping file maps 3 to 56.

Edit the `run_algos_pretrained.sh` script to select the desirable parameters. Note that the parameter `emb_file` in `run_algos_pretrained.sh` should be the `pretrained_embeddings_file_weights.txt` file.

Run
```
./run_algos_pretrained.sh
```

# GloVe

This an implementation of the [GloVe](https://nlp.stanford.edu/projects/glove/) algorithm in C.

### Instructions

Follow the same instructions with the lexiconFALCON algorithm.

Compile with 
```
gcc -O3 glove.c -o glove -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -lm
```


### Instructions to reproduce the results of the paper:
Due to GitHub's space restrictions, you need to download some additional files. Go to [lexiconFALCON/database/](https://github.com/flo3003/lexiconFALCON/tree/master/database) and [lexiconFALCON/input_files/](https://github.com/flo3003/lexiconFALCON/tree/master/input_files) and download the necessary files from the links provided.

Edit the `run_algos.sh` script to select the correct parameters and run
```
./run_algos.sh
```

The directory `~/LexiconFALCON/lexicons/` contains the lexicons that were used in the paper. 

