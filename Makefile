MYPSTREE_OUTPUT=myPsTree
TOPZERA_OUTPUT=topzera

MYPSTREE_SOURCES=myPsTree.cpp proc_common.cpp
TOPZERA_SOURCES=topzera.cpp proc_common.cpp
TOPZERA_CXXFLAGS=-lpthread -lncurses

all: myPsTree topzera

myPsTree: $(MYPSTREE_SOURCES)
	g++ $(MYPSTREE_SOURCES) -o $(MYPSTREE_OUTPUT)

topzera: $(TOPZERA_SOURCES)
	g++ $(TOPZERA_SOURCES) -o $(TOPZERA_OUTPUT) $(TOPZERA_CXXFLAGS)

clean:

	if [ -f $(MYPSTREE_OUTPUT) ]; then \
	  rm $(MYPSTREE_OUTPUT); \
	fi

	if [ -f $(TOPZERA_OUTPUT) ]; then \
	  rm $(TOPZERA_OUTPUT); \
	fi
