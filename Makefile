REFPKGS  =
SRCPKGS  = layout util
LIBPKGS  = $(REFPKGS) $(SRCPKGS)
MAIN     = main

LIBS     = $(addprefix -l, $(LIBPKGS))
SRCLIBS  = $(addsuffix .a, $(addprefix lib, $(SRCPKGS)))

DIRS     = include lib bin

EXEC     = TPLSolver

# for gnuplot
PLOTDIR  = visualization

BENCHPREFIX = c1 c2 c3 c4 c5 c6 c7 c8 c9 c10 s1 s2 s3 s4 s5
BENCHFILE   = $(addprefix $(PLOTDIR)/, $(addsuffix sim_merged.txt, $(BENCHPREFIX))) $(wildcard $(PLOTDIR)/*.html)

.PHONY: directory

all: directory libs main

libs:
	@for pkg in $(SRCPKGS); \
	do \
		echo "Checking $$pkg..."; \
		cd src/$$pkg; make -f make.$$pkg --no-print-directory PKGNAME=$$pkg; \
		cd ../..; \
	done

main:
	@echo "Checking $(MAIN)..."
	@cd src/$(MAIN); \
		make -f make.$(MAIN) --no-print-directory INCLIB="$(LIBS)" EXEC=$(EXEC);
	@ln -fs bin/$(EXEC) .
#	@strip bin/$(EXEC)

directory:
	@for dir in $(DIRS); \
	do \
		printf "Checking $$dir..."; \
		if [ ! -d $$dir ]; then \
			mkdir $$dir; \
			echo "Creating directory \"$$dir\"..."; \
		else \
			echo "Directory $$dir exists."; \
		fi; \
	done

.PHONY: clean

clean:
	@for pkg in $(SRCPKGS); \
	do \
		echo "Cleaning $$pkg..."; \
		cd src/$$pkg; make -f make.$$pkg --no-print-directory PKGNAME=$$pkg clean; \
                cd ../..; \
	done
	@echo "Cleaning $(MAIN)..."
	@cd src/$(MAIN); make -f make.$(MAIN) --no-print-directory clean
	@echo "Removing $(SRCLIBS)..."
	@cd lib; rm -f $(SRCLIBS)
	@echo "Removing $(EXEC)..."
	@rm -f bin/$(EXEC)

cleanall: clean
	@echo "Removing bin/*..."
	@rm -rf bin/*

ctags:	  
	@rm -f src/tags
	@for pkg in $(SRCPKGS); \
	do \
		echo "Tagging $$pkg..."; \
		cd src/$$pkg; ctags -R *.cpp *.h; cd ../..; \
	done
	@echo "Tagging $(MAIN)..."
	@cd src/$(MAIN); ctags -R *.cpp

# for gnuplot

.PHONY: plot

plot:
	@if [ ! -d ${PLOTDIR} ]; then \
		echo "Creating ${PLOTDIR}..."; \
		mkdir ${PLOTDIR}; \
	else \
		echo "Directory ${PLOTDIR} exists."; \
	fi
	@cd ${PLOTDIR}; ./create_txt.sh; gnuplot plot.gp

