## Database Structure

The co-cooccurrence matrix is stored in table `coo_matrix`  and has the following structure:

#### Example:
| *word_a* | *word_b* | *cooccurrence* | *log_cooccurrence* |
| -------- | -------- | -------------- | ------------------ |
| 1        | 2        | 8              | 2.07944            |
| 1        | 3        | 1.75           | 0.559616           |
| 1        | 4        | 0.533333361148 | -0.628609          |
| 1        | 5        | 0.25           | -1.38629           |

Table `word_mapping` maps each word to an Id and has the following structure:

#### Example:
| *word_id* | *name*   |
| --------- | -------- |
| 1         | bromwell |
| 2         | high     |
| 3         | is       |
| 4         | a        |

