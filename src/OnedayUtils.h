// HACK TO BRING TO FRONT AT LAUNCH
#define ALWAYS_ON_TOP_HACK 	static bool hasBecomeFirstResponder = false; \
	if( !hasBecomeFirstResponder && getElapsedSeconds() > 2 ) { \
	hasBecomeFirstResponder = true; \
	this->setAlwaysOnTop( false ); \
	} \


