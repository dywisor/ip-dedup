PHONY += install
install:
	$(DOEXE) -- $(O)/ip-dedup $(DESTDIR)$(BINDIR)/ip-dedup
