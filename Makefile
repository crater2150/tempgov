PROGNAME	= tempgov
PREFIX		= /usr/local
CFLAGS          = -std=c99 -D_POSIX_C_SOURCE=200809L

SRCDIR        = src
OBJDIR        = build

SRCFILES=$(wildcard $(SRCDIR)/*.c)
HEADERS=$(wildcard $(SRCDIR)/*.h)
OBJFILES=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCFILES))

VPATH = src

.PHONY : all clean install tests

$(OBJDIR)/%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

all: $(PROGNAME)

$(PROGNAME): $(OBJFILES)
	$(CC) $(LDFLAGS) $(OBJFILES) -o $@

clean:
	rm $(OBJFILES) $(PROGNAME)

install: $(PROGNAME)
	install -d $(PREFIX)/bin
	install $(PROGNAME) $(PREFIX)/bin

tests:
	cd tests && make
