#include "cassius.h"
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>
#include <strings.h>

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
			if (i + 6 < len && strcmp_on_n(content + i, "static", 6))
				i += 7;
			start = i;
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

void	tidy_prototypes(t_master *m)
{
	DIR *d;
	struct dirent *dir;
	t_string_tab protos;

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
	//build the tree here.
	if (protos.tab)
		free(protos.tab);
}