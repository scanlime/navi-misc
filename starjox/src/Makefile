CLASSES =\
	tcppipe.class\
	tcpfw.class\
	udpfw.class\
	run.class

prog: $(CLASSES)

tcppipe.class: tcppipe.java
	javac tcppipe.java

udpfw.class: udbfw.java
	javac udpfw.java

tcpfw.class: tcpfw.java
	javac tcpfw.java

run.class: run.java
	javac run.java

run: prog
	java tcpfw localhost 6112

jar: prog
	jar cvmf .manifest starjox.jar *.class

clean:
	rm -Rf *.class *.jar *~
