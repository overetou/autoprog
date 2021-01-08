#include "cassius.h"
#include <string.h>
#include <unistd.h>

static t_word_tree *tree_dive(t_word_tree *root, UINT notch)
{
	UINT	floor = 0;

	while (floor++ != notch)
	{
		root = *(root->kids);
		printf("On the floor of letter: %c.\n", root->letter);
	}
	return (root);
}

static void	prepare_adding_object(t_word_tree *branch)
{
	if (branch->kids_nb)
	{
		printf("Already %u kids registered.\n", branch->kids_nb);
		branch->kids = realloc(branch->kids - branch->kids_nb + 1, sizeof(void*) * branch->kids_nb);
		if (!(branch->kids))
		{
			puts("prepare_adding_object: realloc failed.");
			exit(0);
		}
		branch->kids += branch->kids_nb;
		(branch->kids_nb)++;
	}
	else
	{
		puts("First kid to be registered.");
		branch->kids = malloc(sizeof(void*));
		branch->kids_nb = 1;
	}
}

static BOOL	several_words_of_letter(t_string_tab *s_tab, UINT notch)
{
	UINT i = 1;

	while (i != s_tab->cell_number)
	{
		if (s_tab->tab[i][notch] == s_tab->tab[0][notch])
			return (TRUE);
		i++;
	}
	return (FALSE);
}

static t_word_tree	*create_letter_branch(char content)
{
	t_word_tree	*branch = malloc(sizeof(t_word_tree));
	branch->kids_nb = 0;
	branch->letter = content;
	return (branch);
}

static void	branch_add_letter_kid(char letter, t_word_tree *branch)
{
	prepare_adding_object(branch);
	*(branch->kids) = create_letter_branch(letter);
}

//For the duration of the initialisation, kids will always point on the last sub_branch
static void	add_letter_to_floor(char letter, t_word_tree *root, UINT notch)
{
	root = tree_dive(root, notch);
	branch_add_letter_kid(letter, root);
}

static void	branch_add_word_end_kid(char *remainer, t_word_tree *branch)
{
	prepare_adding_object(branch);
	*(branch->kids) = malloc(sizeof(t_remainer));
	((t_remainer*)(*(branch->kids)))->fake_letter = 0;
	((t_remainer*)(*(branch->kids)))->remainer = remainer;
}

static void	add_remainer_to_floor(t_string_tab *s_tab, UINT scanned_word_index, t_word_tree *root, UINT notch)
{
	printf("\nNo concurrent word for remainder: %s.\n", (s_tab->tab[scanned_word_index]) + notch);
	root = tree_dive(root, notch);
	branch_add_word_end_kid(s_tab->tab[scanned_word_index] + notch, root);
	s_tab->tab[scanned_word_index] = NULL;
}

static char	get_next_letter(t_string_tab *s_tab, UINT notch, UINT *scanned_word_index, t_word_tree *root)
{
	root = tree_dive(root, notch);
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
				printf("\nSeveral words of letter: %c.\n", letter);
				add_letter_to_floor(letter, root, notch);
				several_candidats++;
			}
			else
			{
				add_remainer_to_floor(s_tab, scanned_word_index, root, notch);
				printf("added remainder. s_tab->tab[scanned_word_index] is now NULL: %s.\n", s_tab->tab[scanned_word_index] == NULL ? "TRUE" : "FALSE");
				if (s_tab->tab[scanned_word_index])
					exit(0);
			}
			letter = get_next_letter(s_tab, notch, &scanned_word_index, root);
			printf("letter updated. scanned_word_index = %u. Letter = %c.\n", scanned_word_index, (letter ? letter : '0'));
		}
		printf("No more letter on floor: %u. Going %s one floor.\n", notch, (several_candidats ? "down" : "up"));
		several_candidats ? notch++ : notch--;
		scanned_word_index = 0;
		letter = get_next_letter(s_tab, notch, &scanned_word_index, root);
	}
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