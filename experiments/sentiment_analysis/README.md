#### Instructions:

Create directories `results` and `data` by running

```
mkdir results data
```
The `data` directory should contain the files: `labeledTrainData.tsv` and `testData.tsv`. 

The training data should be `\tab` separated in the following format:

| *id*       | *sentiment* | *review*                     |
| ---------- | ----------- | ---------------------------- |
| "5814_8"   | 1           | "With all this stuff ..."    |
| "3630_4"   | 0           | "It must be assumed that..." |

The test data should be `\tab` separated in the following format:

| *id*       | *review*                         |
| ---------- | -------------------------------- |
| "5814_8"   | "Naturally in a film who's ..."  |
| "8348_2"   | "This movie is a disaster        |


Simply run: 

```
./run_sentiment.sh
```

This shell script selects all the embedding files created in directory `lexiconFALCON/embeddings` and runs sentiment analysis.

The results for each review are stored in the `results` directory and the overall accuracy and F1-score for each embedding file are stored in `results_sentiment.txt`.

Once your experiments are completed, you can run

```
./convert_txt2csv.sh results_sentiment.txt
```

so that the results are presented in the following way:

| *Filename* | *Accuracy*  | *F1-score* |
| ---------- | ----------- | ---------- |
| LF+PPDB    | 0.83908     | 0.837997   |
| GloVe      | 0.83156     | 0.830208   |
