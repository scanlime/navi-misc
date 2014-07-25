CLASSES =\
	net.class\
	keyinfo.class\
	key.class\
	accepted.class\
	quit.class\
	get.class\
	reject.class\
	smod.class

app: $(CLASSES)

smod.class: smod.java
	javac smod.java

quit.class: quit.java
	javac quit.java

get.class: get.java
	javac get.java

reject.class: reject.java
	javac reject.java

accepted.class: accepted.java
	javac accepted.java

keyinfo.class: keyinfo.java
	javac keyinfo.java

key.class: key.java
	javac key.java

net.class: net.java
	javac net.java

jar: app
	jar cvf smod.jar *.class

run: app
	open index.html

clean:
	rm -Rf *.class *.jar *~
