SRC=\
	net.class\
	key.class\
	keyinfo.class\
	get.class\
	submit.class\
	reject.class\
	quit.class\
	interview.class

program: $(SRC)

net.class: net.java
	javac net.java

key.class: key.java
	javac key.java

keyinfo.class: keyinfo.java
	javac keyinfo.java

get.class: get.java
	javac get.java

submit.class: submit.java
	javac submit.java

reject.class: reject.java
	javac reject.java

quit.class: quit.java
	javac quit.java

interview.class: interview.java
	javac interview.java

jar: program
	jar cvf interview.jar *.class

run: program jar
	open index.html

clean:
	rm -Rf *.class *.jar *~
