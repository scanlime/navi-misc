CLASSES=\
	info.class\
	net.class\
	authenticate.class\
	interview.class\
	moderator.class\
	admin.class

app: $(CLASSES)

info.class: info.java
	javac info.java

net.class: net.java
	javac net.java

authenticate.class: authenticate.java
	javac authenticate.java

interview.class: interview.java
	javac interview.java

moderator.class: moderator.java
	javac moderator.java

admin.class: admin.java
	javac admin.java

clean:
	rm -Rf *.jar *.class *~

run: app
	open index.html
