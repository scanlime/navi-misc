CLASSES=\
	element.class\
	reader.class\
	updater.class

update: $(CLASSES)

element.class: element.java
	javac element.java

reader.class: reader.java
	javac reader.java

updater.class: updater.java
	javac updater.java

jar: update
	jar cmf .manifest web.jar *.class

clean:
	rm -Rf *.class *~
