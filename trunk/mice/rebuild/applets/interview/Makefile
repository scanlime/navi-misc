SRC=\
	net.class\
	interview.class

program: $(SRC)

net.class: net.java
	javac net.java

interview.class: interview.java
	javac interview.java

run: program
	open index.html

clean:
	rm -Rf *.class *.jar *~
