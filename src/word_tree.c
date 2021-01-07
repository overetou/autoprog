#include "cassius.h"

static UCHAR	letter_number(const char c)
{
	if (c > 'Z')
		return (c - 'a');
	return (c - 'A');
}

void	create_sub_branchs(char *reserved, t_string_tab *prototypes, t_letter_branch *branch)
{
	int i = 0;

	branch->sub_branchs = malloc(sizeof(void*) * branch->sub_branchs_number);
	while (i != prototypes->cell_number)
	{
		if (reserved[i] == 1)
		{
			//create end word
		}
		else if (reserved[i] > 1)
		{
			//create letter
		}
	}
}

void	build_name_tree(t_string_tab *prototypes)
{
	char	reserved[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	UCHAR	i =	0, pos = 0, j;
	char	is_letter = 1;
	t_letter_branch *root;

	while (is_letter)
	{
		root->sub_branchs_number = 0;
		while (i != prototypes->cell_number)
		{
			if (!reserved[letter_number(prototypes->tab[i][pos])])
				(root->sub_branchs_number)++;
			reserved[letter_number(prototypes->tab[i][pos])]++;
			i++;
		}
		create_sub_branchs(reserved, prototypes, root);
		pos++;
		is_letter = 0;
		j = 0;
		while (j != 26)
		{
			if (reserved[j] > 1)
				is_letter = 1;
			reserved[j++] = 0;
		}
	}
}