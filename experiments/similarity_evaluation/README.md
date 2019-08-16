### Instructions:

Create a directory `results` by running

```
mkdir results 
```

#### Requirements:

You need to install the `glove-python` module by running:

```
pip install glove-python
```

and you are ready to go.

Now to compute the Spearman similarity simply run: 

```
./run_similarity.sh
```

This shell script selects all the embedding files created in directory `lexiconFALCON/embeddings` and runs the similarity evaluation task.

The results for each database are stored in the `results` directory in corresponding `.csv` files.

Each database is stored in the directory `data` and contains word pairs and their corresping score in the following format:

```
sun;sunlight;50.000000
automobile;car;50.000000
river;water;49.000000
stairs;staircase;49.000000
```

