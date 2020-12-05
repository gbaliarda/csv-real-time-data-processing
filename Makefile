COMPILER=gcc
OUTPUT_FILEBUE=arbolesADTBUE
OUTPUT_FILEVAN=arbolesADTVAN
CFLAGS=-std=c99 -pedantic -Wall -fsanitize=address
FILES=main.c arbolesADT.c csvParser.c
all: arbolesADTBUE arbolesADTVAN

arbolesADTBUE: $(FILESBUE)
	$(COMPILER) $(CFLAGS) -o $(OUTPUT_FILEBUE) $(FILES) -D BUE

arbolesADTVAN: $(FILESVAN)
	$(COMPILER) $(CFLAGS) -o $(OUTPUT_FILEVAN) $(FILES) -D VAN

clean:
	rm -r $(OUTPUT_FILEBUE)
	rm -r $(OUTPUT_FILEVAN)
