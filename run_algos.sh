####### Parameters #######
host="host_name"
user="user"
pass="password"
database="database_name"
emb_dim="embedding_dimensions"
epochs="number_of_epochs"

######### GloVe #########
lr=learning_rate
##### LexiconFALCON #####
lexicon="lexicon"
dP=dP_value
xi=xi_value

######### Train #########
./lexiconfalcon $host $user $pass $database $lexicon $emb_dim $dP $xi $epochs
./glove $host $user $pass $database $emb_dim $lr $epochs
