#include "cassius.h"
#include <unistd.h>
#include <dirent.h>

BOOL is_c_file(const char *s)
{
	int i = 0;

	while (s[i])
		i++;
	return(i > 3 && s[i - 1] == 'c' && s[i - 2] == '.');
}

void tidy_prototypes(t_master *m)
{
	DIR *d;
	struct dirent *dir;

	(void)m;
	puts("Detected prototypes:");
	critical_test(chdir("src") == 0, "You must be at the root of your project. Your source folder must be named src");

	d = opendir(".");
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			printf("%s\n", dir->d_name);
		}
		closedir(d);
	}
}