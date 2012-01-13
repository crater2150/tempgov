PROGNAME	= tempgov
PREFIX		= /usr/local
CFLAGS          = -std=c99

SRCDIR        = src
BUILDDIR      = build

VPATH = src

.PHONY : all clean install

$(BUILDDIR)/%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

all: $(PROGNAME)

$(PROGNAME): $(BUILDDIR)/tempgov.o 
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm $(GARBAGE)

install: $(PROGNAME)
	install -d $(PREFIX)/bin
	install $(PROGNAME) $(PREFIX)/bin

