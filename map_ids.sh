# Run with the following command:
#./map_ids.sh random_vectors.txt pretrained_vectors.txt

#This produces the map_ids.txt which maps the words' indices of random_vectors.txt file to the words' indices of pretrained_vectors.txt file.
awk '{print NR-1, $1}' $1 | sort -k2 > file1
awk '{print NR-1, $1}' $2 | sort -k2 > file2
join -j 2 -o 1.1,2.1 file1 file2 > mapped_ids.txt

rm file1 file2

#This produces the file which contains only the weights of the pretrained file
fbname=$(basename "$2" .txt)
cat $2 | awk '{for (i=2;i<=NF;i++) printf("%s ",$i); printf("\n");}' > $fbname"_weights.txt"

cp $fbname"_weights.txt" input_files
cp mapped_ids.txt input_files
