#include <stdio.h>

int main(void)
{
	int i;
	for (i = 0; i < 5000000000; i++) {
		printf("%c", (char)('a' + i % 26));
	}
	return 0;
}
