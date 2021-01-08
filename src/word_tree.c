#include "cassius.h"
#include <string.h>
#include <unistd.h>

static t_word_tree	*create_letter_branch(char content)
{
	t_word_tree	*branch = malloc(sizeof(t_word_tree));
	branch->kids_nb = 0;
	branch->letter = content;
	return (branch);
}

t_floor_data	*new_floor_data(t_string_tab *s_tab, t_word_tree *parent_branch, UINT pos)
{
	t_floor_data	*set = malloc(sizeof(t_floor_data));

	set->s_tab = s_tab;
	set->pos = pos;
	set->parent_branch = parent_branch;
	return (set);
}

UINT	count_concurrents(t_floor_data *data_set, UINT notch)
{
	UINT	i = data_set->pos + 1, count = 0;

	printf("\nCounting with notch on %u.\n", notch);
	while (i != data_set->s_tab->cell_number)
	{
		if (data_set->s_tab->tab[data_set->pos][notch] == data_set->s_tab->tab[i][notch])
			{
				printf("\"%s\" matched.\n", data_set->s_tab->tab[i]);
				count++;
			}
		i++;
	}
	printf("Counted %u concurrents in total.\n", count);
	return (count);
}

t_floor_data	*add_floor_data(t_floor_data *data_set, UINT notch, UINT count)
{
	t_string_tab	*t = new_string_tab(count + 1);
	UINT			i = data_set->pos + 1;

	printf("\nKids count of current branch + 1 = %u.\n", data_set->parent_branch->kids_nb + 1);
	data_set->parent_branch->kids = realloc(data_set->parent_branch->kids, data_set->parent_branch->kids_nb + 1);
	data_set->parent_branch->kids[data_set->parent_branch->kids_nb]	= create_letter_branch(data_set->s_tab->tab[data_set->pos][notch]);
	(data_set->parent_branch->kids_nb)++;
	t->tab[0] = data_set->s_tab->tab[data_set->pos];
	printf("Added %s to the s_tab of the kid branch to be. (at index 0)\n", t->tab[0]);
	while (count)
	{
		if (data_set->s_tab->tab[data_set->pos][notch] == data_set->s_tab->tab[i][notch])
		{
			t->tab[t->cell_number - count] = data_set->s_tab->tab[i];
			printf("Added %s to the s_tab of the kid branch to be. (at index %u)\n", t->tab[t->cell_number - count], t->cell_number - count);
			count--;
		}
	}
	return (new_floor_data(t, data_set->parent_branch->kids[data_set->parent_branch->kids_nb - 1], data_set->pos));
}

t_remainer *create_remainer(const char *s)
{
	UINT	len = strlen(s);
	t_remainer *res = malloc(sizeof(t_remainer) + len);

	res->fake_letter = 0;
	res->remainer = (char*)(res + sizeof(t_remainer));
	strcpy_len(s, res->remainer, len);
	return (res);
}

void	add_remainer(t_floor_data *data_set, UINT notch)
{
	data_set->parent_branch->kids = realloc(data_set->parent_branch->kids, data_set->parent_branch->kids_nb + 1);
	data_set->parent_branch->kids[data_set->parent_branch->kids_nb] = create_remainer(data_set->s_tab->tab[data_set->pos] + notch);
	(data_set->parent_branch->kids_nb)++;
}

t_word_tree	*word_tree(t_string_tab *s_tab)
{
	t_word_tree		*root = create_letter_branch('\0');
	UINT			notch = 0, count;
	t_floor_data	*data_sets, *temp;

	data_sets = new_floor_data(s_tab, root, 0);
	data_sets->next = NULL;
	while (data_sets)
	{
		printf("Target locked: %s.\n", data_sets->s_tab->tab[data_sets->pos]);
		while (data_sets->pos != data_sets->s_tab->cell_number)
		{
			count = count_concurrents(data_sets, notch);
			if (count)
			{
				temp = add_floor_data(data_sets, notch, count);
				temp->next = data_sets;
				data_sets = temp;
				notch++;
				printf("added a data pack. Notch is now %u.\n", notch);
			}
			else
			{
				add_remainer(data_sets, notch);
				printf("\"%s\" is well stored in the tree. New pos of current data pack = %u.\n", data_sets->s_tab->tab[data_sets->pos], data_sets->pos + 1);
				(data_sets->pos)++;
			}
		}
		temp = data_sets;
		data_sets = data_sets->next;
		free(temp);
		notch--;
		printf("Freed a data pack. Notch is now %u.\n", notch);
	}
	puts("Done! Don't forget to free your string_tab!\n");
	return (root);
}

BOOL	is_word_in_tree(const char *word, UCHAR word_len, t_word_tree *root)
{
	UINT	notch = 0, i = 0;

	puts("Beginning search for:");write(1, word, word_len);putchar('\n');
	while (root->letter)
	{
		if (((t_word_tree*)(root->kids[i]))->letter == word[notch])
		{
			printf("letter '%c' matched.\n", word[notch]);
			if (notch == word_len)
				return TRUE;
			notch++;
			root = root->kids[i];
			i = 0;
		}
		else
		{
			printf("No match for letter: %c.\n", ((t_word_tree*)(root->kids[i]))->letter);
			i++;
			if (root->kids_nb == i)
			{
				puts("No more available letter at this floor. No match.");
				return FALSE;
			}
		}
	}
	return (strcmp_n(word + notch, word_len - notch, ((t_remainer*)root)->remainer, strlen(((t_remainer*)root)->remainer)));
}