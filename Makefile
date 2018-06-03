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

.PHONY : HCSR04
HCSR04 :
	@ ./compile.sh --upload HCSR04

.PHONY : HCSR04_LCD
HCSR04_LCD :
	@ ./compile.sh --upload HCSR04_LCD

.PHONY : DHT11
DHT11 :
	@ ./compile.sh --upload DHT11

.PHONY : CSNA2
CSNA2 :
	@ ./compile.sh --upload CSNA2

.PHONY : TM1638
TM1638 :
	@ ./compile.sh --upload TM1638

.PHONY : MY9221
MY9221 :
	@ ./compile.sh --upload MY9221

.PHONY : LCDMatrix88
LCDMatrix88 :
	@ ./compile.sh --upload LCDMatrix88

.PHONY : RotaryEncoder
RotaryEncoder :
	@ ./compile.sh --upload RotaryEncoder

.PHONY : IR
IR :
	@ ./compile.sh --upload IR

.PHONY : check
check :
	@ ./compile.sh --check SSD1306
	@ ./compile.sh --check HMC5883L
	@ ./compile.sh --check ITG3200
	@ ./compile.sh --check ADXL345
	@ ./compile.sh --check HCSR04
	@ ./compile.sh --check HCSR04_LCD
	@ ./compile.sh --check DHT11
	@ ./compile.sh --check CSNA2
	@ ./compile.sh --check TM1638
	@ ./compile.sh --check MY9221
	@ ./compile.sh --check LCDMatrix88
	@ ./compile.sh --check RotaryEncoder
	@ ./compile.sh --check IR

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
