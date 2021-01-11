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

static void	store_proto_names(const char *file_name, t_string_tab *protos)
{
	int fd = open(file_name, O_RDONLY);
	UINT len = file_len(fd), i = 0, start, prot_len;
	char *content = malloc(len + 1);

	read(fd, content, len);
	content[len] = '\0';
	close(fd);
	while (content[i])
	{
		if (is_word_material(content[i]))
		{
			start = pass_non_types(content, i, len);
			i = next_line_offset(content, i);
			printf("Proto detected. start: %c, end: %c.\n", content[start], content[i]);
			if (content[i] == '{')
			{
				prot_len = i - start - 1;
				protos->tab = realloc(protos->tab, (protos->cell_number + 1) * sizeof(char*));
				protos->tab[protos->cell_number] = malloc(prot_len + 2);
				strcpy_len(content + start, protos->tab[protos->cell_number], prot_len);
				protos->tab[protos->cell_number][prot_len] = ';';
				protos->tab[protos->cell_number][prot_len + 1] = '\0';
				(protos->cell_number)++;
			}
		}
		else
		{
			//printf("i = %u, max = %u\n ", i, len);
			i = next_line_offset(content, i);
		}
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
		puts("Adding to the s_tab for the tree the name:");
		write(1, proto_tab->tab[i] + len, func_name_len);
		names->tab[i] = malloc(func_name_len + 1);
		strcpy_len(names->tab[i], proto_tab->tab[i] + len, func_name_len);
		names->tab[func_name_len] = '\0';
		i++;
	}
	//build the tree here.
	res = word_tree(names);
	free_string_tab(names);
	puts("Finished builing the tree.");
	return (res);
}

static UINT	next_func_call(const char *s, UINT *pos, UINT *len)
{
	UINT i = 0, start;

	while (!s[i] || s[i] == '\n')
	{
		while (!is_word_material(s[i]))
		{
			i++;
			if (!s[i] || s[i] == '\n')
			{
				*pos += i;
				return (FALSE);
			}
		}
		start = i;
		while(is_word_material(s[i++]));
		if (s[i] == '(')
		{
			*pos += start;
			*len = i - start;
			return TRUE;
		}
	}
	*pos += i;
	return (FALSE);
}

static BOOL	is_outside_of_current_file(UINT pos, UINT *file_limits)
{
	return (pos < *file_limits || pos > file_limits[1]);
}

static void	add_extrafile_funcs(const char *file_name, UINT **interfile_funcs, UINT *interfile_func_nb, UINT *file_limits, t_word_tree *tree)
{
	int fd = open(file_name, O_RDONLY);
	UINT len = file_len(fd), i = 0, remainer_pos;
	char *content = malloc(len + 1);
	t_word_tree *parent_branch;

	read(fd, content, len);
	content[len] = '\0';
	close(fd);
	while (content[i])
	{
		if (is_sep(content[i]))
		{
			puts("trying a new line.");
			if (next_func_call(content + i, &i, &len))
			{
				puts("A func call was found at:");
				write(1, content + i, len);putchar('\n');
				parent_branch = get_word_info_from_tree(content, len, tree, &remainer_pos);
				if (parent_branch && is_outside_of_current_file(((t_remainer*)(parent_branch->kids[remainer_pos]))->pos, file_limits))
				{
					printf("Found a matching prototype out of the current file. The number of public func will now be: %u. It's pos is: %u\n", (*interfile_func_nb) + 1, ((t_remainer*)(parent_branch->kids[remainer_pos]))->pos);
					*interfile_funcs = realloc(*interfile_funcs, sizeof(UINT) * ((*interfile_func_nb) + 1));
					(*interfile_funcs)[(*interfile_func_nb)] = ((t_remainer*)(parent_branch->kids[remainer_pos]))->pos;
					(*interfile_func_nb)++;
					puts("Deleting the remainer and every branch above it that will consequently be left with no children.");
					delete_tree_end(parent_branch, remainer_pos);
				}
			}
		}
		else
			i = next_line_offset(content, i);
	}
	free(content);
}

//The interfile funcs will be stored inside file_limits. The number will directly returned.
UINT	search_interfile_funcs(t_word_tree *tree, UINT **file_limits)
{
	UINT			interfile_func_nb = 0, i = 0;
	UINT			*interfile_funcs = NULL;
	DIR				*d;
	struct dirent 	*dir;

	d = opendir(".");
	if (d)
	{
		puts("Beginning to search interfile functions.");
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_REG && is_dot(dir->d_name, 'c'))
			{
				printf("Opening %s. Current file limits: %u - %u\n", dir->d_name, (*file_limits)[i], (*file_limits)[i + 1]);
				add_extrafile_funcs(dir->d_name, &interfile_funcs, &interfile_func_nb, (*file_limits) + i, tree);
				i++;
			}
		}
		closedir(d);
	}
	free(*file_limits);
	*file_limits = interfile_funcs;
	puts("Finished finding functions whose prototypes must be public. Their index are:");
	i = 0;
	while (i != interfile_func_nb)
	{
		printf("%u\n", (*file_limits)[i]);
		i++;
	}
	return (interfile_func_nb);
}

static	void extract_prototypes(t_string_tab *protos, UINT *file_limits)
{
	DIR *d;
	struct dirent *dir;
	UINT	i = 0;

	d = opendir(".");
	if (d)
	{
		bzero(protos, sizeof(t_string_tab));
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_REG && is_dot(dir->d_name, 'c'))
			{
				file_limits[i++] = protos->cell_number;
				printf("Currently parsed file = %s\n", dir->d_name);
				store_proto_names(dir->d_name, protos);
			}
		}
		file_limits[i] = protos->cell_number + 1;
		closedir(d);
	}
	print_string_tab(protos);
}

void	tidy_prototypes(t_master *m)
{
	t_string_tab protos;
	t_word_tree *tree;
	UINT		*file_limits, i;

	puts("Detected prototypes:");
	critical_test(chdir("src") == 0, "You must be at the root of your project. Your source folder must be named src.");
	//puts("step 2");
	file_limits = malloc(sizeof(UINT) * (get_dir_files_number() + 1));
	//puts("step 3");
	extract_prototypes(&protos, file_limits);
	//puts("step 4");
	tree = create_func_names_tree(&protos);
	//puts("step 5");
	i = search_interfile_funcs(tree, &file_limits);
	//search for their prototype in the .h
	//if they are not in there, add them.
	//puts("step 6");
	i = 0;
	//puts("step 7");
	while (i != protos.cell_number)
		free(protos.tab[i++]);
	//puts("step 8");
	free(protos.tab);
	//puts("step 9");
	if (m->ft)
		puts("42 mode is not implemented yet. The generated prototypes may be too long, so you will still have to split them on two lines by yourself.");
}