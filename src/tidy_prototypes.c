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

UINT	next_char_in_tab_offset(const char *content, UINT i, const char *possible_ends, UINT tab_size)
{
	UINT	evaluated_char_index;

	while (content[i])
	{
		evaluated_char_index = 0;
		while (evaluated_char_index != tab_size)
		{
			if (content[i] == possible_ends[evaluated_char_index])
				return i;
			evaluated_char_index++;
		}
		i++;
	}
	return i;
}

void	store_proto_names(const char *file_name, t_string_tab *protos)
{
	int fd = open(file_name, O_RDONLY);
	UINT len = file_len(fd), i = 0, start, prot_len;
	char *content = malloc(len + 1), possible_ends[] = {';', '{'};

	read(fd, content, len);
	content[len] = '\0';
	close(fd);
	while (content[i])
	{
		if (is_type_material(content[i]))
		{
			start = pass_non_types(content, i, len);
			i = next_char_in_tab_offset(content, i, possible_ends, 2);
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

static t_string_tab *cpy_str_tab_one_alloc(t_string_tab *names, const UINT full_len)
{
	t_string_tab *one_alloc_names;
	UINT	i = 0, written_len = names->cell_number * sizeof(char*), len;

	one_alloc_names = malloc(sizeof(t_string_tab) + names->cell_number * sizeof(char*) + full_len);
	one_alloc_names->tab = ((void*)one_alloc_names) + sizeof(t_string_tab);
	one_alloc_names->cell_number = names->cell_number;
	while (i != one_alloc_names->cell_number)
	{
		one_alloc_names->tab[i] = ((void*)(one_alloc_names->tab)) + written_len;
		len = slen(names->tab[i]);
		strcpy_len(names->tab[i], one_alloc_names->tab[i], len);
		one_alloc_names->tab[i][len] = '\0';
		written_len += len + 1;
		i++;
	}
	return (one_alloc_names);
}

static t_word_tree	*create_func_names_tree(t_string_tab *proto_tab, UINT *shortest_len, t_string_tab **names)
{
	UINT			i = 0, len, func_name_len, full_len = 0;
	t_word_tree		*res;
	t_string_tab *one_alloc_names;

	*names = new_string_tab(proto_tab->cell_number);
	print_string_tab(proto_tab);
	exit(0);
//	printf("there are %u available name spaces.\n", names->cell_number);//debug4
	//create func names string tab
	while (i != (*names)->cell_number)
	{
		len = get_chunk_len(proto_tab->tab[i], is_type_material);
		len += get_sep_len(proto_tab->tab[i] + len);
		func_name_len = get_chunk_len(proto_tab->tab[i] + len, is_word_material);
//		puts("Adding to the s_tab for the tree the name:");//debug4
//		write(1, proto_tab->tab[i] + len, func_name_len);putchar('\n');//debug4
//		printf("func name len = %u.\n", func_name_len);//debug4
		(*names)->tab[i] = malloc(func_name_len + 1);
		critical_test((*names)->tab[i] != NULL, "Malloc failed.");
		strcpy_len(proto_tab->tab[i] + len, (*names)->tab[i], func_name_len);
		(*names)->tab[i][func_name_len] = '\0';
//		printf("names->tab[i] = %s\n", names->tab[i]);//debug4
		i++;
		if (func_name_len < *shortest_len)
			*shortest_len = func_name_len;
		full_len += func_name_len + 1;
	}
	one_alloc_names = cpy_str_tab_one_alloc(*names, full_len);
//	puts("Func names:");//debug4
//	print_string_tab(names);//debug4
	//build the tree here.
	res = word_tree(*names);
//	puts("Finished builing the tree.");//debug4
//	exit(0);//debug4
	free_string_tab(*names);
	*names = one_alloc_names;
	return (res);
}

//Returns false if given char is a maj or a digit.
static BOOL	is_forbidden_func_border(const char c)
{
	return (is_digit(c) || is_maj(c) || c == '[');
}

//Returns true if a word material is found before line end or file end. Either way pos is updated on the parsed length.
static BOOL	find_next_word_material(const char *s, UINT *pos)
{
	UINT i = *pos;

	while (!is_word_material(s[i]))
	{
		if (s[i] == '\'' || s[i] == '"')
			i = skip_brackets(s, i);
		if (s[i] == '\n' || !s[i])
		{
			*pos = i;
//			printf("At pos %u, the string is at line or file end.\n", *pos);//DEBUG1
			return (FALSE);
		}
		i++;
	}
	*pos = i;
//	printf("At pos %u, the string has a word material.\n", *pos);//DEBUG1
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
//			printf("At pos %u, the string has a forbidden func border.\n", (*pos) - 1);//DEBUG1
			(*pos)++;
		}
		else
		{
			*len = get_chunk_len(s + (*pos), is_word_material);
			if (*len < min_len || is_forbidden_func_border(s[(*pos) + (*len)]))
			{
//				if (*len < min_len)//DEBUG1
//					printf("Calculated len %u < min len %u.\n", *len, min_len);//DEBUG1
//				else//DEBUG1
//					printf("String has a forbidden func border at pos %u (that number will be the new index of search).\n", (*pos) + (*len));//DEBUG1
				*pos += *len;
			}
			else
			{
//				printf("Found a func candidat at pos %u and of length %u.\n", *pos, *len);//DEBUG1
				return (TRUE);
			}
		}
	}
	return (FALSE);
}

static BOOL	is_outside_of_current_file(UINT pos, const UINT *file_limits)
{
	return (pos < *file_limits || pos > file_limits[1]);
}

void	add_extrafile_funcs(const char *file_name, UINT **interfile_funcs, UINT *interfile_func_nb, const UINT *file_limits, t_word_tree *tree, const UINT shortest_len)
{
	int fd = open(file_name, O_RDONLY);
	UINT len = file_len(fd), i = 0, remainer_pos;
//	UINT line = 1, parsed = 0;//debug3
	char *content = malloc(len + 1);
	t_word_tree *branch;

	read(fd, content, len);
	content[len] = '\0';
	close(fd);
	while (content[i])
	{
		if (is_sep(content[i]))
		{
//			printf("Line %u begins with a sep. Continuing.\n", line);//debug3
			while (next_func_call(content, &i, &len, shortest_len))
			{
//				printf("Found a potential func call at col %u. Continuing.\n", i - parsed);//debug3
				branch = get_word_info_from_tree(content + i, len, tree, &remainer_pos);
				if (branch && is_outside_of_current_file(((t_remainer*)(branch->kids[remainer_pos]))->pos, file_limits))
				{
//					puts("Found a matching func:");write(1, content + i, len);putchar('\n');//debug3
					*interfile_funcs = realloc(*interfile_funcs, sizeof(UINT) * ((*interfile_func_nb) + 1));
					(*interfile_funcs)[*interfile_func_nb] = ((t_remainer*)(branch->kids[remainer_pos]))->pos;
					(*interfile_func_nb)++;
					delete_tree_end(branch, remainer_pos);
				}
//				else//debug3
//					puts("The candidat was not found in the tree.");//debug3
				i += len;
			}
			if (content[i] == '\n')
			{
//				printf("Arrived at line end (line %u). Advancing by one.\n", line);//debug3
//				line++;parsed=i + 1;//debug3
				i++;
			}
		}
		else
		{
//			printf("Line %u does not start with a sep. Skipping it.\n", line);//debug3
			i = next_line_offset(content, i);
//			//printf("pos is now %u.\n", i);//debug3
//			line++;parsed=i;//debug3
		}
	}
//	printf("Done at line %u and pos %u.\n", line, i);//debug3
	free(content);
}

//The interfile funcs will be stored inside file_limits. The number will directly returned.
UINT	search_interfile_funcs(t_word_tree *tree, UINT **file_limits, const UINT shortest_len)
{
	UINT			interfile_func_nb = 0, i = 0;
	UINT			*interfile_funcs = NULL;
	DIR				*d;
	struct dirent 	*dir;

	d = opendir(".");
	if (d)
	{
//		puts("Beginning to search interfile functions.");//debug5
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_REG && is_dot(dir->d_name, 'c'))
			{
//				printf("Opening %s. Current file limits: %u - %u\n", dir->d_name, (*file_limits)[i], (*file_limits)[i + 1]);//debug5
				add_extrafile_funcs(dir->d_name, &interfile_funcs, &interfile_func_nb, (*file_limits) + i, tree, shortest_len);
//				if (strcmp_n(dir->d_name, slen(dir->d_name), "common.c", 8))//debug5
//					exit(0);//debug5
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
	}
	exit(0); */
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
				//printf("Currently parsed file = %s\n", dir->d_name);
				store_proto_names(dir->d_name, protos);
			}
		}
		file_limits[i] = protos->cell_number + 1;
		closedir(d);
	}
	//puts("Detected prototypes:");
	//print_string_tab(protos);
}

static UINT	pass_typedefs(const char *s, const UINT len)
{
	UINT	i = 0;
	UINT	line = 1;

	while (s[i])
	{
		if (is_type_material(s[i]))
		{
			if (i + 7 < len && strcmp_on_n(s + i, "typedef", 7))
				i += 7;
			else
				break ;
		}
		line++;
		i = next_line_offset(s, i);
	}
	/* printf("Found proto beginning on line %u.\n", line);
	exit(0); */
	return (i);
}

static BOOL	is_in_tab(const char *s, UINT func_name_len, t_string_tab *names, UINT *to_add, const UINT to_add_len, UINT *pos)
{
	UINT i = 0;

	while (i != to_add_len)
	{
		if (strcmp_n(names->tab[to_add[i]], slen(names->tab[to_add[i]]), s, func_name_len))
		{
			//puts(names->tab[to_add[i]]);//debug
			*pos = i;
			return (TRUE);
		}
		i++;
	}
	return (FALSE);
}

static UINT	list_out_names(t_string_tab *names, UINT *to_add, UINT *to_add_len, const char *file_name)
{
	int fd = open(file_name, O_RDONLY);
	UINT len = file_len(fd), i, func_name_len, pos;
	char *content = malloc(len + 1);

	read(fd, content, len);
	content[len] = '\0';
	close(fd);
	critical_test(len > 8 && strcmp_on_n(content + len - 8, "\n#endif\n", 8), "Your header must be protected.");
	i = pass_typedefs(content, len);
	(void)names;
	(void)to_add;
	//puts("\nAlready present funcs:");//debug
	while (content[i])
	{
		if (is_type_material(content[i]))
		{
			i += get_chunk_len(content + i, is_type_material);
			i += get_sep_len(content + i);
			func_name_len = get_chunk_len(content + i, is_word_material);
			if (is_in_tab(content + i, func_name_len, names, to_add, *to_add_len, &pos))
			{
				(*to_add_len)--;
				while (pos != *to_add_len)
				{
					to_add[pos] = to_add[pos + 1];
					pos++;
				}
			}
		}
		i = next_line_offset(content, i);
	}
//	printf("Length of the remaining protos to add: %u\n", *to_add_len);
	free(content);
	return (len);
}

static void		do_file_edit(t_string_tab *protos, UINT *to_add, const UINT to_add_len, const char *file_name, const UINT file_len)
{
	int fd = open(file_name, O_WRONLY);
	UINT	i = 0;
	int	offset;

	critical_test(fd != -1, "Failed to edit the header file. Do the program have the right permissions?");
	offset = lseek(fd, file_len - 8, SEEK_SET);
	printf("offset = %d\n", offset);
	while (i != to_add_len)
	{
		write(fd, protos->tab[to_add[i]], slen(protos->tab[to_add[i]]));
		write(fd, "\n", 1);
		i++;
	}
	write(fd, "\n#endif\n", 8);
	close(fd);
}

static void	add_prototypes(t_string_tab *protos, t_string_tab *names, UINT *to_add, UINT to_add_len)
{
	//search for already added protos and outlist them
	DIR *d;
	struct dirent *dir;
	char *file_name = NULL;
	UINT	file_len;

	critical_test(chdir("../includes") == 0, "You must be at the root of your project. Your header folder must be named \"includes\".");
	d = opendir(".");
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_REG && is_dot(dir->d_name, 'h'))
			{
				file_name = new_string(dir->d_name);
				break ;
			}
		}
		closedir(d);
	}
	critical_test(file_name != NULL, "You must have created a header file in your includes/ dir before using this command. Don't forget to secure it! (ifndef,define,endif)");
	//printf("Found header file name: %s\n", file_name);
	(void)protos;
	file_len = list_out_names(names, to_add, &to_add_len, file_name);
	if (to_add_len)
		do_file_edit(protos, to_add, to_add_len, file_name, file_len);
	//add remaining protos at the end of the header file, but before the ifndef end.
	free(to_add);
	free(file_name);
}

void	tidy_prototypes(t_master *m)
{
	t_string_tab protos, *names;
	t_word_tree *tree;
	UINT		*file_limits, i, shortest_func_len = 100;
	//UINT j = 0;

	critical_test(chdir("src") == 0, "You must be at the root of your project. Your source folder must be named src.");
	file_limits = malloc(sizeof(UINT) * (get_dir_files_number() + 1));
	puts("Calculated file limits.");//debugseg
	extract_prototypes(&protos, file_limits);
	puts("Extracted prototypes.");//debugseg
	tree = create_func_names_tree(&protos, &shortest_func_len, &names);
	puts("Extracted prototypes.");//debugseg
	i = search_interfile_funcs(tree, &file_limits, shortest_func_len);
	puts("Found interfile funcs.");//debugseg
	free_word_tree(tree);
	puts("Freed remaining tree.");//debugseg
	/* puts("End of the searching job in C files. List of funcs needed in header file:");
	while (j != i)
	{
		puts(names->tab[file_limits[j]]);
		j++;
	} */
	add_prototypes(&protos, names, file_limits, i);
	puts("Added prototypes.");//debugseg
	i = 0;
	while (i != protos.cell_number)
		free(protos.tab[i++]);
	free(names);
	free(protos.tab);
	puts("Freed everything.");//debugseg
	if (m->ft)
		puts("42 mode is not implemented yet. The generated prototypes may be too long, so you will still have to split them on two lines by yourself.");
}