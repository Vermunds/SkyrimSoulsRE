#ifndef	SKYRIMSOULSRE_VERSION_INCLUDED
#define SKYRIMSOULSRE_VERSION_INCLUDED

#define MAKE_STR_HELPER(x) #x
#define MAKE_STR(x) MAKE_STR_HELPER(x)

#define SKYRIMSOULSRE_VERSION_MAJOR		1
#define SKYRIMSOULSRE_VERSION_MINOR		4
#define SKYRIMSOULSRE_VERSION_PATCH		1
#define SKYRIMSOULSRE_VERSION_BETA		0
#define SKYRIMSOULSRE_VERSION_VERSTRING	MAKE_STR(SKYRIMSOULSRE_VERSION_MAJOR) "." MAKE_STR(SKYRIMSOULSRE_VERSION_MINOR) "." MAKE_STR(SKYRIMSOULSRE_VERSION_PATCH) "." MAKE_STR(SKYRIMSOULSRE_VERSION_BETA)

#endif
