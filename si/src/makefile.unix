# katalog docelowy
OUTPUTDIR = release
# katalog na dokumentacjê (w katalogu docelowym)
DOCDIR = doc
# separator ¶cie¿ki
PS = /

# kompilator javy
JC = javac
JCFLAGS = -d $(OUTPUTDIR) -deprecation
JCFINALFLAGS = -O -g:none

# kompilator dokumentacji
JDOC = javadoc
JDOCFLAGS = -d $(OUTPUTDIR)$(PS)$(DOCDIR)

# polecenia do kasowania plików i katalogów
RM = rm
RMFLAGS = -r -f

# polecenia do kopiowania plików i katalogów
CP = cp
CPFLAGS = -f

# polecenie do tworzenia katalogu
MD = md
MDFLAGS = 


MAIN_TARGETS = 		*.java

ENGINE_TARGETS =	engine/*.java \
			engine/agents/*.java \
			engine/event/*.java \
			engine/parser/*.java \
			engine/theory/*.java

INTERFACE_TARGETS = 	ui/*.java



ALL_TARGETS = $(MAIN_TARGETS) $(ENGINE_TARGETS) $(INTERFACE_TARGETS)

PROPERTIES_TARGETS = HardCAKE.pr Parser.pr


CLEAN_TARGETS = $(OUTPUTDIR)$(PS)HardCAKE.class \
		$(OUTPUTDIR)$(PS)HardCAKE.properties \
		$(OUTPUTDIR)$(PS)Parser.properties \
		$(OUTPUTDIR)$(PS)engine \
		$(OUTPUTDIR)$(PS)ui \
		$(OUTPUTDIR)$(PS)$(DOCDIR)

ALL = engine interface main properties
ALL_FINAL = engine_final interface_final main_final properties doc


all: 	$(ALL)

main:	$(MAIN_TARGETS) $(OUTPUTDIR)
	$(JC) $(JCFLAGS) $(MAIN_TARGETS)

engine: $(ENGINE_TARGETS) $(OUTPUTDIR)
	$(JC) $(JCFLAGS) $(ENGINE_TARGETS)

interface: $(INTERFACE_TARGETS) $(OUTPUTDIR)
	$(JC) $(JCFLAGS) $(INTERFACE_TARGETS)

final:	$(CLEAN) $(ALL_FINAL) 

main_final: $(MAIN_TARGETS) $(OUTPUTDIR)
	$(JC) $(JCFLAGS) $(JCFINALFLAGS) $(MAIN_TARGETS)

engine_final: $(ENGINE_TARGETS) $(OUTPUTDIR)
	$(JC) $(JCFLAGS) $(JCFINALFLAGS) $(ENGINE_TARGETS)

interface_final: $(INTERFACE_TARGETS) $(OUTPUTDIR)
	$(JC) $(JCFLAGS) $(JCFINALFLAGS) $(INTERFACE_TARGETS)

properties: $(PROPERTIES_TARGETS) $(OUTPUTDIR)
	$(CP) $(CPFLAGS) HardCAKE.pr $(OUTPUTDIR)$(PS)HardCAKE.properties
	$(CP) $(CPFLAGS) Parser.pr $(OUTPUTDIR)$(PS)Parser.properties

$(OUTPUTDIR):
	$(MD) $(MDFLAGS) $(OUTPUTDIR)

doc: 	$(ALL_TARGETS) $(OUTPUTDIR)
	$(JDOC) $(JDOCFLAGS) $(ALL_TARGETS)

clean:  
	$(RM) $(RMFLAGS) $(CLEAN_TARGETS)

