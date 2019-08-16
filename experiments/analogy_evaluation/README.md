## Modified word analogy task

As discussed in [1]
>The word analogy task [(Levy & Goldberg, 2014)](https://www.aclweb.org/anthology/W14-1618) evaluates word embeddings based on their ability to find analogies like ``<img src="https://latex.codecogs.com/svg.latex?\Large&space;\boldmath{w_{a}}" title="\Large \boldmath{w_{a}}" />  is to <img src="https://latex.codecogs.com/svg.latex?\Large&space;\boldmath{w_{b}}" title="\Large \boldmath{w_{b}}" /> as to <img src="https://latex.codecogs.com/svg.latex?\Large&space;\boldmath{w_{c}}" title="\Large \boldmath{w_{c}}" /> is to ?''. The scope of the task is to identify the correct word vector <img src="https://latex.codecogs.com/svg.latex?\Large&space;\boldmath{w_{d}}" title="\Large \boldmath{w_{d}}" /> from the vocabulary <img src="https://latex.codecogs.com/svg.latex?\Large&space;\mathcal{V}" title="\Large \mathcal{V}" /> that has the maximum cosine similarity to  <img src="https://latex.codecogs.com/svg.latex?\Large&space;\boldmath{w_{b}}" title="\Large \boldmath{w_{b}}" /> - <img src="https://latex.codecogs.com/svg.latex?\Large&space;\boldmath{w_{a}}" title="\Large \boldmath{w_{a}}" /> + <img src="https://latex.codecogs.com/svg.latex?\Large&space;\boldmath{w_{c}}" title="\Large \boldmath{w_{c}}" />
The original word analogy task is formulated in a way that there can be only one correct answer for each question. For example, the expected answer for the question *“dad is to mom as grandfather is to ?”* in the dataset is *“grandmother”* therefore we aim for the following quantity <img src="https://latex.codecogs.com/svg.latex?\Large&space;cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandmother}})" title="\Large cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandmother}})" /> to be maximum. If <img src="https://latex.codecogs.com/svg.latex?\Large&space;cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandma}})" title="\Large cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandma}})" /> > <img src="https://latex.codecogs.com/svg.latex?\Large&space;cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandmother}})" title="\Large cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandmother}})" /> then the answer *“grandma”* to the question would be considered incorrect. However, we can exploit the information provided by the semantic lexicons so that more than one answer can be considered as correct. Therefore, if the word *“grandmother”* is semantically related to *“grandma”* in the lexicon, we consider the answer *“grandma”* to be also correct. 


### Instructions:

Clone the following [Github repo](https://github.com/flo3003/glove-python) in your home directory and run

```
pip install glove_python
```

Go to the `glove-python` directory and run

```
python setup.py cythonize
pip install -e .
```

Then run

```
ipython -i -- examples/get_database_files.py -c corpus.txt -o ~/LexiconFALCON/lexicons/lexicon.txt -d 100
```

The following will be constructed:
- `coo_matrix.csv` is the co-occurrence matrix of `corpus.txt` 
- `word_mapping.csv` is the mapping of each **word** to an **Id**
- `lexicon.csv` contains the Ids of the words that are semantically related

The directory `~/LexiconFALCON/lexicons/` contains the lexicons that were used in the paper. 


