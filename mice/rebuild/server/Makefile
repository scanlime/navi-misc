CLASSFILES=\
	question.class\
	qqueue.class\
	qbase.class\
	keyinfo.class\
	key.class\
	ainterview.class\
	amoderate.class\
	asubmit.class\
	aupdatei.class\
	aupdate.class\
	nbase.class\
	njavasub.class\
	nperlsub.class\
	nchatsub.class\
	njavamod.class\
	njavaint.class\
	nchatint.class\
	nchatup.class\
	imain.class\
	njavaadmin.class

program: $(CLASSFILES)

pbase.class: pbase.java
	javac pbase.java

nbase.class: nbase.java
	javac nbase.java

njavasub.class: njavasub.java
	javac njavasub.java

nperlsub.class: nperlsub.java
	javac nperlsub.java

nchatsub.class:	nchatsub.java
	javac nchatsub.java

njavamod.class: njavamod.java
	javac njavamod.java

njavaint.class: njavaint.java
	javac njavaint.java

nchatint.class: nchatint.java
	javac nchatint.java

nchatup.class: nchatup.java
	javac nchatup.java

njavaadmin.class: njavaadmin.java
	javac njavaadmin.java

keyinfo.class: keyinfo.java
	javac keyinfo.java

key.class: key.java
	javac key.java

imain.class: imain.java
	javac imain.java

ainterview.class: ainterview.java
	javac ainterview.java

amoderate.class: amoderate.java
	javac amoderate.java

asubmit.class: asubmit.java
	javac asubmit.java

aupdate.class: aupdate.java
	javac aupdate.java

aupdatei.class: aupdatei.java
	javac aupdatei.java

question.class: question.java
	javac question.java

qqueue.class: qqueue.java
	javac qqueue.java

qbase.class: qbase.java
	javac qbase.java

clean:
	rm -Rf *.class *~ *.jar

server.jar: 
	jar cmf .manifest server.jar *.class

jar: $(CLASSFILES) server.jar
	rm server.jar
	jar cmf .manifest server.jar *.class

run: $(CLASSFILES) server.jar
	java imain
#	java -jar server.jar
