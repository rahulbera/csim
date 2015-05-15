CC=g++
CORE = Helper.o BaseCache.o LRUCache.o LIPCache.o BIPCache.o DIPCache.o NRUCache.o SRRIPCache.o DRRIPCache.o VRRIPCache.o OptCache.o
MAIN = main.cpp
PREPROCESS = preprocess.cpp
OBJDIR = obj/

all: preprocessor csim
	
csim: dir $(MAIN) $(CORE)	
	@echo "Linking $@..."
	@$(CC) -o $@ $(MAIN) $(CORE)
	@mv *.o $(OBJDIR)

dir:
	@mkdir -p $(OBJDIR)

%.o: %.cpp 
	@echo "Making $@..."
	@$(CC) -c -o $@ $<

preprocessor:
	@echo "Making $@..."
	@$(CC) -o $@ $(PREPROCESS)

clean:
	@echo "Cleaning..."
	@rm -f $(OBJDIR)*o csim preprocessor