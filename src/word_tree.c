#include "cassius.h"
#include <string.h>

t_word_tree *tree_dive(t_word_tree *root, UINT notch)
{
	UINT	floor = 0;

	while (floor++ != notch)
		root = *(root->kids);
	return (root);
}

void	prepare_adding_object(t_word_tree *branch)
{
	if (branch->kids_nb)
	{
		branch->kids = realloc(branch->kids - branch->kids_nb + 1, sizeof(void*) * branch->kids_nb);
		branch->kids += branch->kids_nb;
		(branch->kids_nb)++;
	}
	else
	{
		branch->kids = malloc(sizeof(void*));
		branch->kids_nb = 1;
	}
}

BOOL	several_words_of_letter(t_string_tab *s_tab, UINT notch)
{
	int i = 1;

	while (i != s_tab->cell_number)
	{
		if (s_tab->tab[i][notch] == s_tab->tab[0][notch])
			return (TRUE);
		i++;
	}
	return (FALSE);
}

t_word_tree	*create_letter_branch(char content)
{
	t_word_tree	*branch = malloc(sizeof(t_word_tree));
	branch->kids_nb = 0;
	branch->letter = content;
	return (branch);
}

void	branch_add_letter_kid(char letter, t_word_tree *branch)
{
	prepare_adding_object(branch);
	*(branch->kids) = create_letter_branch(letter);
}

//For the duration of the initialisation, kids will always point on the last sub_branch
void	add_letter_to_floor(char letter, t_word_tree *root, UINT notch)
{
	root = tree_dive(root, notch);
	branch_add_letter_kid(letter, root);
}

void	branch_add_word_end_kid(char *remainer, t_word_tree *branch)
{
	prepare_adding_object(branch);
	*(branch->kids) = remainer;
}

void	add_remainer_to_floor(t_string_tab *s_tab, UINT scanned_word_index, t_word_tree *root, UINT notch)
{
	root = tree_dive(root, notch);
	branch_add_word_end_kid(s_tab->tab[scanned_word_index] + notch, root);
	s_tab->tab[scanned_word_index] = NULL;
}

char	get_next_letter(t_string_tab *s_tab, UINT notch, UINT *scanned_word_index, t_word_tree *root)
{
	root = tree_dive(root, notch);
	if (!(*scanned_word_index) && root->kids_nb)
	{
		while (s_tab->tab[*(scanned_word_index)][notch] != root->letter)
			(*(scanned_word_index))++;
	}
	(*(scanned_word_index))++;
	while(!s_tab->tab[*scanned_word_index])
	{
		if (*scanned_word_index == s_tab->cell_number)
			return (0);
		(*(scanned_word_index))++;
	}
	return (s_tab->tab[*scanned_word_index][notch]);
}

//the given branch must not be already allocated or there will be a leak.
t_word_tree	*word_tree(t_string_tab *s_tab)
{
	t_word_tree	*root;
	UINT		notch = 0, scanned_word_index = 0;
	char		letter = s_tab->tab[0][0],
				several_candidats = s_tab->cell_number;

	root = create_letter_branch('\0');
	while (letter)
	{
		several_candidats = 0;
		while (letter)
		{
			if (several_words_of_letter(s_tab + scanned_word_index, notch))
			{
				add_letter_to_floor(letter, root, notch);
				several_candidats++;
			}
			else
				add_word_end_to_floor(s_tab, scanned_word_index, root, notch);
			letter = get_next_letter(s_tab, notch, &scanned_word_index, root);
		}
		several_candidats ? notch++ : notch--;
		scanned_word_index = 0;
		letter = get_next_letter(s_tab, notch, &scanned_word_index, root);
	}
	return (root);
}

BOOL	is_word_in_tree(const char *word, t_word_tree *root)
{
	UINT	notch = 0, i = 0, len = strlen(word);

	while (root->kids)
	{
		if (root->kids[i]->letter == word[notch])
		{
			if (notch == len)
				return TRUE;
			notch++;
			root = root->kids[i];
			i = 0;
		}
		else
		{
			i++;
			if (root->kids_nb == i)
				return FALSE;
		}
	}
	return (strcmp_n(word + notch, len - notch, (char*)root, strlen((char*)root)));
}