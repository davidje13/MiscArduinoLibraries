.PHONY : install
install : SSD1306

.PHONY : SSD1306
SSD1306 :
	@ ./compile.sh --upload SSD1306

.PHONY : HMC5883L
HMC5883L :
	@ ./compile.sh --upload HMC5883L

.PHONY : ITG3200
ITG3200 :
	@ ./compile.sh --upload ITG3200

.PHONY : ADXL345
ADXL345 :
	@ ./compile.sh --upload ADXL345

.PHONY : check
check :
	@ ./compile.sh --check SSD1306
	@ ./compile.sh --check HMC5883L
	@ ./compile.sh --check ITG3200
	@ ./compile.sh --check ADXL345

.PHONY : preprocess
preprocess :
	@ ./compile.sh --preprocess

.PHONY : clean
clean :
	@ rm -rf "out";

.PHONY : local-test
local-test :
	@ ./compile.sh --local-test

.PHONY : remote-test
remote-test :
# TODO

.PHONY : test
test : local-test remote-test check
