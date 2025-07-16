#ifndef __RWONCE_H
#define __RWONCE_H

#define READ_ONCE(x) (*(const volatile typeof(x) *)&(x))

#define WRITE_ONCE(x, val)			        \
do {									    \
	*(volatile typeof(x) *)&(x) = (val);	\
} while (0)

#endif //__RWONCE_H

