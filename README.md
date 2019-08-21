# lexiconFALCON

This repository contains the codes for the *Lexicon-FALCON (LF)* algorithm proposed in 

Ampazis, N., and Sakketou, F. (2019) *A Constrained Optimization Algorithm for Learning GloVe Embeddings with Semantic Lexicons, Knowledge-based Systems (Under review)*

## Installation

Obviously first clone this repository.

```
git clone https://github.com/flo3003/lexiconFALCON.git
```

Change into the cloned lexiconFALCON directory:

```
cd lexiconFALCON
```

Now you will need to clone the following [Github repo](https://github.com/flo3003/glove-python) in lexiconFALCON's directory

```
git clone https://github.com/flo3003/glove-python.git
```

Then run the following commands in order:

```
cd glove-python
python setup.py cythonize
pip install -e .
```

### Instructions to create the necessary files:

In the glove-python directory run

```
python examples/get_database_files.py -c /path/to/some/corpustextfile -o /path/to/somelexiconfile -d 100
```
The argument `-d` refers to the embedding dimensions. The default is 100. 

`corpustextfile` can be any plain text file (with words being separated by space) with punctuation or not. `somelexiconfile` should have the format of the files in the lexicon directory of this repo.

The following files will be constructed:
- `coo_matrix.csv` which contains the co-occurrence matrix of `corpustextfile` 
- `word_mapping.csv` which contains the mapping of each **word** to an **Id**
- `lexicon.csv` which contains the Ids of the words that are semantically related
- `corpus.model` and `glove.model` are the saved corpus and glove models
- `random_initial_vectors.txt` contains the embeddings' initialization 

If you need to construct **only** the `lexicon.csv` file from previously saved corpus and glove models then run

```
python examples/get_database_files.py -l 1 -o /path/to/somelexiconfile -d 100
```

*Note that every time you run this command the `random_initial_vectors.txt` file will change.*

### Instructions to create the MySQL database schema and load the csv files:

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

## Training

To start training:

Copy the `random_initial_vectors.txt` in the `lexiconFALCON/input_files` directory by issuing:

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

Edit the `run_algos.sh` script to fill in the parameter values and run

```
./run_algos.sh
```

At the end of the training the embeddings will be saved as:

```
embeddings/LF_embeddings_dP_*dP_value*_xi_*xi_value*_num_epochs_*number_of_epochs*_final_error_*error*_*lexicon*.txt
```

And the error history will be saved in `log_file.txt`

If you want to start the training from pretrained embeddings:

Download pretrained embeddings (e.g. [glove.6B](http://nlp.stanford.edu/data/glove.6B.zip)) in the `lexiconFALCON` directory and run

```
./map_ids.sh  input_files/random_initial_vectors.txt pretrainedembeddingsfile
```

This will create the following files:
- `input_files/pretrainedembeddingsfile_weights.txt` which contains only the words' vectors (witout the actual word in the first column)
- `input_files/mapped_ids.txt` which maps the indices of the vocabulary words to the indices of the corresponding words in the pretrained vector file. For example word "the" has index "3" in the vocabulary and index "56" in the pretrained vector file. The mapping file maps 3 to 56.

Edit the `run_algos_pretrained.sh` script to fill in the parameter values, and run

```
./run_algos_pretrained.sh
```

*Note that the parameter `emb_file` in `run_algos_pretrained.sh` should be the `input_files/pretrainedembeddingsfile_weights.txt` file.*

At the end of the training the embeddings will be saved as:

```
embeddings/LF_pretrained_embeddings_dP_*dP_value*_xi_*xi_value*_num_epochs_*number_of_epochs*_final_error_*error*_*lexicon*.txt
```

And the error history will be saved in `log_file.txt`

# GloVe

This an implementation of the [GloVe](https://nlp.stanford.edu/projects/glove/) algorithm in C.

## Installation

Follow the same instructions with the lexiconFALCON algorithm and compile `glove.c` with 

```
gcc -O3 glove.c -o glove -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -lm
```

## Training

Uncomment lines `21-22` from the `run_algos.sh` script.

At the end of the training the embeddings will be saved as:

```
embeddings/GloVe_embeddings_lr_*learning_rate*_num_epochs_*number_of_epochs*_final_error_*error*.txt
```

And the error history will be saved in `log_glove.txt`
