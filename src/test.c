#include "cassius.h"

//This is a test for the word tree builder and searcher.
int	main(void)
{
	t_string_tab *tab = new_string_tab(9);
	t_word_tree *tree;

	tab->tab[0] = new_string("ti");
	tab->tab[1] = new_string("test");
	tab->tab[2] = new_string("mangekyou");
	tab->tab[3] = new_string("tintouin");
	tab->tab[4] = new_string("bernard");
	tab->tab[5] = new_string("berni");
	tab->tab[6] = new_string("gorille");
	tab->tab[7] = new_string("benzema");
	tab->tab[8] = new_string("ber");

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
	puts(is_word_in_tree("ber", sizeof("ber") - 1, tree) ? "Ok" : "Failure");
	puts(is_word_in_tree("ti", sizeof("ti") - 1, tree) ? "Ok" : "Failure");
	puts(!is_word_in_tree("be", sizeof("be") - 1, tree) ? "Ok" : "Failure");
	is_word_in_tree("", 0, tree);
	puts("Test succeeded.");
}