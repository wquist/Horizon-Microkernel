#define __VA_ALIGNED_SIZE(x) ((sizeof(x) + sizeof(int) - 1) & ~(sizeof(int) - 1))

typedef struct __va_list* va_list;

#define va_start(ap, last) ((ap) = (void*)(((char*)&(last)) + __VA_ALIGNED_SIZE(last)))
#define va_end(ap) ((void)0)
#define va_copy(dst, src) ((dst) = (src))

#define va_arg(ap, type) \
	( ((ap) = (va_list)((char*)(ap) + __VA_ALIGNED_SIZE(type))), \
	*(type*)(void*)((char*)(ap) - __VA_ALIGNED_SIZE(type)) )
