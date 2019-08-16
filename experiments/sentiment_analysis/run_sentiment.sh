function run_me ()
{       while read line1; 
do

fname="$line1"
fbname=$(basename "$fname" .txt)
echo "Doing... $fbname"  | tee -a results_sentiment.txt 2>&1

stdbuf -oL python sentiment_analysis.py $fname | tee -a results_sentiment.txt 

echo | tee -a results_sentiment.txt 2>&1

done
}

ls -1 ../../embeddings/* | run_me
