SRC=\
	question.class

app: $(SRC)

question.class: question.java
	javac question.java

clean:
	rm -Rf *.jar *.class *~

run: app
	open index.html
