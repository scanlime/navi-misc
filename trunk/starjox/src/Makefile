CLASSES =\
	pipe.class\
	smain.class

prog: $(CLASSES)

pipe.class: pipe.java
	javac pipe.java

smain.class: smain.java
	javac smain.java

run: prog
	java smain localhost 6112

jar: prog
	jar cvmf .manifest starjox.jar *.class
