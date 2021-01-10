#include "cassius.h"

/*
1 vérifier si on est en mode 42
2 
*/

//The result must be true else the given message is displayed and the program ends.
void critical_test(char bool_val, const char *msg)
{
	if (bool_val)
		return;
	puts("\nError:");
	puts(msg);
	exit(0);
}

//returns true if both strings are the same.
BOOL strcmp_n(const char *s1, int s1_size, const char *s2, int s2_size)
{
	int i;

	printf("Comparing %s of size %u and %s of size %u.\n", s1, s1_size, s2, s2_size);
	if (s1_size != s2_size)
		return (0);
	i = 0;
	while (i != s1_size)
	{
		if (s1[i] != s2[i])
			return (0);
		i++;
	}
	return (1);
}

int slen(const char *s)
{
	int i = 0;

	while (s[i])
		i++;
	return (i);
}

/* static void process_command(int argc, const char **argv, t_master *m, int len, int i)
{
	critical_test(strcmp_n(argv[i], len, "tidy", 4), "Invalid command.");
	i++;
	critical_test(i < argc, "The given command needs an object.");
	critical_test(strcmp_n(argv[i], slen(argv[i]), "prototypes", 10), "Invalid command object.");
	m->to_exec = tidy_prototypes;
}

static void process_args(int argc,	const char **argv, t_master *m)
{
	int len;

	if (argc < 2 || argc > 4)
	{
		puts(USAGE);
		exit(0);
	}
	len = slen(argv[1]);
	if (strcmp_n(argv[1], len, "help", 4))
	{
		puts(USAGE);
		exit(0);
	}
	len = slen(argv[1]);
	if (strcmp_n(argv[1], len, "42", 2))
	{
		puts("42 - activated");
		m->ft = TRUE;
		critical_test(argc > 2, "No command was given.");
		process_command(argc, argv, m, slen(argv[2]), 2);
	}
	else
	{
		m->ft = FALSE;
		process_command(argc, argv, m, len, 1);
	}
}

int main(int argc, char const *argv[])
{
	t_master m;

	process_args(argc, argv, &m);
	m.to_exec(&m);
	puts("\nSuccess.");
	return 0;
} */

char *new_string(const char *s)
{
	int len = slen(s);
	char *alloc;

	alloc = malloc(len + 1);
	strcpy_len(s, alloc, len);
	alloc[len] = '\0';
	return (alloc);
}

t_string_tab	*new_string_tab(UINT cell_number)
{
	t_string_tab *t = malloc(sizeof(t_string_tab) + cell_number * sizeof(char*));
	t->tab = ((void*)t) + sizeof(t_string_tab);
	t->cell_number = cell_number;
	return (t);
}

void			free_string_tab(t_string_tab *to_free)
{
	UINT	i = 0;

	while (i != to_free->cell_number)
	{
		free(to_free->tab[i]);
		i++;
	}
	free(to_free);
}

int	main(void)
{
	t_string_tab *tab = new_string_tab(7);
	t_word_tree *tree;

	tab->tab[0] = new_string("test");
	tab->tab[1] = new_string("mangekyou");
	tab->tab[2] = new_string("tintouin");
	tab->tab[3] = new_string("bernard");
	tab->tab[4] = new_string("berni");
	tab->tab[5] = new_string("gorille");
	tab->tab[6] = new_string("benzema");

	puts("Trying to build the tree.");
	tree = word_tree(tab);

	puts("Tree built. Beginning search test.");
	puts(is_word_in_tree("tintouin", sizeof("tintouin") - 1, tree) ? "Ok" : "Failure");
	puts(!is_word_in_tree("bertouin", sizeof("bertouin") - 1, tree) ? "Ok" : "Failure");
	puts(is_word_in_tree("test", sizeof("test") - 1, tree) ? "Ok" : "Failure");
	puts(is_word_in_tree("bernard", sizeof("bernard") - 1, tree) ? "Ok" : "Failure");
	puts(is_word_in_tree("gorille", sizeof("gorille") - 1, tree) ? "Ok" : "Failure");
	puts(is_word_in_tree("benzema", sizeof("benzema") - 1, tree) ? "Ok" : "Failure");
	puts(!is_word_in_tree("bernar", sizeof("bernar") - 1, tree) ? "Ok" : "Failure");
	puts(!is_word_in_tree("bernardo", sizeof("bernardo") - 1, tree) ? "Ok" : "Failure");
	puts(!is_word_in_tree("ber", sizeof("ber") - 1, tree) ? "Ok" : "Failure");
	is_word_in_tree("", 0, tree);
	puts("Test succeeded.");
}
