#include "autoprog.h"
#include <stdio.h>
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
			//printf("Proto detected. start: %c, end: %c.\n", content[start], content[i]);
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

	//printf("there are %u available name spaces.\n", names->cell_number);
	//create func names string tab
	while (i != names->cell_number)
	{
		len = get_word_len(proto_tab->tab[i]);
		len += get_sep_len(proto_tab->tab[i] + len);
		func_name_len = get_word_len(proto_tab->tab[i] + len);
		//puts("Adding to the s_tab for the tree the name:");
		//write(1, proto_tab->tab[i] + len, func_name_len);putchar('\n');
		//printf("func name len = %u.\n", func_name_len);
		names->tab[i] = malloc(func_name_len + 1);
		critical_test(names->tab[i] != NULL, "Malloc failed.");
		strcpy_len(proto_tab->tab[i] + len, names->tab[i], func_name_len);
	//	printf("names->tab[i] = %s\n", names->tab[i]);
		names->tab[i][func_name_len] = '\0';
		i++;
	}
	//puts("Func names:");
	//print_string_tab(names);
	//build the tree here.
	res = word_tree(names);
	free_string_tab(names);
	puts("Finished builing the tree.");
	return (res);
}

//Returns false if given char is a maj or a digit.
static BOOL	is_forbidden_func_border(const char c)
{
	return (is_digit(c) || is_maj(c));
}

//Returns true if a word material is found before line end or file end. Either way pos is updated on the parsed length.
static BOOL	find_next_word_material(const char *s, UINT *pos)
{
	UINT i = *pos;

	while (!is_word_material(s[i]))
	{
		if (s[i] == '\n' || !s[i])
		{
			*pos = i;
			printf("At pos %u, the string is at line or file end.\n", *pos);
			return (FALSE);
		}
		i++;
	}
	*pos = i;
	printf("At pos %u, the string has a word material.\n", *pos);
	return (TRUE);
}

/*
Position pos at the start of the detected func. Len will be the size of it. The position will either point at a line return, a func or the string end.
To identify a func candidat, we first want to find a word material. Then we count how many word materials are behind it. The final len must be >= min_len.
Note: The caracters preceding or folowing a func call can not be a number or a Maj.
*/
BOOL	next_func_call(const char *s, UINT *pos, UINT *len, const UINT min_len)
{
	while (find_next_word_material(s, pos))
	{
		if (*pos && is_forbidden_func_border(s[(*pos) - 1]))
		{
			printf("At pos %u, the string has a forbidden func border.\n", (*pos) - 1);//DEBUG1
			(*pos)++;
		}
		else
		{
			*len = get_word_len(s + (*pos));
			if (*len < min_len || is_forbidden_func_border(s[(*pos) + (*len)]))
			{
				if (*len < min_len)//DEBUG1
					printf("Calculated len %u < min len %u.\n", *len, min_len);//DEBUG1
				else//DEBUG1
					printf("String has a forbidden func border at pos %u (that number will be the new index of search).\n", (*pos) + (*len));//DEBUG1
				*pos += *len;
			}
			else
			{
				printf("Found a func candidat at pos %u and of length %u.\n", *pos, *len);//DEBUG1
				return (TRUE);
			}
		}
	}
	return (FALSE);
}

static BOOL	is_outside_of_current_file(UINT pos, UINT *file_limits)
{
	return (pos < *file_limits || pos > file_limits[1]);
}

static void	add_extrafile_funcs(const char *file_name, UINT **interfile_funcs, UINT *interfile_func_nb, UINT *file_limits, t_word_tree *tree)
{
	int fd = open(file_name, O_RDONLY);
	UINT len = file_len(fd), i = 0, remainer_pos, line = 1;
	char *content = malloc(len + 1);
	t_word_tree *parent_branch;

	read(fd, content, len);
	content[len] = '\0';
	close(fd);
	while (content[i])
	{
		if (is_sep(content[i]))
		{
			//printf("found a line beginning by a sep at line: %u\n", line);
			while (content[i] != '\n' && content[i])
			{
				if (next_func_call(content + i, &i, &len, 5))
				{
					printf("A func call was found at line %u:\n", line);
					write(1, content + i, len);putchar('\n');
					parent_branch = get_word_info_from_tree(content + i, len, tree, &remainer_pos);
					//printf("Search completed. The func call was %s.\n", parent_branch ? "found" : "not found");
					if (parent_branch && is_outside_of_current_file(((t_remainer*)(parent_branch->kids[remainer_pos]))->pos, file_limits))
					{
						printf("Found a matching prototype out of the current file. The number of public func will now be: %u. It's pos is: %u\n", (*interfile_func_nb) + 1, ((t_remainer*)(parent_branch->kids[remainer_pos]))->pos);
						*interfile_funcs = realloc(*interfile_funcs, sizeof(UINT) * ((*interfile_func_nb) + 1));
						(*interfile_funcs)[(*interfile_func_nb)] = ((t_remainer*)(parent_branch->kids[remainer_pos]))->pos;
						(*interfile_func_nb)++;
						puts("Deleting the remainer and every branch above it that will consequently be left with no children.");
						delete_tree_end(parent_branch, remainer_pos);
					}
					else
						puts("This func call refers a func defined in the same file or in a lib.");
					i += len;
				}
			}
			line++;
		}
		else
		{
			line++;
			i = next_line_offset(content, i);
		}
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
				if (strcmp_n(dir->d_name, slen(dir->d_name), "main.c", 6))
					exit(0);
				i++;
			}
		}
		closedir(d);
	}
	free(*file_limits);
	*file_limits = interfile_funcs;
	/* puts("Finished finding functions whose prototypes must be public. Their index are:");
	i = 0;
	while (i != interfile_func_nb)
	{
		printf("%u\n", (*file_limits)[i]);
		i++;
	} */
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
	//puts("Detected prototypes:");
	//print_string_tab(protos);
}

void	tidy_prototypes(t_master *m)
{
	t_string_tab protos;
	t_word_tree *tree;
	UINT		*file_limits, i, j = 0;

	critical_test(chdir("src") == 0, "You must be at the root of your project. Your source folder must be named src.");
	file_limits = malloc(sizeof(UINT) * (get_dir_files_number() + 1));
	extract_prototypes(&protos, file_limits);
	tree = create_func_names_tree(&protos);
	//puts("step 5");
	i = search_interfile_funcs(tree, &file_limits);
	puts("End of the searching job in C files. List of funcs needed in header file:");
	while (j != i)
	{
		puts(protos.tab[file_limits[j]]);
		j++;
	}
	exit(0);
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