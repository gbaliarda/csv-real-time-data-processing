COMPILER=gcc
OUTPUT_FILEBUE=arbolesADTBUE
OUTPUT_FILEVAN=arbolesADTVAN
QUERIES=queries
CFLAGS=-std=c99 -pedantic -Wall -fsanitize=address
FILES=./front/main.c ./arbolesADT/arbolesADT.c ./front/csvParser.c
all: arbolesADTBUE arbolesADTVAN

arbolesADTBUE: $(FILESBUE)
	$(COMPILER) $(CFLAGS) -o $(OUTPUT_FILEBUE) $(FILES) -D BUE

arbolesADTVAN: $(FILESVAN)
	$(COMPILER) $(CFLAGS) -o $(OUTPUT_FILEVAN) $(FILES) -D VAN

clean:
	rm -r $(OUTPUT_FILEBUE)
	rm -r $(OUTPUT_FILEVAN)
	rm -r $(QUERIES)/*
