#ifndef VERSION_H_
#define VERSION_H_

#define PBUOGT_MAJOR_VERSION 0 
#define PBUOGT_MINOR_VERSION 2
#define PBUOGT_MICRO_VERSION 0

#define IS_COMPATABILE(x,y) \
	(PBUOGT_MAJOR_VERSION == x && PBUOGT_MINOR_VERSION == y ? 1 : 0)

#endif /* !VERSION_H_ */
