USE LexiconFALCON_db;

LOAD DATA LOCAL INFILE 'glove-python/coo_matrix.csv' INTO TABLE coo_matrix FIELDS TERMINATED BY ',' ENCLOSED BY '' LINES TERMINATED BY '\n' IGNORE 1 ROWS;

LOAD DATA LOCAL INFILE 'glove-python/word_mapping.csv' INTO TABLE word_mapping FIELDS TERMINATED BY ',' ENCLOSED BY '' LINES TERMINATED BY '\n' IGNORE 1 ROWS;

LOAD DATA LOCAL INFILE 'glove-python/word_mapping.csv' INTO TABLE word_mapping FIELDS TERMINATED BY ',' ENCLOSED BY '' LINES TERMINATED BY '\n' IGNORE 1 ROWS;
