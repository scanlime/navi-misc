CLASSES=\
	key.class\
	dpipe.class\
	epipe.class\
	server.class
	
prog: $(CLASSES)

dpipe.class: dpipe.java
	javac dpipe.java

epipe.class: epipe.java
	javac epipe.java

server.class: server.java
	javac server.java

key.class: key.java
	javac key.java

clean:
	rm -Rf *.jar *.class *~ #*
