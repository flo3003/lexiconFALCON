USE LexiconFALCON_db;

LOAD DATA LOCAL INFILE 'glove-python/coo_matrix.csv' INTO TABLE coo_matrix FIELDS TERMINATED BY ',' ENCLOSED BY '' LINES TERMINATED BY '\n' IGNORE 1 ROWS;

LOAD DATA LOCAL INFILE 'glove-python/word_mapping.csv' INTO TABLE word_mapping FIELDS TERMINATED BY ',' ENCLOSED BY '' LINES TERMINATED BY '\n' IGNORE 1 ROWS;

/* Uncomment one of the following depending on your choice of the lexicon file */;

-- LOAD DATA LOCAL INFILE 'glove-python/ppdb.csv' INTO TABLE ppdb FIELDS TERMINATED BY ',' ENCLOSED BY '' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
-- LOAD DATA LOCAL INFILE 'glove-python/framenet.csv' INTO TABLE framenet FIELDS TERMINATED BY ',' ENCLOSED BY '' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
-- LOAD DATA LOCAL INFILE 'glove-python/wordnet.csv' INTO TABLE wordnet FIELDS TERMINATED BY ',' ENCLOSED BY '' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
