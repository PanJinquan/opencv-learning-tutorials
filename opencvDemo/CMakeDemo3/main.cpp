#include <stdio.h>

#ifdef SET_DEBUG
#define PRINT	printf("DEBUG is define\n");
#else
#define PRINT	printf("DEBUG is not define\n");
#endif // SET_DEBUG

int main()
{
	PRINT;
	return 0;
}
