#include "cassius.h"
#include <unistd.h>

BOOL is_dot(const char *s, const char c)
{
	int i = 0;

	while (s[i])
		i++;
	return(i > 3 && s[i - 1] == c && s[i - 2] == '.');
}

UINT file_len(int fd)
{
	UINT res = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	return (res);
}

BOOL is_alpha(const char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

BOOL is_sep(const char c)
{
	return (c == ' ' || c == '\t');
}

void strcpy_len(const char *src, char *dest, UINT len)
{
	UINT i = 0;

	while (i != len)
	{
		dest[i] = src[i];
		i++;
	}
}

UINT next_line_offset(const char *s, UINT i)
{
	while (s[i] && s[i] != '\n')
		i++;
	return (i + 1);
}

void 	print_string_tab(t_string_tab *protos)
{
	int i = 0;
	UINT pos = 0;

	printf("cell number: %d\n", protos->cell_number);
	while (i++ != protos->cell_number)
	{
		puts(protos->tab + pos);
		while (protos->tab[pos++]);
	}
}
