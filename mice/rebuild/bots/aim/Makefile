CLASSES=\
	keyinfo.class\
	key.class\
	net.class\
	person.class\
	personindex.class\
	interview.class\
	sleeper.class\
	botmain.class\
	aimup.class

bot: $(CLASSES)

keyinfo.class: keyinfo.java
	javac keyinfo.java

key.class: key.java
	javac key.java

net.class: net.java
	javac net.java

person.class: person.java
	javac person.java

personindex.class: personindex.java
	javac personindex.java

interview.class: interview.java
	javac interview.java

sleeper.class: sleeper.java
	javac sleeper.java

botmain.class: botmain.java
	javac botmain.java

aimup.class: aimup.java
	javac aimup.java

clean:
	rm -Rf *.class *~ *.jar

jar: bot
	jar cvmf .manifest aim.jar *.class jaim/*.class

run: bot
	java aimup
