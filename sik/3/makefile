JAVAPATH = /usr/local/lib/j2sdk1.4.0/bin/
ORBPORT = 1050
ORBHOST = localhost
ORBFLAGS = -ORBInitialPort $(ORBPORT) -ORBInitialHost $(ORBHOST)

IDLC = /usr/local/lib/j2sdk1.4.0/bin/idlj
IDLCFLAGS = -fallTIE
JAVAC = /usr/local/lib/j2sdk1.4.0/bin/javac
JAVACFLAGS = -deprecation


ALL = idl OXClient.class OXServer.class

all: $(ALL)

idl: OX.idl
	$(IDLC) $(IDLCFLAGS) OX.idl

OXServer.class: idl OXServer.java
	$(JAVAC) $(JAVACFLAGS) OXServer.java

OXClient.class: idl OXClient.java
	$(JAVAC) $(JAVACFLAGS) OXClient.java

start_orbd:
	$(JAVAPATH)orbd $(ORBFLAGS)&

start_server:
	$(JAVAPATH)java OXServer $(ORBFLAGS)&

start_client:
	$(JAVAPATH)java OXClient $(ORBFLAGS)

clean:
	rm -rf OX *.class 