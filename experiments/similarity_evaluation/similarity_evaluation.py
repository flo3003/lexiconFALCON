from glove import Glove
from glove import Corpus
import pandas as pd
import numpy 

from gensim.models import KeyedVectors
from gensim.scripts.glove2word2vec import glove2word2vec

import sys
import gensim
import data_io
import os
from scipy.stats import spearmanr

filename = sys.argv[1]
dataset = sys.argv[2]

f_base = os.path.splitext(os.path.basename(filename))[0]
d_base = os.path.splitext(os.path.basename(dataset))[0]

(words, We) = data_io.getWordmap(filename)

wordsim = pd.read_csv(dataset,delimiter=';', names=['word1','word2','sim'], index_col=None)

similarities = []
i=0
tot=0
for index, row in wordsim.iterrows():
    try:
        similarity = numpy.dot(We[words[row['word1']]], We[words[row['word2']]])/(numpy.linalg.norm(We[words[row['word1']]])* numpy.linalg.norm(We[words[row['word2']]]))
        tot+=1
    except KeyError:
        similarity = numpy.nan
        i+=1
    similarities.append(similarity)

wordsim['cosine_similarity'] = similarities
X = wordsim.dropna(how='any')[['sim', 'cosine_similarity']]

sp = spearmanr(X.sim, X.cosine_similarity)

tot+=i

print f_base+","+d_base+","+str(sp[0])+","+str(sp[1])

