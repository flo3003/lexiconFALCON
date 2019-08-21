#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <mysql.h>

/* Compile with the following command */
// gcc -O3 lexiconfalcon.c -o lexiconfalcon -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -lm
/* Run with the following command */
// ./lexiconfalcon $server $user $password $database $lexicon $embedding_dimension $dP $ksi $number_of_epochs

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define X_MAX 100.0
#define ALPHA 0.75
#define SIZE 1024

char *lexicon;
double dP;
double ksi;
int Max_Epochs;
int g_cnt=0;
char query_string[200];
char *path_to_pretrained;

struct connection_details
{
  char *server;
  char *user;
  char *password;
  char *database;
};

MYSQL* mysql_connection_setup(struct connection_details mysql_details)
{
  /* first of all create a mysql instance and initialize the variables within */
  MYSQL *connection = mysql_init(NULL);

  /* connect to the database with the details attached */
  if (!mysql_real_connect(connection,mysql_details.server, mysql_details.user, mysql_details.password, mysql_details.database, 0, NULL, 0)) {
    printf("Conection error : %s\n", mysql_error(connection));
    exit(1);
  }
  return connection;
}

MYSQL_RES* mysql_perform_query(MYSQL *connection, char *sql_query)
{
  /* send the query to the database */
  if (mysql_query(connection, sql_query))
  {
    printf("MySQL query error : %s\n", mysql_error(connection));
    exit(1);
  }

  return mysql_use_result(connection);
}

/* basic functions */

void calc_features(int TOTAL_FEATURES);

int initialize_weights (int TOTAL_FEATURES);

void set_F(int TOTAL_FEATURES);

double get_feature_gradients(int TOTAL_FEATURES);

void features_update(int TOTAL_FEATURES);

void save_new_features_files(int TOTAL_FEATURES, double sq1);

/* secondary functions */

int get_num_lines(char *filename);

double predict_coo(int word_a_Id, int word_b_Id, int TOTAL_FEATURES);

FILE *lgfile=NULL;

void lg(char *fmt,...);

void lgopen(int argc, char**argv);

void error(char *fmt,...);

MYSQL *conn;    // the connection
MYSQL_RES *res; // the results
MYSQL_ROW row;  // the results row (line by line)

struct connection_details mysqlD;

int TOTAL_PRIORS=400000;
int TOTAL_WORDS;
int TOTAL_WORDS;
double GLOBAL_AVERAGE;
double GLOBAL_SCALED_AVERAGE;
int min_c,max_c, cooccurrence_range;
double avg;

double **word_a_features;     // Array of features by word_a (using doubles to save space)
double **word_b_features;   // Array of features by word_b (using doubles to save space)
double *word_a_bias;
double *word_b_bias;

double **prior_embeddings;

int **cooccurrence_ids;
int *cooccurrence_size;
double **cooccurrence_value;

double **D_word_a_features;     // Array of features by word_a (using doubles to save space)
double **D_word_b_features;   // Array of features by word_b (using doubles to save space)
double *D_word_a_bias;
double *D_word_b_bias;

double **word_a_features_gradients;     // Array of gradients for features by word_a (using doubles to save space)
double **word_b_features_gradients;   // Array of gradients for features by word_b (using doubles to save space)
double *word_a_bias_gradients;
double *word_b_bias_gradients;

double **F_word_a_features_gradients;     // Array of gradients for features by word_a (using doubles to save space)
double **F_word_b_features_gradients;   // Array of gradients for features by word_b (using doubles to save space)

double *F_word_a_bias_gradients;
double *F_word_b_bias_gradients;

int *synonyms_size;
int **word_synonyms;

int word_a_Id, word_b_Id, synonym_Id;

int num_cooccurrences;
int num_synonyms;

int num_word_a;
int *all_word_a_Ids;
int num_word_b;
int *all_word_b_Ids;

/* ------------------------------------------------------------------------- */

main (int argc, char**argv) {

  lgopen(argc,argv);

  double prediction;
  unsigned int c,i,j,f,h;
  time_t start, stop;
  double diff;

  /* start timer */
  start = time(NULL);

  mysqlD.server = argv[1];  // where the mysql database is
  mysqlD.user = argv[2];   // the root user of mysql
  mysqlD.password = argv[3]; // the password of the root user in mysql
  mysqlD.database = argv[4]; // the database to pick
  lexicon = argv[5]; // semantic lexicon
  int TOTAL_FEATURES = atoi(argv[6]); // embedding dimension
  dP = atof(argv[7]); // dP
  ksi = atof(argv[8]); // xi
  Max_Epochs = atoi(argv[9]); // maximum number of epochs
  path_to_pretrained = argv[10];

  lg("Using %s lexicon.. Epochs: %d, dP: %g, xi: %g and embedding dimension: %d\n", lexicon, Max_Epochs, dP, ksi, TOTAL_FEATURES);

  // connect to the mysql database
  conn = mysql_connection_setup(mysqlD);

  /* ------------------------------------------------------------------------- */
  // Get size of vocabulary from word_mapping table
  sprintf(query_string,"SELECT count(*) FROM word_mapping");

  res = mysql_perform_query(conn,query_string);

  while ((row = mysql_fetch_row(res)) !=NULL) {
    TOTAL_WORDS=atoi(row[0]);
  }

  //clean up the database result set
  mysql_free_result(res);

  TOTAL_WORDS=TOTAL_WORDS;

  /* ------------------------------------------------------------------------- */
  // Get average cooccurrence from the cooccurrence table

  sprintf(query_string,"SELECT avg(cooccurrence) FROM coo_matrix");

  res = mysql_perform_query(conn,query_string);

  while ((row = mysql_fetch_row(res)) !=NULL) {
    GLOBAL_AVERAGE=atof(row[0]);
  }

  //clean up the database result set
  mysql_free_result(res);

  /* ------------------------------------------------------------------------- */
  // Get maximum cooccurrence from the cooccurrence table

  sprintf(query_string,"SELECT MAX(cooccurrence) FROM coo_matrix");

  res = mysql_perform_query(conn,query_string);

  while ((row = mysql_fetch_row(res)) !=NULL) {
    max_c=atoi(row[0]);
  }

  /* clean up the database result set */
  mysql_free_result(res);

  /* ------------------------------------------------------------------------- */
  // Get minimum cooccurrence from the cooccurrence table

  sprintf(query_string,"SELECT MIN(cooccurrence) FROM coo_matrix");

  res = mysql_perform_query(conn,query_string);

  while ((row = mysql_fetch_row(res)) !=NULL) {
    min_c=atoi(row[0]);
  }

  /* clean up the database result set */
  mysql_free_result(res);

  cooccurrence_range=max_c-min_c;
  avg = (GLOBAL_AVERAGE - min_c) / cooccurrence_range;
  GLOBAL_SCALED_AVERAGE = log(avg / (1 - avg));

  prior_embeddings = ( double** )malloc(TOTAL_PRIORS * sizeof(double *));

  if(prior_embeddings == NULL)
  {
    fprintf(stderr, "out of memory for prior embeddings array\n");
    exit(-1);
  }
  for(i = 0; i < TOTAL_PRIORS; i++)
  {
    prior_embeddings[i] = ( double* )malloc(TOTAL_FEATURES * sizeof(double));
    if(prior_embeddings[i] == NULL)
    {
      fprintf(stderr, "out of memory for prior embeddings array row\n");
      exit(-1);
    }
  }

  word_a_features = ( double** )malloc(TOTAL_WORDS * sizeof(double *));

  if(word_a_features == NULL)
  {
    fprintf(stderr, "out of memory for word_a features array\n");
    exit(-1);
  }
  for(i = 0; i < TOTAL_WORDS; i++)
  {
    word_a_features[i] = ( double* )malloc(TOTAL_FEATURES * sizeof(double));
    if(word_a_features[i] == NULL)
    {
      fprintf(stderr, "out of memory for word_a features row\n");
      exit(-1);
    }
  }

  D_word_a_features = ( double** )malloc(TOTAL_WORDS * sizeof(double *));

  if(D_word_a_features == NULL)
  {
    fprintf(stderr, "out of memory for word_a features array\n");
    exit(-1);
  }
  for(i = 0; i < TOTAL_WORDS; i++)
  {
    D_word_a_features[i] = ( double* )malloc(TOTAL_FEATURES * sizeof(double));
    if(D_word_a_features[i] == NULL)
    {
      fprintf(stderr, "out of memory for word_a features row\n");
      exit(-1);
    }
  }

  word_b_features = ( double** )malloc(TOTAL_WORDS * sizeof(double *));

  if(word_b_features == NULL)
  {
    fprintf(stderr, "out of memory for word_b features array\n");
    exit(-1);
  }
  for(i = 0; i < TOTAL_WORDS; i++)
  {
    word_b_features[i] = ( double* )malloc(TOTAL_FEATURES * sizeof(double));
    if(word_b_features[i] == NULL)
    {
      fprintf(stderr, "out of memory for word_b features row\n");
      exit(-1);
    }
  }

  D_word_b_features = ( double** )malloc(TOTAL_WORDS * sizeof(double *));

  if(D_word_b_features == NULL)
  {
    fprintf(stderr, "out of memory for word_b features array\n");
    exit(-1);
  }
  for(i = 0; i < TOTAL_WORDS; i++)
  {
    D_word_b_features[i] = ( double* )malloc(TOTAL_FEATURES * sizeof(double));
    if(D_word_b_features[i] == NULL)
    {
      fprintf(stderr, "out of memory for word_b features row\n");
      exit(-1);
    }
  }

  word_a_bias =  (double *)malloc(sizeof(double)*TOTAL_WORDS);

  D_word_a_bias =  (double *)malloc(sizeof(double)*TOTAL_WORDS);

  word_b_bias =  (double *)malloc(sizeof(double)*TOTAL_WORDS);

  D_word_b_bias =  (double *)malloc(sizeof(double)*TOTAL_WORDS);

  word_a_features_gradients = ( double** )malloc(TOTAL_WORDS * sizeof(double *));

  if(word_a_features_gradients == NULL)
  {
    fprintf(stderr, "out of memory for word_a gradients array\n");
    exit(-1);
  }
  for(i = 0; i < TOTAL_WORDS; i++)
  {
    word_a_features_gradients[i] = ( double* )malloc(TOTAL_FEATURES * sizeof(double));
    if(word_a_features_gradients[i] == NULL)
    {
      fprintf(stderr, "out of memory for word_a gradients array row\n");
      exit(-1);
    }
  }

  F_word_a_features_gradients = ( double** )malloc(TOTAL_WORDS * sizeof(double *));

  if(F_word_a_features_gradients == NULL)
  {
    fprintf(stderr, "out of memory for word_a gradients array\n");
    exit(-1);
  }
  for(i = 0; i < TOTAL_WORDS; i++)
  {
    F_word_a_features_gradients[i] = ( double* )malloc(TOTAL_FEATURES * sizeof(double));
    if(F_word_a_features_gradients[i] == NULL)
    {
      fprintf(stderr, "out of memory for word_a gradients array row\n");
      exit(-1);
    }
  }

  word_b_features_gradients = ( double** )malloc(TOTAL_WORDS * sizeof(double *));

  if(word_b_features_gradients == NULL)
  {
    fprintf(stderr, "out of memory for word_b gradients array\n");
    exit(-1);
  }
  for(i = 0; i < TOTAL_WORDS; i++)
  {
    word_b_features_gradients[i] = ( double* )malloc(TOTAL_FEATURES * sizeof(double));
    if(word_b_features_gradients[i] == NULL)
    {
      fprintf(stderr, "out of memory for word_b gradients row\n");
      exit(-1);
    }
  }

  F_word_b_features_gradients = ( double** )malloc(TOTAL_WORDS * sizeof(double *));

  if(F_word_b_features_gradients == NULL)
  {
    fprintf(stderr, "out of memory for word_b gradients array\n");
    exit(-1);
  }
  for(i = 0; i < TOTAL_WORDS; i++)
  {
    F_word_b_features_gradients[i] = ( double* )malloc(TOTAL_FEATURES * sizeof(double));
    if(F_word_b_features_gradients[i] == NULL)
    {
      fprintf(stderr, "out of memory for word_b gradients row\n");
      exit(-1);
    }
  }

  word_a_bias_gradients =  (double *)malloc(sizeof(double)*TOTAL_WORDS);

  F_word_a_bias_gradients =  (double *)malloc(sizeof(double)*TOTAL_WORDS);

  word_b_bias_gradients =  (double *)malloc(sizeof(double)*TOTAL_WORDS);

  F_word_b_bias_gradients =  (double *)malloc(sizeof(double)*TOTAL_WORDS);

  word_synonyms = ( int** )malloc(TOTAL_WORDS * sizeof(int *));

  if(word_synonyms == NULL)
  {
    fprintf(stderr, "out of memory for word_b connections\n");
    exit(-1);
  }

  synonyms_size =  (int *)malloc(sizeof(int)*TOTAL_WORDS);

  cooccurrence_ids = ( int** )malloc(TOTAL_WORDS * sizeof(int *));

  if(cooccurrence_ids == NULL)
  {
    fprintf(stderr, "out of memory for word_b connections\n");
    exit(-1);
  }

  cooccurrence_size =  (int *)malloc(sizeof(int)*TOTAL_WORDS);

  cooccurrence_value = ( double** )malloc(TOTAL_WORDS * sizeof(double *));

  if(cooccurrence_value == NULL)
  {
    fprintf(stderr, "out of memory for word_b connections\n");
    exit(-1);
  }

  /* ------------------------------------------------------------------------- */
  // We count the distinct words from the first column of the cooccurrence matrix and store them

  sprintf(query_string,"SELECT COUNT(DISTINCT word_a) FROM coo_matrix");

  res = mysql_perform_query(conn,query_string);

  while ((row = mysql_fetch_row(res)) !=NULL) {
    num_word_a=atoi(row[0]);
  }

  /* clean up the database result set */
  mysql_free_result(res);


  all_word_a_Ids = (int *)malloc(sizeof(int)*num_word_a);

  sprintf(query_string,"SELECT DISTINCT word_a FROM coo_matrix ORDER BY word_a");

  res = mysql_perform_query(conn,query_string);

  h=0;
  while ((row = mysql_fetch_row(res)) !=NULL) {
    all_word_a_Ids[h]=atoi(row[0]);
    h++;
  }

  /* clean up the database result set */
  mysql_free_result(res);

  /* ------------------------------------------------------------------------- */
  // We count the distinct words from the second column of the cooccurrence matrix and store them

  sprintf(query_string,"SELECT COUNT(DISTINCT word_b) FROM coo_matrix");

  res = mysql_perform_query(conn,query_string);

  while ((row = mysql_fetch_row(res)) !=NULL) {
    num_word_b=atoi(row[0]);
  }

  /* clean up the database result set */
  mysql_free_result(res);

  all_word_b_Ids = (int *)malloc(sizeof(int)*num_word_b);

  sprintf(query_string,"SELECT DISTINCT word_b FROM coo_matrix ORDER BY word_b");

  res = mysql_perform_query(conn,query_string);

  h=0;
  while ((row = mysql_fetch_row(res)) !=NULL) {
    all_word_b_Ids[h]=atoi(row[0]);
    h++;
  }

  /* clean up the database result set */
  mysql_free_result(res);

  /* ------------------------------------------------------------------------- */
  // We count the number of words that each word cooccurs with

  for (c=0; c<TOTAL_WORDS;c++) {

    word_a_Id = c+1;

    sprintf(query_string,"SELECT count(word_b) FROM coo_matrix WHERE word_a=%d",word_a_Id);

    res = mysql_perform_query(conn,query_string);

    while ((row = mysql_fetch_row(res)) !=NULL) {
      num_cooccurrences=atoi(row[0]);
    }

    /* clean up the database result set */
    mysql_free_result(res);


    if (num_cooccurrences!=0) {

      cooccurrence_size[c] = num_cooccurrences;
      cooccurrence_ids[c] = ( int* )malloc(num_cooccurrences * sizeof(int));

      if(cooccurrence_ids[c] == NULL)
      {
        fprintf(stderr, "out of memory for connections of word_a %d\n", word_a_Id);
        exit(-1);
      }

      cooccurrence_value[c] = ( double* )malloc(num_cooccurrences * sizeof(double));

      if(cooccurrence_value[c] == NULL)
      {
        fprintf(stderr, "out of memory for connections of word_a %d\n", word_a_Id);
        exit(-1);
      }

      /* ------------------------------------------------------------------------- */
      // We select the words that each word cooccurs with and store their ids and their cooccurrence value

      sprintf(query_string,"SELECT word_b, cooccurrence FROM coo_matrix WHERE word_a=%d ORDER BY word_b ASC",word_a_Id);

      res = mysql_perform_query(conn,query_string);

      h=0;
      while ((row = mysql_fetch_row(res)) !=NULL) {
        cooccurrence_ids[c][h]=atoi(row[0]);
        cooccurrence_value[c][h]=atof(row[1]);
        h++;
      }

      /* clean up the database result set */
      mysql_free_result(res);

    } // if a word cooccurs with another word

    /* ------------------------------------------------------------------------- */
    // We count the number of words that each word is semantically related with and store them

    sprintf(query_string,"SELECT count(target_id) FROM %s WHERE source_id=%d", lexicon, word_a_Id);

    res = mysql_perform_query(conn,query_string);

    ///fetch all SELECTed rows
    while ((row = mysql_fetch_row(res)) !=NULL) {
      num_synonyms=atoi(row[0]);
    }

    /* clean up the database result set */
    mysql_free_result(res);

    if (num_synonyms!=0) {

      synonyms_size[c]=num_synonyms;
      word_synonyms[c] = ( int* )malloc(num_synonyms * sizeof(int));

      if(word_synonyms[c] == NULL)
      {
        fprintf(stderr, "out of memory for connections of word_a %d\n", word_a_Id);
        exit(-1);
      }

      sprintf(query_string,"SELECT target_id FROM %s WHERE source_id=%d", lexicon, word_a_Id);

      res = mysql_perform_query(conn,query_string);

      h=0;
      while ((row = mysql_fetch_row(res)) !=NULL) {
        word_synonyms[c][h]=atoi(row[0]);
        h++;
      }

      /* clean up the database result set */
      mysql_free_result(res);
    } // if a word is semantically related to another

  } // for all word ids

  /* stop timer and display time */
  stop = time(NULL);
  diff = difftime(stop, start);
  lg("SQL queries completed in %f sec..\n", diff);

  /* ------------------------------------------------------------------------- */
  // Now that have stored all the information, we proceed with the algorithm
  /* ------------------------------------------------------------------------- */

  /* start timer */
  start = time(NULL);
  char algorithm_name[10];
  sscanf(argv[0], "./%s", algorithm_name);
  lg("Running %s..\n", algorithm_name);

  calc_features(TOTAL_FEATURES);

  /* stop timer and display time */
  stop = time(NULL);
  diff = difftime(stop, start);
  lg("%f sec\n", diff);

  exit(-1);

}

/* basic functions */

void calc_features(int TOTAL_FEATURES) {

  time_t start, stop;
  double diff, avg_diff=0.0, sq1;
  int i,f;

  /* Uncomment if you want to stop training as soon as the error increases */
  // double rmse_last=9999999999, rmse = 9999999998, MIN_IMPROVEMENT=0.00001;

  i=initialize_weights(TOTAL_FEATURES);

  for (i=0; i<TOTAL_WORDS; i++) {
    word_a_bias[i] = 1.0;
    word_b_bias[i] = 1.0;

    D_word_b_bias[i] = word_b_bias[i];
    D_word_a_bias[i] = word_a_bias[i];

    for (f=0; f<TOTAL_FEATURES; f++) {
       D_word_a_features[i][f] = word_a_features[i][f];
       D_word_b_features[i][f] = word_b_features[i][f];
    }
  }

  set_F(TOTAL_FEATURES);

  //  while ((rmse < rmse_last - MIN_IMPROVEMENT) && g_cnt < Max_Epochs) {
  while (g_cnt < Max_Epochs) {

    g_cnt++;

    /* start timer */
    start = time(NULL);

    sq1=get_feature_gradients(TOTAL_FEATURES);
    // rmse_last = rmse;
    // rmse=sq1;

    features_update(TOTAL_FEATURES);
    set_F(TOTAL_FEATURES);

    /* stop timer and display time */
    stop = time(NULL);
    diff = difftime(stop, start);

    lg("     <Iteration='%d' Training Error='%lf'/> time: %f sec\n", g_cnt, sq1, (double) diff);

    avg_diff+=diff;

    /* Save embeddings regularly */
    if ((g_cnt<150) && (g_cnt % 10 == 0)){
       save_new_features_files(TOTAL_FEATURES,sq1);
    }
    else if ((g_cnt>150)&&(g_cnt % 50 == 0)){
       save_new_features_files(TOTAL_FEATURES,sq1);
    }

  }

  save_new_features_files(TOTAL_FEATURES, sq1);

  mysql_close(conn);

}

int initialize_weights(int TOTAL_FEATURES) {

  int i,j,f;
  FILE *infile;

  /* ------------------------------------------------------------------------- */
  // First initialize all words from the vocabulary randomly

  infile=fopen("input_files/random_initial_vectors.txt","r");

  for (i=0; i<TOTAL_WORDS; i++) {
    for (f=0; f<TOTAL_FEATURES; f++) {
      fscanf(infile,"%lf", &word_a_features[i][f]);
    }
  }

  fclose(infile);

  infile=fopen("input_files/random_initial_vectors.txt","r");

  for (i=0; i<TOTAL_WORDS; i++) {
    for (f=0; f<TOTAL_FEATURES; f++) {
      fscanf(infile,"%lf", &word_b_features[i][f]);
    }
  }

  fclose(infile);

  if (path_to_pretrained) {

    printf("Using pretrained embeddings from %s...\n", path_to_pretrained);
    /* ------------------------------------------------------------------------- */
    // Store all the pretrained vectors

    infile=fopen(path_to_pretrained,"r");

    for (i=0; i<TOTAL_PRIORS; i++) {
      for (f=0; f<TOTAL_FEATURES; f++) {
        fscanf(infile,"%lf", &prior_embeddings[i][f]);
      }
    }

    fclose(infile);

    /* ------------------------------------------------------------------------- */
    // The mapping file maps the indices of the vocabulary words to the indices of the corresponding words in the pretrained vector file for example word "the" has index "3" in the vocabulary and index "56" in the pretrained vector file. The mapping file maps 3 to 56.

    int num_lines;
    char *mapping_file="input_files/mapped_ids.txt";
    int **mapping_array;

    num_lines = get_num_lines(mapping_file);

    mapping_array = ( int** )malloc(num_lines * sizeof(int *));

    if(mapping_array == NULL)
    {
      fprintf(stderr, "out of memory for mapping array\n");
      exit(-1);
    }

    for(i = 0; i < num_lines; i++)
    {
      mapping_array[i] = ( int* )malloc(2 * sizeof(int));
      if(mapping_array[i] == NULL)
      {
        fprintf(stderr, "out of memory for mapping array row\n");
        exit(-1);
      }
    }

    if (NULL == (infile = fopen(mapping_file, "r"))) {
      perror(mapping_file);
      return EXIT_FAILURE;
    }


    for (i=0; i<num_lines; i++) {
      for (f=0; f<2; f++) {
        fscanf(infile,"%d", &mapping_array[i][f]);
      }

    }

    fclose(infile);

    /* ------------------------------------------------------------------------- */
    // Initialize the words of the vocabulary to the pretrained vectors

    for (i=0; i<num_lines; i++) {
      for (f=0; f<TOTAL_FEATURES; f++) {
        word_b_features[ mapping_array[i][0] -1 ][f] = prior_embeddings[ mapping_array[i][1] -1 ][f];
        word_a_features[ mapping_array[i][0] -1 ][f] = prior_embeddings[ mapping_array[i][1] -1 ][f];
      }

    }

    free((void **)prior_embeddings);

  } //If pretrained embeddings exist


  return 0;

}

void set_F(int TOTAL_FEATURES) {

  int c,i,f;
  double Tuv, syn_bias, syn_diff[TOTAL_FEATURES];

  for (c=0; c < TOTAL_WORDS; c++)  {

    word_a_Id = c+1;

    // If word_a has semantic relationships
    if (synonyms_size[word_a_Id-1]!=0) {

      Tuv=1.0/synonyms_size[word_a_Id-1];
      syn_bias=0.0;
      for (f=0;f<TOTAL_FEATURES;f++) {
        syn_diff[f] = 0.0;
      }

      // For all words that are semantically related to word_a
      for (i=0;i<synonyms_size[word_a_Id-1];i++) {

        synonym_Id=word_synonyms[word_a_Id-1][i];

        for (f=0;f<TOTAL_FEATURES;f++) {
          syn_diff[f] += D_word_a_features[synonym_Id - 1][f];
        }

        syn_bias += D_word_a_bias[synonym_Id - 1];

      }

    }
    // Else if word_a has no semantic relationships
    else {

      Tuv=1.0;

      for (f=0;f<TOTAL_FEATURES;f++) {
        syn_diff[f] = D_word_a_features[word_a_Id - 1][f];
      }

      syn_bias = D_word_a_bias[word_a_Id - 1];

    }

    for (f=0; f<TOTAL_FEATURES; f++) {
      F_word_a_features_gradients[word_a_Id - 1][f] = syn_diff[f]*Tuv;
    }

    F_word_a_bias_gradients[word_a_Id - 1] = syn_bias*Tuv;

  }

  for (c=0; c < TOTAL_WORDS; c++) {

    word_b_Id = c+1;

    // If word_b has semantic relationships
    if (synonyms_size[word_b_Id-1]!=0) {

      Tuv=1.0/synonyms_size[word_b_Id-1];
      syn_bias=0.0;
      for (f=0;f<TOTAL_FEATURES;f++) {
        syn_diff[f] = 0.0;
      }

      // For all words that are semantically related to word_a
      for (i=0;i<synonyms_size[word_b_Id-1];i++) {

        synonym_Id=word_synonyms[word_b_Id-1][i];

        for (f=0;f<TOTAL_FEATURES;f++) {
          syn_diff[f] += D_word_b_features[synonym_Id - 1][f];
        }

        syn_bias += D_word_b_bias[synonym_Id - 1];

      }

    }
    // Else if word_b has no semantic relationships
    else {

      Tuv=1.0;

      for (f=0;f<TOTAL_FEATURES;f++) {
        syn_diff[f] = D_word_b_features[word_b_Id - 1][f];
      }

      syn_bias = D_word_b_bias[word_b_Id - 1];

    }

    for (f=0; f<TOTAL_FEATURES; f++) {
      F_word_b_features_gradients[word_b_Id - 1][f] = syn_diff[f]*Tuv;
    }

    F_word_b_bias_gradients[word_b_Id - 1] = syn_bias*Tuv;

  }

}

double get_feature_gradients(int TOTAL_FEATURES) {

  int c,i,f;
  double p, sq1, err1, entry_weight;
  double w_b_f, w_a_f;

  sq1=0.0;

  for (i=0; i<TOTAL_WORDS; i++) {
    word_b_bias_gradients[i]=0.0;
    word_a_bias_gradients[i]=0.0;

    for (f=0; f<TOTAL_FEATURES; f++) {
      word_a_features_gradients[i][f] = 0.0;
      word_b_features_gradients[i][f]=0.0;
    }
  }

  for (c=0; c < num_word_a; c++)  {

    word_b_Id = all_word_a_Ids[c];

    if (cooccurrence_size[word_b_Id-1]!=0) {

      for (i=0; i< cooccurrence_size[word_b_Id-1]; i++) {

        word_a_Id=cooccurrence_ids[word_b_Id-1][i];
        p = predict_coo(word_a_Id, word_b_Id, TOTAL_FEATURES);
        entry_weight = pow(MIN(1.0, ((double)cooccurrence_value[word_b_Id-1][i] / X_MAX)), ALPHA);
        err1 = - (entry_weight * (log((double)cooccurrence_value[word_b_Id-1][i]) - p));
        sq1 += -err1*(log((double)cooccurrence_value[word_b_Id-1][i]) - p);

        word_b_bias_gradients[word_b_Id - 1] += 2.0 * err1;
        word_a_bias_gradients[word_a_Id -1] += 2.0 * err1;

        for (f=0; f<TOTAL_FEATURES; f++) {
          w_a_f = word_a_features[word_a_Id - 1][f];
          w_b_f = word_b_features[word_b_Id - 1][f];

          word_a_features_gradients[word_a_Id - 1][f] += 2.0 * err1 * w_b_f;
          word_b_features_gradients[word_b_Id - 1][f] += 2.0 * err1 * w_a_f;

        }
      }
    }
  }

  return sq1;

}

void features_update(int TOTAL_FEATURES) {

  int c,f;
  double wa_upd,wb_upd,wa_b_upd,wb_b_upd,dwdw_b,dwdw_a;
  long double IJJ,IJF,IFF;
  long double dQ, lamda2, lamda1;

  for (c=0; c < num_word_a; c++)  {

    word_b_Id = all_word_a_Ids[c];

    IJJ = 0;
    IJF = 0;
    IFF = 0;

    dwdw_b=0.0;

    IJJ += pow(word_b_bias_gradients[word_b_Id - 1],2);
    IJF += word_b_bias_gradients[word_b_Id - 1] * F_word_b_bias_gradients[word_b_Id - 1];
    IFF += pow(F_word_b_bias_gradients[word_b_Id - 1],2);

    for (f=0; f<TOTAL_FEATURES; f++) {
      IJJ += pow(word_b_features_gradients[word_b_Id - 1][f],2);
      IJF += word_b_features_gradients[word_b_Id - 1][f] * F_word_b_features_gradients[word_b_Id - 1][f];
      IFF += pow(F_word_b_features_gradients[word_b_Id - 1][f],2);
    }

    if ( fabs(IFF*IJJ-IJF*IJF) < 1.e-10 ) {
      continue;
    }

    dQ=-ksi*dP*sqrt(IJJ);
    lamda2=0.5*1/sqrt(((IJJ*dP*dP)-dQ*dQ)/(IFF*IJJ-IJF*IJF));
    lamda1=(IJF-(2*lamda2*dQ))/IJJ;
    wb_b_upd = -((lamda1/(2*lamda2))*word_b_bias_gradients[word_b_Id - 1]) + ((1/(2*lamda2))*F_word_b_bias_gradients[word_b_Id - 1]);
    dwdw_b += wb_b_upd*wb_b_upd;

    word_b_bias[word_b_Id - 1] += wb_b_upd;
    D_word_b_bias[word_b_Id - 1] = wb_b_upd;

    for (f=0; f<TOTAL_FEATURES; f++) {
      wb_upd = -((lamda1/(2*lamda2))*word_b_features_gradients[word_b_Id - 1][f]) + ((1/(2*lamda2))*F_word_b_features_gradients[word_b_Id - 1][f]);

      word_b_features[word_b_Id - 1][f] += wb_upd;
      D_word_b_features[word_b_Id - 1][f] = wb_upd;
      dwdw_b += wb_upd*wb_upd;

    }

  }

  for (c=0; c < num_word_b; c++)  {

    word_a_Id = all_word_b_Ids[c];

    IJJ=0;
    IJF=0;
    IFF=0;

    dwdw_a=0.0;

    IJJ += pow(word_a_bias_gradients[word_a_Id - 1],2);
    IJF += word_a_bias_gradients[word_a_Id - 1] * F_word_a_bias_gradients[word_a_Id - 1];
    IFF += pow(F_word_a_bias_gradients[word_a_Id - 1],2);

    for (f=0; f<TOTAL_FEATURES; f++) {
      IJJ += pow(word_a_features_gradients[word_a_Id - 1][f],2);
      IJF += word_a_features_gradients[word_a_Id - 1][f] * F_word_a_features_gradients[word_a_Id - 1][f];
      IFF += pow(F_word_a_features_gradients[word_a_Id - 1][f],2);
    }

    dQ=-ksi*dP*sqrt(IJJ);
    lamda2=0.5*1/sqrt(((IJJ*dP*dP)-dQ*dQ)/(IFF*IJJ-IJF*IJF));
    lamda1=(IJF-(2*lamda2*dQ))/IJJ;

    if ( fabs(IFF*IJJ-IJF*IJF) < 1.e-18) {
      continue;
    }

    wa_b_upd = -((lamda1/(2*lamda2))*word_a_bias_gradients[word_a_Id - 1]) + ((1/(2*lamda2))*F_word_a_bias_gradients[word_a_Id - 1]);

    dwdw_a += wa_b_upd*wa_b_upd;
    word_a_bias[word_a_Id - 1] += wa_b_upd;
    D_word_a_bias[word_a_Id - 1] = wa_b_upd;

    for (f=0; f<TOTAL_FEATURES; f++) {
      wa_upd = -((lamda1/(2*lamda2))*word_a_features_gradients[word_a_Id - 1][f]) + ((1/(2*lamda2))*F_word_a_features_gradients[word_a_Id - 1][f]);

      word_a_features[word_a_Id - 1][f] += wa_upd;
      D_word_a_features[word_a_Id - 1][f] = wa_upd;

      dwdw_a += wa_upd*wa_upd;
    }
  }
}

void save_new_features_files (int TOTAL_FEATURES, double sq1) {

  char my_word[100] = "";
  char filetype[600] = "";
  char filename[700];
  char folder[700] = "embeddings/";
  float my_value;
  unsigned int i, j;


  strcpy (filetype, ".txt");
  if (path_to_pretrained) {
    sprintf(filename, "LF_pretrained_embeddings_dP_%g_xi_%g_num_epochs_%d_final_error_%lf_%s", dP, ksi, g_cnt, sq1, lexicon);
  } else {
    sprintf(filename, "LF_embeddings_dP_%g_xi_%g_num_epochs_%d_final_error_%lf_%s", dP, ksi, g_cnt, sq1, lexicon);
  }
  strcat (folder, filename);
  strcat (folder, filetype);

  FILE *ctrans_file = fopen(folder, "w"); /* open for writing */

  lg("\n\nSaving files... \n");


  for (i=0; i < TOTAL_WORDS; i++) {

    sprintf(query_string,"SELECT name FROM word_mapping WHERE word_id=%d",i+1);

    res = mysql_perform_query(conn,query_string);

    while ((row = mysql_fetch_row(res)) !=NULL) {
      strcpy (my_word, row[0]);
    }

    /* clean up the database result set */
    mysql_free_result(res);

    fprintf(ctrans_file,"%s ",my_word);

    for (j=0; j < TOTAL_FEATURES; j++) {

      my_value = word_b_features[i][j]+word_a_features[i][j];

      fprintf(ctrans_file,"%g ",my_value);

    }

    fprintf(ctrans_file,"\n");

  }

  fclose(ctrans_file);

}

/* secondary functions */

int get_num_lines(char *filename) {
  FILE *in_file;
  char buffer[SIZE + 1], lastchar = '\n';
  size_t bytes;
  int lines = 0;

  if (NULL == (in_file = fopen(filename, "r"))) {
      perror(filename);
      return EXIT_FAILURE;
  }

  while ((bytes = fread(buffer, 1, sizeof(buffer) - 1, in_file))) {
    lastchar = buffer[bytes - 1];
    for (char *c = buffer; (c = memchr(c, '\n', bytes - (c - buffer))); c++) {
        lines++;
    }
  }
  if (lastchar != '\n') {
    lines++;  /* Count the last line even if it lacks a newline */
  }
  if (ferror(in_file)) {
    perror(filename);
    fclose(in_file);
    return EXIT_FAILURE;
  }

  fclose(in_file);

  return (lines);

}

double predict_coo(int word_a_Id, int word_b_Id, int TOTAL_FEATURES) {

  int f;
  float sum = 0.0;

  for (f=0; f<TOTAL_FEATURES; f++) {
     sum += word_a_features[word_a_Id - 1][f] * word_b_features[word_b_Id - 1][f];
  }

  sum += word_b_bias[word_b_Id - 1] + word_a_bias[word_a_Id - 1];

  return sum;

}

void lg(char *fmt,...) {
  char buf[2048];
  va_list ap;

  va_start(ap, fmt);
  vsprintf(buf,fmt,ap);
  va_end(ap);
  fprintf(stderr,"%s",buf);
  if(lgfile) {
    fprintf(lgfile,"%s",buf);
    fflush(lgfile);
  }
}

void lgopen(int argc, char**argv) {
  lgfile=fopen("log_file.txt","a");
  if(!lgfile) error("Cant open log file");
  /* Print out the date and time in the standard format.  */
  time_t curtime=time(NULL);
}

void error(char *fmt,...) {
  char buf[2048];
  va_list ap;

  va_start(ap, fmt);
  vsprintf(buf,fmt,ap);
  va_end(ap);
  lg("%s",buf);
  lg("\n");
  exit(1);
}
