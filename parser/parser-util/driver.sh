#!/bin/bash

root="http://export.arxiv.org/api/query"
dest="$1"
argc="$#"
for cat_no in $(seq 2 $argc); do
	cat_name="${!cat_no}"
	query="${root}?search_query=cat:\"${cat_name}\""
	curl -s ${query} > "$dest"			
done
# curl -s "http://export.arxiv.org/api/query?search_query=all:electron&start=0&max_results=10"
