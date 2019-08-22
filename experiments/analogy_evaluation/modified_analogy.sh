function larger_mv ()
{       while read line1;
do

fname="$line1"
fbname=$(basename "$fname" .txt)
echo "Doing... $fbname" | tee -a modified_analogy_report

cat $fname | awk '{for (i=2;i<=NF;i++) printf("%s ",$i); printf("\n");}' > "new_"$fbname".txt"

python ../../glove-python/examples/import_txt2glove.py "new_"$fbname".txt" > /dev/null 2>&1

python ../../glove-python/examples/modified_analogy_tasks_evaluation.py --test ../../glove-python/examples/questions-words.txt --model imported_new_$fbname.model --parallelism 10 | tee -a modified_analogy_report

echo | tee -a modified_analogy_report 2>&1
rm new_$fbname.txt imported_new_$fbname.model

done
}

ls -1 ../../embeddings/*.txt | larger_mv
