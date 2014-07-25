CLASSES=\
	sshPacket.class\
	TunnelSingleConnection.class\
	TunnelList.class\
	TunnelMain.class\

prog: $(CLASSES)
	
sshPacket.class: sshPacket.java
	javac sshPacket.java

TunnelSingleConnection.class: TunnelSingleConnection.java
	javac TunnelSingleConnection.java

TunnelList.class: TunnelList.java
	javac TunnelList.java

TunnelMain.class: TunnelMain.java
	javac TunnelMain.java

run: prog
	java TunnelMain
