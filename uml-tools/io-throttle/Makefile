io-throttle: io-throttle.c
	$(CC) -o $@ $< -O3 -Wall

install: io-throttle
	install -s $< /usr/local/bin/$<

clean:
	rm -f io-throttle
