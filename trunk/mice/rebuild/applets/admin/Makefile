CLASSES=\
	admin.class

app: $(CLASSES)

admin.class: admin.java
	javac admin.java

clean:
	rm -Rf *.jar *.class *~

run: app
	open index.html
