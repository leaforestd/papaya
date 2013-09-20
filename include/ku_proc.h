//this header contain vars,funcs,macros that be used by ring0 and ring3
#ifndef KU_PROC_H
#define KU_PROC_H

#define MSGTYPE_TIMER 255
#define MSGTYPE_CHAR 1

#define BOTTOM_PROC_STACK(pid) (BASE_PROCSTACK+LEN_PROCSTACK*(pid+1))
#define ERRNO(pid) (*((int*const)(BOTTOM_PROC_STACK(pid)-4)))


#endif
