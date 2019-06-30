PHONY += clean
clean:
	test ! -d '$(O_OBJ)' || find '$(O_OBJ)' -type f -name '*.o' -delete
	test ! -d '$(O_OBJ)' || find '$(O_OBJ)' -depth -type d -empty -delete
	test ! -f '$(O)/ip-dedup' || rm -- '$(O)/ip-dedup'
