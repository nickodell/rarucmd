DESTDIR=/usr/local/bin/

all:
	cc raru.c -o raru
clean:
	rm raru
lint:
	lint raru.c
install: all
	@echo WARNING: This attempts to install raru as setuid!
	install -m 4755 -o 0 -g 0 raru $(DESTDIR)
	install -m 755 -o 0 -g 0 xraru $(DESTDIR)
