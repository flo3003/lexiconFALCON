#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <mysql.h>

/* Compile with the following command */
// gcc -O3 glove.c -o glove -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient -lm
/* Run with the following command */
// ./glove $server $user $password $database $embedding_dimension $learning_rate $number_of_epochs

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define X_MAX 100.0
#define ALPHA 0.75
#define SIZE 1024

double init_my_lr;
int Max_Epochs;
int g_cnt=0;
char query_string[200];

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

int initialize_weights(int TOTAL_FEATURES);

double get_feature_gradients(int TOTAL_FEATURES);

void save_new_features_files(int TOTAL_FEATURES, double sq1);

/* secondary functions */

int get_num_lines(char *filename);

double predict_coo(int word_a_Id, int word_b_Id, int TOTAL_FEATURES);

float fastsqrt(float val);

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

double **word_features;
double *word_bias;

double **prior_embeddings;

int **cooccurrence_ids;
int *cooccurrence_size;
double **cooccurrence_value;
double **log_cooccurrence_value;

double **word_features_gradients;
double *word_bias_gradients;

int word_a_Id, word_b_Id;

int num_cooccurrences;

int num_words;
int *all_word_a_Ids;

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
  int TOTAL_FEATURES = atoi(argv[5]); // embedding dimension
  init_my_lr=atof(argv[6]); // initial learning rate
  Max_Epochs =atoi(argv[7]); // maximum number of epochs

  lg("Epochs: %d, learning rate: %g and embedding dimension: %d\n", Max_Epochs, init_my_lr, TOTAL_FEATURES);

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


  word_features = ( double** )malloc(TOTAL_WORDS * sizeof(double *));

  if(word_features == NULL)
  {
    fprintf(stderr, "out of memory for word features array\n");
    exit(-1);
  }
  for(i = 0; i < TOTAL_WORDS; i++)
  {
    word_features[i] = ( double* )malloc(TOTAL_FEATURES * sizeof(double));
    if(word_features[i] == NULL)
    {
      fprintf(stderr, "out of memory for word features row\n");
      exit(-1);
    }
  }

  word_bias =  (double *)malloc(sizeof(double)*TOTAL_WORDS);

  word_features_gradients = ( double** )malloc(TOTAL_WORDS * sizeof(double *));

    if(word_features_gradients == NULL)
      {
      fprintf(stderr, "out of memory for word features array\n");
      exit(-1);
          }
    for(i = 0; i < TOTAL_WORDS; i++)
      {
      word_features_gradients[i] = ( double* )malloc(TOTAL_FEATURES * sizeof(double));
      if(word_features_gradients[i] == NULL)
        {
        fprintf(stderr, "out of memory for word features row\n");
        exit(-1);
        }
      }

      word_bias_gradients =  (double *)malloc(sizeof(double)*TOTAL_WORDS);




  cooccurrence_size =  (int *)malloc(sizeof(int)*TOTAL_WORDS);

  cooccurrence_ids = ( int** )malloc(TOTAL_WORDS * sizeof(int *));

  if(cooccurrence_ids == NULL)
  {
    fprintf(stderr, "out of memory for user connections\n");
    exit(-1);
  }

  cooccurrence_value = ( double** )malloc(TOTAL_WORDS * sizeof(double *));

  if(cooccurrence_value == NULL)
  {
    fprintf(stderr, "out of memory for user connections\n");
    exit(-1);
  }

  log_cooccurrence_value = ( double** )malloc(TOTAL_WORDS * sizeof(double *));

   if(log_cooccurrence_value == NULL)
  {
    fprintf(stderr, "out of memory for log user connections\n");
    exit(-1);
  }

  /* ------------------------------------------------------------------------- */
  // We count the distinct words and store them

    sprintf(query_string,"SELECT COUNT(DISTINCT word_a) FROM coo_matrix");

    res = mysql_perform_query(conn,query_string);

    while ((row = mysql_fetch_row(res)) !=NULL) {
      num_words=atoi(row[0]);
    }

    /* clean up the database result set */
    mysql_free_result(res);


    all_word_a_Ids = (int *)malloc(sizeof(int)*num_words);

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
    // We count the number of words that each word cooccurs with

    for (c=0; c<TOTAL_WORDS;c++) {

      word_b_Id = c+1;

      sprintf(query_string,"SELECT count(word_b) FROM coo_matrix WHERE word_a=%d",word_b_Id);

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
          fprintf(stderr, "out of memory for connections of word %d\n", word_b_Id);
          exit(-1);
        }

        cooccurrence_value[c] = ( double* )malloc(num_cooccurrences * sizeof(double));

        if(cooccurrence_value[c] == NULL)
        {
          fprintf(stderr, "out of memory for connections of word_a %d\n", word_b_Id);
          exit(-1);
        }


        log_cooccurrence_value[c] = ( double* )malloc(num_cooccurrences * sizeof(double));

        if(log_cooccurrence_value[c] == NULL)
        {
          fprintf(stderr, "out of memory for log connections of word_a %d\n", word_b_Id);
          exit(-1);
        }

        /* ------------------------------------------------------------------------- */
        // We SELECT the words that each word cooccurs with and store their ids and their cooccurrence value

        sprintf(query_string,"SELECT word_b, cooccurrence, log_cooccurrence FROM coo_matrix WHERE word_a=%d ORDER BY word_b ASC",word_b_Id);

        res = mysql_perform_query(conn,query_string);

        h=0;
        while ((row = mysql_fetch_row(res)) !=NULL) {
          cooccurrence_ids[c][h]=atoi(row[0]);
          cooccurrence_value[c][h]=atof(row[1]);
          log_cooccurrence_value[c][h]=atof(row[2]);
          h++;
        }

        /* clean up the database result set */
        mysql_free_result(res);

      } // if a word cooccurs with another word

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

  /* Uncomment if you want to stop coo_matrixing as soon as the error increases */
  // double rmse_last=9999999999, rmse = 9999999998, MIN_IMPROVEMENT=0.00001;

  i=initialize_weights(TOTAL_FEATURES);

  for (i=0; i<TOTAL_WORDS; i++) {
    for (f=0; f<TOTAL_FEATURES; f++) {
      word_features_gradients[i][f] = 1.0;
      }
    word_bias[i] = 0.0;
    word_bias_gradients[i] = 1.0;
  }


  //  while ((rmse < rmse_last - MIN_IMPROVEMENT) && g_cnt < Max_Epochs) {
  while (g_cnt < Max_Epochs) {

    g_cnt++;

    /* start timer */
    start = time(NULL);

    sq1=get_feature_gradients(TOTAL_FEATURES);
    // rmse_last = rmse;
    // rmse=sq1;

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
      fscanf(infile,"%lf", &word_features[i][f]);
    }
  }

  fclose(infile);

  /* ------------------------------------------------------------------------- */
  // Comment the following lines if you don't need to use precoo_matrixed vectors
  /* ------------------------------------------------------------------------- */

  /* ------------------------------------------------------------------------- */
  // The mapping file maps the indices of the vocabulary words to the indices of the corresponding words in the precoo_matrixed vector file for example word "the" has index "3" in the vocabulary and index "56" in the precoo_matrixed vector file. The mapping file maps 3 to 56.

  int num_lines;
  char *mapping_file="input_files/mapped_ids.txt";
  int **mapping_array;

  infile=fopen("input_files/glove.6B.100d_weights.txt","r");

  for (i=0; i<TOTAL_PRIORS; i++) {
    for (f=0; f<TOTAL_FEATURES; f++) {
      fscanf(infile,"%lf", &prior_embeddings[i][f]);
    }
  }

  fclose(infile);

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
  // Initialize the words of the vocabulary to the precoo_matrixed vectors

  for (i=0; i<num_lines; i++) {
    for (f=0; f<TOTAL_FEATURES; f++) {
      word_features[ mapping_array[i][0] -1 ][f] = prior_embeddings[ mapping_array[i][1] -1 ][f];
    }

  }

  free((void **)prior_embeddings);

  return 0;

}

double get_feature_gradients(int TOTAL_FEATURES) {

  int c,i,f;
  double p, sq1, err1, entry_weight;
  double my_lr_cb,my_lr_cf,my_lr_mf;
  double cf, mf, tmp1, tmp2;

  sq1=0.0;

  for (c=0; c < num_words; c++)  {

    word_b_Id = all_word_a_Ids[c];

    if (cooccurrence_size[word_b_Id-1]!=0) {

      for (i=0; i< cooccurrence_size[word_b_Id-1]; i++) {

        word_a_Id=cooccurrence_ids[word_b_Id-1][i];
        p = predict_coo (word_a_Id, word_b_Id, TOTAL_FEATURES);
        entry_weight = pow(MIN(1.0, ((double)cooccurrence_value[word_b_Id-1][i] / X_MAX)), ALPHA);
        err1 = - (entry_weight * ((double)log_cooccurrence_value[word_b_Id-1][i] - p));
        sq1 += -err1*((double)log_cooccurrence_value[word_b_Id-1][i] - p);

        tmp1 = fastsqrt(word_bias_gradients[word_b_Id - 1]);
        my_lr_cb = init_my_lr/tmp1;

        word_bias[word_b_Id - 1] += -my_lr_cb * err1;
        word_bias_gradients[word_b_Id - 1] += err1*err1;

        tmp1 = fastsqrt(word_bias_gradients[word_a_Id - 1]);
        my_lr_cb = init_my_lr/tmp1;
        word_bias[word_a_Id - 1] += -my_lr_cb * err1;
        word_bias_gradients[word_a_Id - 1] +=  err1*err1;

        for (f=0; f<TOTAL_FEATURES; f++) {

          cf = word_features[word_b_Id - 1][f];
          mf = word_features[word_a_Id - 1][f];

          tmp1 = fastsqrt(word_features_gradients[word_b_Id - 1][f]);
          my_lr_cf = init_my_lr/tmp1;

          tmp2 = err1 *mf;
          word_features[word_b_Id - 1][f] += -my_lr_cf * tmp2;
          word_features_gradients[word_b_Id - 1][f] += tmp2*tmp2;

          tmp1 = fastsqrt(word_features_gradients[word_a_Id - 1][f]);
          my_lr_mf = init_my_lr/tmp1;

          tmp2 = err1*cf;
          word_features[word_a_Id - 1][f] +=  -my_lr_mf * tmp2;
          word_features_gradients[word_a_Id - 1][f] += tmp2*tmp2;

        }
      }
    }
  }

  return sq1;

}

void save_new_features_files (int TOTAL_FEATURES, double sq1) {

  char my_word[100] = "";
  char filetype[600] = "";
  char filename[700];
  char folder[700] = "embeddings/";
  float my_value;
  unsigned int i, j;

  strcpy (filetype, ".txt");
  sprintf(filename, "GloVe_embeddings_lr_%g_num_epochs_%d_final_error_%lf", init_my_lr, g_cnt, sq1);
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

      my_value = word_features[i][j];

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
     sum += word_features[word_a_Id - 1][f] * word_features[word_b_Id - 1][f];
  }

  sum += word_bias[word_a_Id - 1] + word_bias[word_b_Id - 1];

  return sum;

}

float fastsqrt(float val)  {
  union
  {
    int tmp;
    float val;
  } u;
  u.val = val;
  u.tmp -= 1<<23; /* Remove last bit so 1.0 gives 1.0 */
  /* tmp is now an approximation to logbase2(val) */
  u.tmp >>= 1; /* divide by 2 */
  u.tmp += 1<<29; /* add 64 to exponent: (e+127)/2 =(e/2)+63, */
  /* that represents (e/2)-64 but we want e/2 */
  return u.val;
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
  lgfile=fopen("log_glove.txt","a");
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
