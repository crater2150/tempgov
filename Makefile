
PROGNAME	= tempgov
PREFIX		= /usr/local
GARBAGE		=
CFLAGS      = 
LDFLAGS		= 

SRCDIR      = src
OBJDIR      = build

.PHONY : all clean

$(OBJDIR)%.o:$(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

all: main

main: $(OBJDIR)/main.o
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm $(GARBAGE)

install: $(PROGNAME)
	mkdir -vp $(PREFIX)/bin
	cp -v $(PROGNAME) $(PREFIX)/bin

