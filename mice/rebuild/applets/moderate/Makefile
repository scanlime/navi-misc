CLASSES =\
	net.class\
	keyinfo.class\
	key.class\
	moderate.class\
	authenticate.class\
	build.class\
	accepted.class\
	quit.class\
	get.class\
	reject.class

app: $(CLASSES)

moderate.class: moderate.java
	javac moderate.java

quit.class: quit.java
	javac quit.java

get.class: get.java
	javac get.java

reject.class: reject.java
	javac reject.java

accepted.class: accepted.java
	javac accepted.java

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

jar: app
	jar cvf moderate.jar *.class

run: app
	open index.html

clean:
	rm -Rf *.class *.jar *~
