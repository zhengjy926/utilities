#ifndef RWONCE_H
#define RWONCE_H

#define READ_ONCE(x) (*(const volatile typeof(x) *)&(x))

#define WRITE_ONCE(x, val)			        \
do {									    \
	*(volatile typeof(x) *)&(x) = (val);	\
} while (0)

#endif //RWONCE_H

