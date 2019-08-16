## Modified word analogy task

As discussed in [1]
>The word analogy task \citep{LevyG14} evaluates word embeddings based on their ability to find analogies like ``$w_a$ is to $w_b$ as to $w_c$ is to ?''. The scope of the task is to identify the correct word vector $w_d$ from the vocabulary $V$ that has the maximum cosine similarity to $w_b - w_a + w_c$ by evaluating:
\beq
%analogy(w_a:w_b \longrightarrow w_c: ?) =
\argmax_{w_d \in V} cos(w_b - w_a + w_c, w_d) \nonumber
\eeq
The original word analogy task is formulated in a way that there can be only one correct answer for each question. For example, the expected answer for the question *“dad is to mom as grandfather is to ?”* in the dataset is *“grandmother”* therefore we aim for the following quantity <img src="https://latex.codecogs.com/svg.latex?\Large&space;cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandmother}})" title="\Large cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandmother}})" /> to be maximum. If <img src="https://latex.codecogs.com/svg.latex?\Large&space;cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandma}})" title="\Large cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandma}})" /> > <img src="https://latex.codecogs.com/svg.latex?\Large&space;cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandmother}})" title="\Large cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandmother}})" />

>cos(wmom −wdad +wgrandfather,wgrandmother) then the answer *“grandma”* to the question would be considered incorrect. However, we can exploit the information provided by the semantic lexicons so that more than one answer can be considered as correct. Therefore, if the word *“grandmother”* is semantically related to *“grandma”* in the lexicon, we consider the answer *“grandma”* to be also correct. We refer to this version of the task as the **modified analogy task**.


<img src="https://latex.codecogs.com/svg.latex?\Large&space;cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandmother}})" title="\Large cos(\boldmath{w_{mom}}-\boldmath{w_{dad}}+\boldmath{w_{grandfather}},\boldmath{w_{grandmother}})" /> 

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

Once these steps are completed, you are ready to go.


Run
```
./modified_analogy.sh
```

This shell script selects all the embedding files created in directory `lexiconFALCON/embeddings` and runs the modified word analogy task.

[1] Ampazis, N., and Sakketou, F. (2019) *A Constrained Optimization Algorithm for Learning GloVe Embeddings with Semantic Lexicons, Knowledge-based Systems (Under review)*
