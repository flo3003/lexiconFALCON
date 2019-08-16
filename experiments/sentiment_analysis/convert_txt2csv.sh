grep Doing $1 | awk '{print $2}' > ppp

grep -e Doing -e Accuracy $1 > ttt
awk '/Doing/ { show[NR+1] = 1 } (NR in show) { print $0 ; delete show[NR] }' ttt | awk '{print $2}' > a_r_f

grep -e Doing -e "F1-score" $1 > sss
awk '/Doing/ { show[NR+1] = 1 } (NR in show) { print $0 ; delete show[NR] }' sss | awk '{print $2}' > f1_r_f

fbname=$(basename "$1" .txt)
echo "Filename, Accuracy, F1-score" > $fbname.csv

paste -d "," ppp a_r_f f1_r_f >> $fbname.csv

rm ppp ttt sss a_r_f f1_r_f 
