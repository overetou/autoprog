#include "cassius.h"
#include <unistd.h>
#include <dirent.h>

BOOL strcmp_on_n(const char *s1, const char *s2, int n)
{
	int i = 0;

	while (i != n)
	{
		if (s1[i] != s2[i])
			return (FALSE);
		i++;
	}
	return (TRUE);
}

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

UINT get_dir_files_number()
{
	DIR *d;
	struct dirent *dir;
	UINT	res = 0;

	d = opendir(".");
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_REG && is_dot(dir->d_name, 'c'))
				res++;
		}
		closedir(d);
	}
	return (res);
}

BOOL is_word_material(const char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
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
	UINT i = 0;

	//printf("cell number: %d\n", protos->cell_number);
	while (i != protos->cell_number)
	{
		puts(protos->tab[i]);
		i++;
	}
}

UINT	get_word_len(const char *s)
{
	UINT	i = 0;

	while(is_word_material(s[i]))
		i++;
	return (i);
}

UINT	get_sep_len(const char *s)
{
	UINT	i = 0;

	while(is_sep(s[i]))
		i++;
	return (i);
}
