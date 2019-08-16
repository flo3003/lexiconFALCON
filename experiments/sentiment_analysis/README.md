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


