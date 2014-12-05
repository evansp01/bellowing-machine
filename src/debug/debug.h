#ifndef DEBUG_
#define DEBUG_

#ifdef DEBUG
#define debugf(...) printf(__VA_ARGS__)
#else
#define debugf(...)
#endif

#endif