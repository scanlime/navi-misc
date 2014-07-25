CLASSES =\
	logger.class\
	tcppipe.class\
	tcpfw.class\
	udpfw.class\
	run.class

prog: $(CLASSES)

logger.class: logger.java
	javac logger.java

tcppipe.class: tcppipe.java
	javac tcppipe.java

udpfw.class: udpfw.java
	javac udpfw.java

tcpfw.class: tcpfw.java
	javac tcpfw.java

run.class: run.java
	javac run.java

run: prog
	java run

jar: prog
	jar cvmf .manifest starjox.jar *.class

clean:
	rm -Rf *.class *.jar *~
