function run_me ()
{       while read line1; 
do

fname="$line1"
fbname=$(basename "$fname" .txt)

DATASETS=("data/men.csv" "data/ws-353.csv" "data/rg-65.csv")

for d in "${DATASETS[@]}"

do

d_base=$(basename "$d" .csv)

stdbuf -oL python similarity_evaluation.py $fname $d | tee -a results/similarity_results_$d_base.csv

done

done
}


ls -1 ../../embeddings/* | run_me
