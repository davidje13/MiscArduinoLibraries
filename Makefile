.PHONY : install
install : SSD1306

.PHONY : SSD1306
SSD1306 :
	@ ./compile.sh --upload SSD1306

.PHONY : HMC5883L
HMC5883L :
	@ ./compile.sh --upload HMC5883L

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
