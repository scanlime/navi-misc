CLASSES =\
	connection.class\
	smain.class

prog: $(CLASSES)

connection.class: connection.java
	javac connection.java

smain.class: smain.java
	javac smain.java

run: prog
	java smain localhost 6112
