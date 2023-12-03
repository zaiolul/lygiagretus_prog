#!/bin/sh
MIN_PROC=1
MAX_PROC=12
AVG_COUNT=5
OUT_FILE="results.txt"

rm "$(pwd)/$OUT_FILE"
data=$(ls ./data | grep "\.json")

for file in $data; do
    echo "$file:" >> $OUT_FILE
    echo "pradėtas $file"
    for p in $(seq $MIN_PROC $MAX_PROC); do
        sum=0.00
        for i in $(seq 1 $AVG_COUNT); do
            result=$( { /usr/bin/time --format "%e" ./lp_proj.py -i $file -p $p; } 2>&1 )
            sum=$(echo "$sum + $result" | bc -l)
            
        done
        echo $sum
        result=$(echo "scale=3; $sum / $AVG_COUNT" | bc -l)

        [ "$p" -eq 1 ] && echo "Laikas su 1 procesu: $result"
        echo -e "\t$p:\t$result" >> $OUT_FILE
    done
    echo "baigtas $file"
done
