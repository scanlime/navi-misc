#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DEBUG

#ifdef DEBUG
    void Panic(void);
#   define DEBUG_ONLY(x)   x
#   define ASSERT(x)       \
    do { \
        if (!(x)) { \
            iprintf("\nASSERT failed at %s:%d\n", __FILE__, __LINE__); \
            Panic(); \
        } \
    } while (0);
#   define ASSERT_EQUAL(x, y) \
    do { \
        int _x = (x); \
        int _y = (y); \
        if (_x != _y) { \
            iprintf("\nASSERT failed at %s:%d\n %d != %d\n", __FILE__, __LINE__, _x, _y); \
            Panic(); \
        } \
    } while (0);
#   define LOG(x, args...)     iprintf(x "\n", ##args)
#else
#   define DEBUG_ONLY(x)
#   define ASSERT(x)
#   define ASSERT_EQUAL(x, y)
#   define LOG(x, args...)
#endif

#endif /* __DEBUG_H_ */

