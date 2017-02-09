.PHONY : install
install :
	@ ./compile.sh --upload

.PHONY : check
check :
	@ ./compile.sh --check

.PHONY : preprocess
preprocess :
	@ ./compile.sh --preprocess

.PHONY : clean
clean :
	@ rm -rf "out";

.PHONY : test
test : check
