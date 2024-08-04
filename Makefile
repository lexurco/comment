include version.mk

BIN = comment
OBJ = $(BIN:=.o)
SRC = $(BIN:=.c)
MAN = $(BIN:=.1)

PREFIX ?= $(DESTDIR)/usr/local
MANPREFIX ?= $(PREFIX)/man

bindir = $(PREFIX)/bin
man1dir = $(MANPREFIX)/man1

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ)

.c.o:
	$(CC) -std=c99 -pedantic $(CFLAGS) $(CPPFLAGS) -c $<

install: all
	mkdir -p $(bindir)
	install -m 755 $(BIN) $(bindir)
	mkdir -p $(man1dir)
	install -m 644 $(MAN) $(man1dir)

uninstall:
	cd $(bindir) && rm -f $(BIN)
	cd $(man1dir) && rm -f $(MAN)

clean:
	-rm -rf $(BIN) $(OBJ) comment$(V) *.tar.gz *.core

dist: clean
	mkdir comment$(V)
	cp $(SRC) $(MAN) README Makefile version.mk comment$(V)
	tar cf - comment$(V) | gzip >comment$(V).tar.gz
	rm -rf comment$(V)

tags: $(SRC)
	ctags $(SRC)

.PHONY: all install uninstall clean dist
