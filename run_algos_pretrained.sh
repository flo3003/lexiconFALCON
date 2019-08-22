####### Parameters #######
host="host_name"
user="user"
pass="password"
database="database_name"
emb_dim=embedding_dimensions
epochs=number_of_epochs
emb_file="input_files/pretrainedembeddingsfile_weights.txt"

##### LexiconFALCON #####
lexicon="lexicon"
dP=dP_value
xi=xi_value

######### Train #########
./lexiconfalcon $host $user $pass $database $lexicon $emb_dim $dP $xi $epochs $emb_file
