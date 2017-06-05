OUTPUT=myPsTree
SOURCES=*.cpp

xeu: $(SOURCES)
	g++ $(SOURCES) $(CXXFLAGS) -o $(OUTPUT)

clean:
	
	if [ -f $(OUTPUT) ]; then \
	  rm $(OUTPUT); \
	fi


