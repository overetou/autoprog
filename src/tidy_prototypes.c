#include "cassius.h"
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>
#include <strings.h>

static UINT	pass_non_types(const char *content, UINT i, const UINT len)
{
	if (i + 6 < len && strcmp_on_n(content + i, "static", 6))
		i += 7;
	if (i + 5 < len && strcmp_on_n(content + i, "const", 5))
		i += 6;
	if (i + 8 < len && strcmp_on_n(content + i, "unsigned", 8))
		i += 9;
	return (i);
}

static void	store_proto_names(t_master *m, const char *file_name, t_string_tab *protos)
{
	int fd = open(file_name, O_RDONLY);
	UINT len = file_len(fd);
	char *content = malloc(len + 1);
	UINT i = 0, start;

	read(fd, content, len);
	content[len] = '\0';
	close(fd);
	(void)m;
	while (content[i])
	{
		if (is_alpha(content[i]))
		{
			start = pass_non_types(content, i, len);
			i = next_line_offset(content, i);
			if (content[i] == '{')
			{
				protos->cell_number++;
				protos->tab = realloc(protos->tab, protos->cell_number * sizeof(char*));
				protos->tab[protos->cell_number - 1] = malloc(i - start + 1);
				strcpy_len(content + start, protos->tab[protos->cell_number - 1], i - start - 1);
				protos->tab[protos->cell_number - 1][i - start - 2] = ';';
				protos->tab[protos->cell_number - 1][i - start - 1] = '\0';
			}
		}
		else
			i = next_line_offset(content, i);
	}
	free(content);
}

t_word_tree	*create_func_names_tree(t_string_tab *proto_tab)
{
	UINT			i = 0, len, func_name_len;
	t_string_tab	*names = new_string_tab(proto_tab->cell_number);
	t_word_tree		*res;

	//create func names string tab
	while (i != names->cell_number)
	{
		len = get_word_len(proto_tab->tab[i]);
		len += get_sep_len(proto_tab->tab[i] + len);
		func_name_len = get_word_len(proto_tab->tab[i] + len);
		names->tab[i] = malloc(func_name_len + 1);
		strcpy_len(names->tab[i], proto_tab->tab[i] + len, func_name_len);
		names->tab[func_name_len] = '\0';
		i++;
	}
	//build the tree here.
	res = word_tree(names);
	free_string_tab(names);
	return (res);
}

void	tidy_prototypes(t_master *m)
{
	DIR *d;
	struct dirent *dir;
	t_string_tab protos;
	t_word_tree *tree;
	int			*interfile_funcs;

	(void)m;
	puts("Detected prototypes:");
	critical_test(chdir("src") == 0, "You must be at the root of your project. Your source folder must be named src.");
	d = opendir(".");
	if (d)
	{
		bzero(&protos, sizeof(protos));
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_REG && is_dot(dir->d_name, 'c'))
				store_proto_names(m, dir->d_name, &protos);
		}
		closedir(d);
	}
	print_string_tab(&protos);
	//Create func names tree
	tree = create_func_names_tree(&protos);
	//create index of inter file functions
	interfile_funcs = search_interfile_funcs(tree, opendir("."));
	//search for their prototype in the .h
	//if they are not in there, add them.
	if (protos.tab)
		free(protos.tab);
}