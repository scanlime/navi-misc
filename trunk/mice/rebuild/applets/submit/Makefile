CLASSES=\
	question.class

app: $(CLASSES)

question.class: question.java
	javac question.java

clean:
	rm -Rf *.jar *.class *~

jar: app
	jar cvf question.jar *.class

run: app
	open index.html
