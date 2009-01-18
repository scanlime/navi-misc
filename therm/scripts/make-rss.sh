cat <<EOF
<?xml version="1.0" encoding="utf-8"?>
<rss version="2.0" xmlns:media="http://search.yahoo.com/mrss/">
<channel>
<title>Temperature Data</title>
EOF

for rep1 in 0 1 2 3 4 5 6 7 8 9 a b c d e f; do
   for rep2 in 0 1 2 3 4; do
      for scale in 1d 1w 1m; do
#     for scale in 1h 1d 1w 1m 1y; do

cat <<EOF
<item><media:content url="http://192.168.1.77/therm/temp-$scale.jpeg?$rep1$rep2" type="image/jpeg" /></item>
EOF

      done
   done
done

cat <<EOF
</channel>
</rss>
EOF

