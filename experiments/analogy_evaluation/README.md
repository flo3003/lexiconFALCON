##Modified word analogy task

The original word analogy task is formulated in a way that there can be only one correct answer for each question. For example, the expected answer for the question *“dad is to mom as grandfather is to ?”* in the dataset is *“grandmother”* therefore we aim for the following quantity cos(wmom −wdad +wgrandfather,wgrandmother) to be maximum. If cos(wmom −wdad + wgrandfather,wgrandma)>cos(wmom −wdad +wgrandfather,wgrandmother) then the answer *“grandma”* to the question would be considered incorrect. However, we can exploit the information provided by the semantic lexicons so that more than one answer can be considered as correct. Therefore, if the word *“grandmother”* is semantically related to *“grandma”* in the lexicon, we consider the answer *“grandma”* to be also correct. We refer to this version of the task as the **modified analogy task**.


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
