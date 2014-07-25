#!/bin/sh

# convert .dot files to .png
for file in *.dot; do
	echo "Converting $file..."
	dot -Tpng -o "`echo $file |cut -d'.' -f1`.png" $file
	dot -Tps -o "`echo $file |cut -d'.' -f1`.ps" $file
done
