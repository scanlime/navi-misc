CLASSES=\
	net.class\
	person.class\
	personindex.class\
	interview.class\
	botmain.class\
	aimup.class

bot: $(CLASSES)

net.class: net.java
	javac net.java

person.class: person.java
	javac person.java

personindex.class: personindex.java
	javac personindex.java

interview.class: interview.java
	javac interview.java

botmain.class: botmain.java
	javac botmain.java

aimup.class: aimup.java
	javac aimup.java

clean:
	rm -Rf *.class *~

run: bot
	java aimup
