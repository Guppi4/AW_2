#include "codec.h"
#include <stdio.h>
#include <time.h>
int main(int argc, char *argv[])
{
	clock_t t;
	t = clock();
	char data[] = "inqtrzqvogqqjvxjlzwturgjqixawhbbmdixjwiaouklhcsiiimdnmyomyciwyvrxqimyngrwwlxuxreiuanjovfnjvcxbejfbdpuxlzetkcabpexnufpvvszzoaeokxpftcjjffrfxxktommqiecdhgbccmcrdmyvthorfawzulovlqkcchzlunhwfbgiqoelh"
;
	int key = 12;

	encrypt(data, key);
	printf("encripted data: %s\n", data);

	// decrypt(data,key);
	// printf("decripted data: %s\n",data);
	t = clock() - t;
	double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds

	printf("took %f seconds to execute \n", time_taken);
	return 0;
}