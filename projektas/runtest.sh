#!/bin/sh
MIN_PROC=1
MAX_PROC=12
AVG_COUNT=1
OUT_FILE="results.txt"
OUT_ONE="res-one.txt"
export IFS=$' \t\r\n'  

if [ "$#" -eq 3 ]; then
    rm "$(pwd)/$OUT_ONE"
    echo "$1:" >> "$OUT_ONE"
    echo "pradėtas $1"
    for p in $(seq $2 $3); do
        sum=0.00
        for i in $(seq 1 $AVG_COUNT); do
            result=$( { /usr/bin/time --format "%e" ./lp_proj.py -i $1 -p $p; } 2>&1 ) 
            sum=$(echo "$sum + $result" | bc -l)
        done
        result=$(echo "scale=3; $sum / $AVG_COUNT" | bc -l)
        echo "$p proc. vidutinis laikas $result"
        echo -e "\t$p\t$result" >> "$OUT_ONE"
    done
    echo "baigtas $1"
    exit 1
fi


rm "$(pwd)/$OUT_FILE"
data=$(ls ./data | grep "\.json")

for file in $data; do
    echo "$file:" >> $OUT_FILE
    echo "pradėtas $file"
    for p in $(seq $MIN_PROC $MAX_PROC); do
        sum=0.00
        for i in $(seq 1 $AVG_COUNT); do
            result=$( { /usr/bin/time --format "%e" ./lp_proj.py -i data/$file -p $p; } 2>&1 )
            sum=$(echo "$sum + $result" | bc -l)
        done
        result=$(echo "scale=3; $sum / $AVG_COUNT" | bc -l)
        echo "$p proc. vidutinis laikas $result"
        echo -e "\t$p:\t$result" >> $OUT_FILE
    done
    echo "baigtas $file"
done
