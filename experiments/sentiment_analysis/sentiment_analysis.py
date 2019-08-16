import pandas as pd
import os
from nltk.corpus import stopwords
import nltk.data
import numpy as np
from gensim.models import Word2Vec

from sklearn.linear_model import LogisticRegression

from KaggleWord2VecUtility import KaggleWord2VecUtility

import warnings

with warnings.catch_warnings():
    warnings.filterwarnings("ignore",category=DeprecationWarning)
    from sklearn.cross_validation import cross_val_score
    from sklearn.grid_search import GridSearchCV
    from sklearn.ensemble import RandomForestClassifier

from sklearn.metrics import accuracy_score, f1_score, confusion_matrix

import data_io
import os
import csv
import sys

#  Define functions to create average word vectors  #

def makeFeatureVec(words, word_vectors, word_dict, num_features):
    # Function to average all of the word vectors in a given
    # paragraph

    # Pre-initialize an empty numpy array (for speed)
    featureVec = np.zeros((num_features,),dtype="float32")

    nwords = 0.

    # Loop over each word in the review and, if it is in the model's
    # vocabulary, add its feature vector to the total
    for word in words:
        if word in word_dict:
            nwords = nwords + 1.
            featureVec = np.add(featureVec,word_vectors[word_dict[word]])

    # Return 1 if the review cannot be classified
    exits = 0
    if (nwords==0):
        exists = exists + 1

    # Divide the result by the number of words to get the average
    else:
        featureVec = np.divide(featureVec,nwords)

    return exists, featureVec


def getAvgFeatureVecs(reviews, word_vectors, word_dict, num_features):
    # Given a set of reviews (each one a list of words), calculate
    # the average feature vector for each one and return a 2D numpy array

    # Initialize a counter
    counter = 0

    # Preallocate a 2D numpy array, for speed
    reviewFeatureVecs = np.zeros((len(reviews),num_features),dtype="float32")

    # Loop through the reviews
    overall_exists = 0

    for review in reviews:

       # Call the function (defined above) that makes average feature vectors
       exists_output, reviewFeatureVecs[counter] = makeFeatureVec(review, word_vectors, word_dict, num_features)

       # Count how many reviews cannot be classified
       if exists_output == 0:
           counter = counter + 1

       overall_exists = overall_exists + out_pipa

    p = counter

    return p, reviewFeatureVecs

def getCleanReviews(reviews):
    clean_reviews = []
    for review in reviews["review"]:
        clean_reviews.append( KaggleWord2VecUtility.review_to_wordlist( review, remove_stopwords=False ))
    return clean_reviews

if __name__ == '__main__':

    # Read data from files
    train = pd.read_csv( os.path.join(os.path.dirname(__file__), 'data', 'labeledTrainData.tsv'), header=0, delimiter="\t", quoting=3 )
    test = pd.read_csv(os.path.join(os.path.dirname(__file__), 'data', 'testData.tsv'), header=0, delimiter="\t", quoting=3 )

    #Data Leak
    test["sentiment"] = test["id"].map(lambda x: 1 if int(x.strip('"').split("_")[1]) >= 5 else 0)
    y_test = test["sentiment"]

    vectors = sys.argv[1]

    (words, We) = data_io.getWordmap(vectors)

    num_features = We.shape[1]

    p, trainDataVecs = getAvgFeatureVecs( getCleanReviews(train), We, words, num_features )
    print('Train: {0} '.format(p))

    p, testDataVecs = getAvgFeatureVecs( getCleanReviews(test), We, words, num_features )
    print('Test: {0} '.format(p))

    log_reg = LogisticRegression()

    print "Fitting a logistic regression model to labeled training data..."
    log_reg = log_reg.fit( trainDataVecs, train["sentiment"] )

    result = log_reg.predict( testDataVecs )

    y_pred = (result > 0.5)
    test = (y_test.values==1)
    my_df = pd.DataFrame({'test':test,'prediction':y_pred})

    f_name = os.path.splitext(os.path.basename(str(vectors)))[0]
    my_df.to_csv('results/' + f_name + '.csv',index=False,header=False)
    print('Accuracy: {0}'.format(accuracy_score(y_pred, y_test)))
    print('F1-score: {0}'.format(f1_score(y_pred, y_test)))
    print('Confusion matrix:')
    print confusion_matrix(y_pred, y_test)
