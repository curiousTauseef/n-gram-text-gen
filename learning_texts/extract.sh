#!/bin/bash
cat bibliya_utf.txt | sed 's|[,.]||g' | tr ' ' '\n' | sort | uniq -c
cat bibliya_utf.txt | sed 's|[,.]||g' | tr ' ' '\n' | sort | uniq -c > out.txt
cat bibliya_utf.txt | sed 's|[,.]| |g' | tr ' ' '\n' | sort | uniq -c > out.txt
gawk -v RS='[^[:alpha:]]+' '{sum[$0]++} END{for (word in sum) print word,sum[word]}' bibliya_utf.txt
gawk -v RS='[^[:alpha:]]+' '{sum[$0]++} END{for (word in sum) print word,sum[word]}' bibliya_utf.txt > out.txt
sort out.txt 
sort out.txt > outs.txt
# convert the text to lower case
awk '{print tolower($0)}' < input.txt