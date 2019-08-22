# lexiconFALCON

This repository contains the codes for the *Lexicon-FALCON (LF)* algorithm proposed in 

Sakketou, F., and Ampazis, N.  (2019) *A Constrained Optimization Algorithm for Learning GloVe Embeddings with Semantic Lexicons, Knowledge-based Systems (Under review)*

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
python examples/get_database_files.py -c /path/to/some/corpustextfile -o ../lexicons/somelexiconfile -d 100
```
The argument `-d` refers to the embedding dimensions. The default is 100. 

`corpustextfile` can be any plain text file (with words being separated by space) with punctuation or not. `somelexiconfile` should have the format of the files in the lexicon directory of this repo. For your convenience you may actually use anyone of those in the above command since tables with the same name will be created by default in the database schema.

The following files will be created:
- `coo_matrix.csv` which contains the co-occurrence matrix of `corpustextfile` 
- `word_mapping.csv` which contains the mapping of each **word** to an **Id**
- `somelexiconfile.csv` which contains the Ids of the words that are semantically related
- `corpus.model` and `glove.model` are the saved corpus and glove models
- `random_initial_vectors.txt` contains the embeddings' initialization 

If you need to construct **only** the `somelexiconfile.csv` file from previously saved corpus and glove models then run

```
python examples/get_database_files.py -l 1 -o ../lexicons/somelexiconfile -d 100
```

*Note that every time you run this command the `random_initial_vectors.txt` file will change.*

### Instructions to create the MySQL database schema and load the csv files:

Change to the parent `lexiconFALCON` directory.

```
cd ..
```

Create the database schema by running

```
mysql < LexiconFALCON_db.sql
```

**Edit** the file `load_data.sql` and uncomment the line corresponding to your chosen lexicon.

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

Compile `lexiconfalcon.c` with

```
gcc -O3 lexiconfalcon.c -o lexiconfalcon -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -lm
```

**Edit** the `run_algos.sh` script to fill in the parameter values and run

```
chmod +x run_algos.sh
./run_algos.sh
```

At the end of the training the embeddings will be saved in GloVe format as:

```
embeddings/LF_embeddings_dP_*dP_value*_xi_*xi_value*_num_epochs_*number_of_epochs*_final_error_*error*_*lexicon*.txt
```

And the error history will be saved in `log_file.txt`

### Starting training from pretrained embeddings

If you want to start the training from pretrained embeddings:

Download pretrained embeddings in GloVe format (e.g. [glove.6B](http://nlp.stanford.edu/data/glove.6B.zip)) in the `lexiconFALCON` directory and run

```
chmod +x map_ids.sh
./map_ids.sh pretrainedembeddingsfile
```

This will create the following files:
- `input_files/pretrainedembeddingsfile_weights.txt` which contains only the words' vectors (witout the actual word in the first column)
- `input_files/mapped_ids.txt` which maps the indices of the vocabulary words to the indices of the corresponding words in the pretrained vector file. For example word "the" has index "3" in the vocabulary and index "56" in the pretrained vector file. The mapping file maps 3 to 56.

**Edit** the `run_algos_pretrained.sh` script to fill in the parameter values, and run

```
chmod +x run_algos_pretrained.sh
./run_algos_pretrained.sh
```

*Note that the parameter `emb_file` in `run_algos_pretrained.sh` should be the `input_files/pretrainedembeddingsfile_weights.txt` file.*

At the end of the training the embeddings will be saved in GloVe format as:

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

**Uncomment** lines `21-22` from the `run_algos.sh` script.

At the end of the training the embeddings will be saved as:

```
embeddings/GloVe_embeddings_lr_*learning_rate*_num_epochs_*number_of_epochs*_final_error_*error*.txt
```

And the error history will be saved in `log_glove.txt`

## How to run the experiments

### Analogy Evaluation 

Now that you have trained your model you can run the analogy evaluation tasks by changing into the `analogy_evaluation` directory:

```
cd experiments/analogy_evaluation
```

#### Original Analogy Evaluation Task

Run the original analogy evalution task with:

```
chmod +x analogy.sh
./analogy.sh
```

This script will run experiments for all the embedding files in the `lexiconFALCON/embeddings/` directory. The results will be saved in `analogy` file.

#### Modified Analogy Evaluation Task

As discussed in the paper

>The word analogy task [(Levy & Goldberg, 2014)](https://www.aclweb.org/anthology/W14-1618) evaluates word embeddings based on their ability to find analogies like ``<img src="https://latex.codecogs.com/svg.latex?\Large&space;\boldmath{w_{a}}" title="\Large \boldmath{w_{a}}" />  is to <img src="https://latex.codecogs.com/svg.latex?\Large&space;\boldmath{w_{b}}" title="\Large \boldmath{w_{b}}" /> as to <img src="https://latex.codecogs.com/svg.latex?\Large&space;\boldmath{w_{c}}" title="\Large \boldmath{w_{c}}" /> is to ?''. The scope of the task is to identify the correct word vector <img src="https://latex.codecogs.com/svg.latex?\Large&space;\boldmath{w_{d}}" title="\Large \boldmath{w_{d}}" /> from the vocabulary <img src="https://latex.codecogs.com/svg.latex?\Large&space;\mathcal{V}" title="\Large \mathcal{V}" /> that has the maximum cosine similarity to  <img src="https://latex.codecogs.com/svg.latex?\Large&space;\boldmath{w_{b}}" title="\Large \boldmath{w_{b}}" /> - <img src="https://latex.codecogs.com/svg.latex?\Large&space;\boldmath{w_{a}}" title="\Large \boldmath{w_{a}}" /> + <img src="https://latex.codecogs.com/svg.latex?\Large&space;\boldmath{w_{c}}" title="\Large \boldmath{w_{c}}" />
The original word analogy task is formulated in a way that there can be only one correct answer for each question. For example, the expected answer for the question *“dad is to mom as grandfather is to ?”* in the dataset is *“grandmother”* therefore we aim for the following quantity <img src="https://latex.codecogs.com/svg.latex?\Large&space;cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandmother}})" title="\Large cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandmother}})" /> to be maximum. If <img src="https://latex.codecogs.com/svg.latex?\Large&space;cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandma}})" title="\Large cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandma}})" /> > <img src="https://latex.codecogs.com/svg.latex?\Large&space;cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandmother}})" title="\Large cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandmother}})" /> then the answer *“grandma”* to the question would be considered incorrect. However, we can exploit the information provided by the semantic lexicons so that more than one answer can be considered as correct. Therefore, if the word *“grandmother”* is semantically related to *“grandma”* in the lexicon, we consider the answer *“grandma”* to be also correct. 

Run the modified analogy evalution task with:

```
chmod +x modified_analogy.sh
./modified_analogy.sh
```

This script will run experiments for all the embedding files in the `lexiconFALCON/embeddings/` directory. The results will be saved in `modified_analogy` file.


### Sentiment Analysis 

Now that you have trained your model you can run the sentiment analysis by changing into the `sentiment_analysis` directory:

```
cd experiments/sentiment_analysis
```

**Edit** lines `94-95` of `sentiment_analysis.py` and select the correct directory for your train and test files.

*Note that in our experiments we used the IMDB sentiment analysis dataset found [here](https://www.kaggle.com/c/word2vec-nlp-tutorial/data)*

Now you can run the sentiment analysis task with:

```
chmod +x run_sentiment.sh
./run_sentiment.sh
```

This script will run experiments for all the embedding files in the `lexiconFALCON/embeddings/` directory. 

The results for each review are stored in the `sentiment_analysis/results` directory and the overall accuracy and F1-score for each embedding file are stored in `results_sentiment.txt`.

Once your experiments are completed, you can run

```
./convert_txt2csv.sh results_sentiment.txt
```

so that the results are presented in the following way:

| *Filename* | *Accuracy*  | *F1-score* |
| ---------- | ----------- | ---------- |
| LF+PPDB    | 0.83908     | 0.837997   |
| GloVe      | 0.83156     | 0.830208   |


### Similarity Evaluation
