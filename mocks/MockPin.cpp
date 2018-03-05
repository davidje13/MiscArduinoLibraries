#include "MockPin.h"

Pin Pin::Mock::implementation(void) {
	return Pin(this);
}
