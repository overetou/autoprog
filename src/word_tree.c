#include "cassius.h"
#include <string.h>
#include <unistd.h>

/*
Critical bug:
	if you store "bernard" in the tree and ask if "ber" is in it, it will say that it is so.
	By the way, what happens if you enter ["bernard", "berni", "ber"]? We have to signal that a full word is stored at this point but not elsewhere.
Possible ameliorations:
	Store the length of remainers beside them to avoid recomputing it each time they are assessed.
	Reorder the given s_tab by likedness, to just jump over the transfered ones on upper floors.
*/

//printing

void	print_parent_branch(t_word_tree *b)
{
	UINT i = 0;

	printf("Letter = %c, kids_nb = %u. Kids:\n", b->letter, b->kids_nb);
	while (i != b->kids_nb)
	{
		//if (((t_word_tree*)(b->kids[i]))->letter)
			printf("Letter branch: %c\n.", ((t_word_tree*)(b->kids[i]))->letter);
	//else
			printf("Remainer branch: %s.\n", ((t_remainer*)(b->kids[i]))->remainer);
		i++;
	}
}

//logic

static t_word_tree	*create_letter_branch(char content)
{
	printf("malloc of %lu in branch.\n", sizeof(t_word_tree));
	t_word_tree	*branch = malloc(sizeof(t_word_tree));
	if (branch)
	{
		branch->kids_nb = 0;
		branch->kids = NULL;
		printf("branch kids nb of new branch was set to %u.\n", branch->kids_nb);
		branch->letter = content;
		printf("branch letter: %c.\n", branch->letter);
		return (branch);
	}
	else
	{
		puts("Malloc failed.");
		exit(0);
	}
}

t_floor_data	*new_floor_data(t_string_tab *s_tab, t_word_tree *parent_branch)
{
	t_floor_data	*set = malloc(sizeof(t_floor_data));

	printf("malloc of %lu in set.\n", sizeof(t_floor_data));
	if (set)
	{
		set->s_tab = s_tab;
		puts("Affected s_tab to set. set->s_tab:");print_string_tab(set->s_tab);
		set->parent_branch = parent_branch;
		puts("Affected parent branch to set. set->parent_branch: ");
		print_parent_branch(set->parent_branch);
		set->pos = 0;
		printf("set->pos = %u. Done.\n", set->pos);
		return (set);
	}
	else
	{
		puts("Malloc failed.");
		exit(0);
	}
}

UINT	count_concurrents(t_floor_data *data_set, UINT notch)
{
	UINT	i, count = 0;

	printf("Locked: %s.\n", data_set->s_tab->tab[data_set->pos]);
	i = data_set->pos + 1;
	printf("\nCounting with notch on %u. data_set->pos = %u and i = %u.\n", notch, data_set->pos, i);
	while (i != data_set->s_tab->cell_number)
	{
		printf("Testing condition: %c == %c at i = %u\n", data_set->s_tab->tab[data_set->pos][notch], data_set->s_tab->tab[i][notch], i);
		if (data_set->s_tab->tab[data_set->pos][notch] == data_set->s_tab->tab[i][notch])
		{
			count++;
			printf("\"%s\" matched. Count is now %u.\n", data_set->s_tab->tab[i], count);
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

	printf("\nFloor %u: Kids count of current branch + 1 = %u.\n", notch, data_set->parent_branch->kids_nb + 1);
	data_set->parent_branch->kids = realloc(data_set->parent_branch->kids, (data_set->parent_branch->kids_nb + 1) * sizeof(void*));
	if (data_set->parent_branch->kids == NULL)
	{
		puts("Realloc failed.");
		exit(0);
	}
	printf("Creating a letter branch with letter %c. data_set->parent_branch->kids_nb will then be %u.\n", data_set->s_tab->tab[data_set->pos][notch], data_set->parent_branch->kids_nb + 1);
	data_set->parent_branch->kids[data_set->parent_branch->kids_nb]	= create_letter_branch(data_set->s_tab->tab[data_set->pos][notch]);
	(data_set->parent_branch->kids_nb)++;
	t->tab[0] = data_set->s_tab->tab[data_set->pos];
	printf("Added %s to the s_tab of the kid branch to be. (at index 0). We have %u concurrents to find.\n", t->tab[0], count);
	while (count)
	{
		printf("Testing: %c == %c. i = %u\n", t->tab[0][notch], data_set->s_tab->tab[i][notch], i);
		if (t->tab[0][notch] == data_set->s_tab->tab[i][notch])
		{
			t->tab[t->cell_number - count] = data_set->s_tab->tab[i];
			data_set->s_tab->tab[i][notch] = '.';
			printf("Added %s to the s_tab of the kid branch to be. (at index %u) Count will then be %u\n", t->tab[t->cell_number - count], t->cell_number - count, count - 1);
			count--;
		}
		i++;
	}
	puts("Done setting up a floor data and returning it.");
	return (new_floor_data(t, data_set->parent_branch->kids[data_set->parent_branch->kids_nb - 1]));
}

t_remainer *create_remainer(char *s)
{
	UINT	len = strlen(s);
	t_remainer *res = malloc(sizeof(t_remainer) + len + 1);

	if (res == NULL)
	{
		puts("Malloc failed.");
		exit(0);
	}
	printf("create remainer begins. len = %u, malloc a size of %lu to put into res.\n", len, sizeof(t_remainer) + len);
	res->fake_letter = 0;
	res->remainer = ((void*)res) + sizeof(t_remainer);
	strcpy_len(s, res->remainer, len);
	res->remainer[len] = '\0';
	printf("res->remainer = %s\nDone. Returning res.\n", res->remainer);
	return (res);
}

void	add_remainer(t_floor_data *data_set, UINT notch)
{
	printf("\nFloor %u: Kids count of current branch + 1 = %u.\n", notch, data_set->parent_branch->kids_nb + 1);
	printf("Adding a remainer. realloc data_set->parent_branch->kids with size: %lu.\n", (data_set->parent_branch->kids_nb + 1) * sizeof(void*));
	data_set->parent_branch->kids = realloc(data_set->parent_branch->kids, (data_set->parent_branch->kids_nb + 1) * sizeof(void*));
	if (data_set->parent_branch->kids == NULL)
	{
		puts("Realloc failed.");
		exit(0);
	}
	printf("Putting a remainer inside data_set->parent_branch->kids[%u].\n", data_set->parent_branch->kids_nb);
	data_set->parent_branch->kids[data_set->parent_branch->kids_nb] = create_remainer(data_set->s_tab->tab[data_set->pos] + notch);
	(data_set->parent_branch->kids_nb)++;
}

BOOL	is_count_relevant(t_floor_data *data_set, UINT notch)
{
 	UINT	max_relevant = data_set->s_tab->cell_number - 1;

	printf("Skipping already stored: %c == '.'?", data_set->s_tab->tab[data_set->pos][notch]);
	while(data_set->s_tab->tab[data_set->pos][notch] == '.')
	{
		if (data_set->pos == max_relevant)
			return (FALSE);
		(data_set->pos)++;
		if (data_set->pos != max_relevant)
			printf("Skipping already stored: %c == '.'?", data_set->s_tab->tab[data_set->pos][notch]);
	}
	return (TRUE);
}

t_word_tree	*word_tree(t_string_tab *s_tab)
{
	t_word_tree		*root = create_letter_branch('.');
	UINT			notch = 0, count;
	t_floor_data	*data_sets, *temp;

	data_sets = new_floor_data(s_tab, root);
	data_sets->next = NULL;
	while (data_sets)
	{
		puts("Starting a new floor.");
		printf("About to test: %u != %u. (data_sets->pos != data_sets->s_tab->cell_number)\n", data_sets->pos, data_sets->s_tab->cell_number);
		while (data_sets->pos != data_sets->s_tab->cell_number)
		{
			if (is_count_relevant(data_sets, notch))
			{
				count = count_concurrents(data_sets, notch);
				if (count)
				{
					temp = add_floor_data(data_sets, notch, count);
					temp->next = data_sets;
					(data_sets->pos)++;
					data_sets = temp;
					notch++;
					printf("Added a data pack. Notch is now %u.\n", notch);
				}
				else
				{
					add_remainer(data_sets, notch);
					printf("\"%s\" is well stored in the tree. New pos of current data pack = %u.\n", ((t_remainer*)(data_sets->parent_branch->kids[(data_sets->parent_branch->kids_nb) - 1]))->remainer, data_sets->pos + 1);
					(data_sets->pos)++;
				}
			}
			else
				break;
			printf("About to test: %u != %u. (data_sets->pos != data_sets->s_tab->cell_number)\n", data_sets->pos, data_sets->s_tab->cell_number);
		}
		temp = data_sets;
		data_sets = data_sets->next;
		puts("About to free the current data set.");
		free(temp);
		notch--;
		printf("Freed a data pack. Notch is now %u.\n", notch);
	}
	puts("Done! Don't forget to free your string_tab! (And don't fret by seeing the notch at 4294967295 just above. It's 0 - 1\n");
	return (root);
}

BOOL	is_word_in_tree(const char *word, UCHAR word_len, t_word_tree *root)
{
	UINT	notch = 0, i = 0;

	puts("Beginning search for:");write(1, word, word_len);putchar('\n');
	while (notch != word_len)
	{
		if (((t_word_tree*)(root->kids[i]))->letter)
		{
			printf("Comparing with letter %c.\n", ((t_word_tree*)(root->kids[i]))->letter);
			if (word[notch] == ((t_word_tree*)(root->kids[i]))->letter)
			{
				puts("The letter matched.");
				notch++;
				root = root->kids[i];
				i = 0;
			}
			else
			{
				i++;
				printf("No match. Increasing i to %u. Still %u chances to match.\n", i, root->kids_nb - i);
			}
		}
		else
		{
			printf("Comparing with remainer: %s\n", ((t_remainer*)(root->kids[i]))->remainer);
			if (strcmp_n(word + notch, word_len - notch, ((t_remainer*)(root->kids[i]))->remainer, strlen(((t_remainer*)(root->kids[i]))->remainer)))
			{
				puts("The remainer matched.");
				return (TRUE);
			}
			i++;
			printf("No match. Increasing i to %u. Still %u chances to match.\n", i, root->kids_nb - i);
		}
		if (i == root->kids_nb)
			return (FALSE);
		printf("Notch = %u, word_len = %u.\n", notch, word_len);
	}
	return (FALSE);
}