CLASSES =\
	net.class\
	keyinfo.class\
	key.class\
	build.class\
	moderate.class\
	authenticate.class\
	accept.class

app: $(CLASSES)

moderate.class: moderate.java
	javac moderate.java

accept.class: accept.java
	javac accept.java

build.class: build.java
	javac build.java

keyinfo.class: keyinfo.java
	javac keyinfo.java

key.class: key.java
	javac key.java

net.class: net.java
	javac net.java

authenticate.class: authenticate.java
	javac authenticate.java
run: app
	open index.html

clean:
	rm -Rf *.class *.jar *~
