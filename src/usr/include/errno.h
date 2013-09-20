#ifndef ERRNO_H
#define ERRNO_H
#define errno (ERRNO(getpid()))
#endif
