SRC=\
	net.class\
	submit.class\
	reject.class\
	quit.class\
	interview.class

program: $(SRC)

net.class: net.java
	javac net.java

submit.class: submit.java
	javac submit.java

reject.class: reject.java
	javac reject.java

quit.class: quit.java
	javac quit.java

interview.class: interview.java
	javac interview.java

run: program
	open index.html

clean:
	rm -Rf *.class *.jar *~
