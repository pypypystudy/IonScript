FOLDERS = library example

all:
	@for d in $(FOLDERS);\
		do\
			$(MAKE) --directory=$$d all;\
		done

debug:
	@for d in $(FOLDERS);\
		do\
			$(MAKE) --directory=$$d debug;\
		done
		
release:
	@for d in $(FOLDERS);\
		do\
			$(MAKE) --directory=$$d release;\
		done

headers:
	@for d in $(FOLDERS);\
		do\
			$(MAKE) --directory=$$d headers;\
		done
clean:
	@for d in $(FOLDERS);\
		do\
			$(MAKE) --directory=$$d clean;\
		done