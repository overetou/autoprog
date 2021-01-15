#include "autoprog.h"
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

BOOL is_digit(const char c)
{
	return (c >= '0' && c <= '9');
}

BOOL is_maj(const char c)
{
	return (c >= 'A' && c <= 'Z');
}

BOOL is_dot(const char *s, const char c)
{
	int i = 0;

	while (s[i])
		i++;
	return (i > 3 && s[i - 1] == c && s[i - 2] == '.');
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
	return ((c >= 'a' && c <= 'z') || c == '_');
}

BOOL is_type_material(const char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

BOOL is_sep(const char c)
{
	return (c == ' ' || c == '\t' || c == '*');
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
	while (s[i])
	{
		if (s[i] == '\n')
			return (i + 1);
		i++;
	}
	return (i);
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

	while (is_word_material(s[i]))
		i++;
	return (i);
}

UINT	get_sep_len(const char *s)
{
	UINT	i = 0;

	while (is_sep(s[i]))
		i++;
	return (i);
}

char *new_string(const char *s)
{
	UINT len = slen(s);
	char *alloc;

	alloc = malloc(len + 1);
	strcpy_len(s, alloc, len);
	alloc[len] = '\0';
	return (alloc);
}

t_string_tab	*new_string_tab(UINT cell_number)
{
	t_string_tab *t = malloc(sizeof(t_string_tab) + cell_number * sizeof(char*));
	t->tab = ((void*)t) + sizeof(t_string_tab);
	t->cell_number = cell_number;
	return (t);
}

void			free_string_tab(t_string_tab *to_free)
{
	UINT	i = 0;

	while (i != to_free->cell_number)
	{
		free(to_free->tab[i]);
		i++;
	}
	free(to_free);
}


//The result must be true else the given message is displayed and the program ends.
void critical_test(char bool_val, const char *msg)
{
	if (bool_val)
		return ;
	puts("\nError:");
	puts(msg);
	exit(0);
}

//returns true if both strings are the same.
BOOL strcmp_n(const char *s1, int s1_size, const char *s2, int s2_size)
{
	int i;

	//printf("Comparing %s of size %u and %s of size %u.\n", s1, s1_size, s2, s2_size);
	if (s1_size != s2_size)
		return (0);
	i = 0;
	while (i != s1_size)
	{
		if (s1[i] != s2[i])
			return (0);
		i++;
	}
	return (1);
}

UINT slen(const char *s)
{
	int i = 0;

	while (s[i])
		i++;
	return (i);
}

