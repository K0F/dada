#!/bin/bash
start_time=$(date +%s)
end_time=$((start_time + 300))
offset=0
total_size=$(stat -c%s noise.png)

echo "Commencing 5 minutes of noise extraction..."

while [ $(date +%s) -lt $end_time ]; do
    # Wrap offset if needed
    if [ $offset -ge $((total_size - 2)) ]; then
        offset=0
    fi
    
    # Source ALL randomness from noise.png
    val1=$(od -j $offset -N 1 -An -t u1 noise.png | tr -d ' ')
    val2=$(od -j $((offset+1)) -N 1 -An -t u1 noise.png | tr -d ' ')
    
    # Calculate sleep (1-3 seconds) and chunk size based on noise bytes
    sleep_time=$(( (val1 % 3) + 1 ))
    chunk_size=$(( val2 + 10 ))
    
    offset=$((offset + 2))
    
    if [ $offset -ge $((total_size - chunk_size)) ]; then
        offset=0
    fi
    
    # Extract noise and append as hex text
    od -j $offset -N $chunk_size -An -t x1 noise.png >> bullshit.txt
    
    offset=$((offset + chunk_size))
    
    sleep $sleep_time
done

git add bullshit.txt
git commit -m "Dadaist intervention: 5 minutes of concentrated noise"
git push github main

# Beep!
echo -e "\a"
echo "Bullshit published and beeped."
