OUTPUT=myPsTree
SOURCES=*.cpp

myPsTree: $(SOURCES)
	g++ $(SOURCES) $(CXXFLAGS) -o $(OUTPUT)

clean:

	if [ -f $(OUTPUT) ]; then \
	  rm $(OUTPUT); \
	fi
