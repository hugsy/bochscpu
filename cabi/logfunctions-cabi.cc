// 3/25/19: rust cannot currently define extern C variadic functions so were
// stuck with this shit
//
// TODO when rust supports extern variadic functions, replace all of this
// with var args versions
//
// 4/18/19 so because were stuck with this shit right now we spend a lot of
// time inside vsnprintf, only to discard the formatted results. We'll hack
// around this by not doing shit on release builds.
#include <stdio.h>
#include <stdarg.h>

#include "bochs.h"

namespace rust {
extern "C" {
	void logfunctions_error(const char *);
	void logfunctions_ldebug(const char *);
	void logfunctions_info(const char *);
	void logfunctions_panic(const char *);
	void logfunctions_fatal1(const char *);
	// Added in bochs#67466a4139e24065e9d05b1ee13d2041445f82d5
	void logfunctions_warn(const char *);
	void logfunctions_lwarn(const char *);
}
}

int logfunctions::default_onoff[N_LOGLEV] =
{
  ACT_IGNORE,  // ignore debug
  ACT_REPORT,  // report info
  ACT_REPORT,  // report error
  ACT_FATAL    // on panic, quit
};

logfunctions::logfunctions(void) {}
logfunctions::~logfunctions(void) {}

void logfunctions::error(const char *fmt, ...) {
#ifndef RUST_CC_RELEASE
	char buf[0x1000];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	rust::logfunctions_error(buf);
#else
	(void)fmt;
#endif
}

void logfunctions::fatal1(const char *fmt, ...) {
#ifndef RUST_CC_RELEASE
	char buf[0x1000];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	rust::logfunctions_fatal1(buf);
#else
	(void)fmt;
#endif
}

void logfunctions::warn(const char *fmt, ...)
{
#ifndef RUST_CC_RELEASE
	char buf[0x1000];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	rust::logfunctions_warn(buf);
#else
	(void)fmt;
#endif
}

void logfunctions::lwarn(const char *fmt, ...)
{
#ifndef RUST_CC_RELEASE
	char buf[0x1000];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	rust::logfunctions_lwarn(buf);
#else
	(void)fmt;
#endif
}

void logfunctions::info(const char *fmt, ...) {
#ifndef RUST_CC_RELEASE
	char buf[0x1000];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	rust::logfunctions_info(buf);
#else
	(void)fmt;
#endif
}

void logfunctions::ldebug(const char *fmt, ...) {
#ifndef RUST_CC_RELEASE
	char buf[0x1000];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	rust::logfunctions_ldebug(buf);
#else
	(void)fmt;
#endif
}

void logfunctions::panic(const char *fmt, ...) {
#ifndef RUST_CC_RELEASE
	char buf[0x1000];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof buf, fmt, args);
	va_end(args);

	rust::logfunctions_panic(buf);
#else
	(void)fmt;
#endif
}

void logfunctions::put(const char *p, const char *q) {}

BOCHSAPI class logfunctions *genlog = NULL;
