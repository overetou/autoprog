#include "autoprog.h"

UINT	test_number;
const	char *test_section;

static void	test_exit()
{
	puts("Critical test failed. Stopping now.\n");
	exit(0);
}

static void	change_test_section(const char *new_section)
{
	test_section = new_section;
	test_number = 1;
}

static void	test(const BOOL success)
{
	if (success)
		puts("Ok");
	else
		printf("In section [%s], test %u failed.\n", test_section, test_number);
	test_number++;
}

static BOOL test_strings_eq(const char *s1, const UINT s1len, const char *s2, const UINT s2len)
{
	UINT	i;

	if (s1len != s2len)
	{
		test(FALSE);
		printf("s1 len = %u, s2 len = %u.\n", s1len, s2len);
		return TRUE;
	}
	i = 0;
	while (i != s1len)
	{
		if (s1[i] != s2[i])
		{
			test(FALSE);
			printf("On pos %u, s1 is '%c'(val=%d) and s2 is '%c'(val=%d).\n", i, s1[i], s1[i], s2[i], s2[i]);
			return TRUE;
		}
		i++;
	}
	test(TRUE);
	return FALSE;
}

static BOOL	test_uint_eq(const int u1, const int u2)
{
	if (u1 == u2)
	{
		test(TRUE);
		return FALSE;
	}
	test(FALSE);
	printf("Tested numbers did not match. %u != %u.\n", u1, u2);
	return(TRUE);
}

static void	test_word_tree()
{
	change_test_section("Word Tree");
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
	test(is_word_in_tree("tintouin", sizeof("tintouin") - 1, tree));
	test(!is_word_in_tree("bertouin", sizeof("bertouin") - 1, tree));
	test(is_word_in_tree("test", sizeof("test") - 1, tree));
	test(is_word_in_tree("bernard", sizeof("bernard") - 1, tree));
	test(is_word_in_tree("gorille", sizeof("gorille") - 1, tree));
	test(is_word_in_tree("benzema", sizeof("benzema") - 1, tree));
	test(!is_word_in_tree("bernar", sizeof("bernar") - 1, tree));
	test(!is_word_in_tree("bernardo", sizeof("bernardo") - 1, tree));
	test(is_word_in_tree("ber", sizeof("ber") - 1, tree));
	test(is_word_in_tree("ti", sizeof("ti") - 1, tree));
	test(!is_word_in_tree("be", sizeof("be") - 1, tree));
	is_word_in_tree("", 0, tree);
}

/* "int main(int argc, char const *argv[])\n"
	"{\n"
	"	t_master m;\n"
	"	process_args(argc, argv, &m);\n"
	"	m.to_exec(&m);\n"
	"	puts(\"\nSuccess.\");\n"
	"	return 0;\n"
	"}"; */

BOOL	next_func_call(const char *s, UINT *pos, UINT *len, const UINT min_len);

static void	test_next_func_call()
{
	change_test_section("Next Func call");
	const char	*s = "int main(int argc, char const *argv[])\n";
	const char	*s1 = "\t\ti = i + 2 * (tenebres / caris(5))";
	UINT	pos = 0, len, min_len = 4;

	test(next_func_call(s, &pos, &len, min_len));
	test_strings_eq(s + pos, len, "main", 4);pos += len;
	test(next_func_call(s, &pos, &len, min_len));
	test_strings_eq(s + pos, len, "argc", 4);pos += len;
	test(next_func_call(s, &pos, &len, min_len));
	test_strings_eq(s + pos, len, "char", 4);pos += len;
	test(next_func_call(s, &pos, &len, min_len));
	test_strings_eq(s + pos, len, "const", 5);pos += len;
	test(!next_func_call(s, &pos, &len, min_len));pos = 0;
	test(next_func_call(s1, &pos, &len, min_len));
	test_strings_eq(s1 + pos, len, "tenebres", 8);
}

void	add_extrafile_funcs(const char *file_name, UINT **interfile_funcs, UINT *interfile_func_nb, const UINT *file_limits, t_word_tree *tree, const UINT shortest_len);

static void	test_add_extrafile_funcs()
{
	change_test_section("Add Extra Funcs");
	t_string_tab *tab = new_string_tab(9);
	t_word_tree *tree;
	UINT	*interfile_funcs = NULL, interfile_func_nb = 0, file_limits[] = {3, 6};

	tab->tab[0] = new_string("main");
	tab->tab[1] = new_string("launch_flowers");
	tab->tab[2] = new_string("func");
	tab->tab[3] = new_string("count_flowers");
	tab->tab[4] = new_string("bernard");
	tab->tab[5] = new_string("berni");
	tab->tab[6] = new_string("gorille");
	tab->tab[7] = new_string("benzema");
	tab->tab[8] = new_string("ber");
	
	test((tree = word_tree(tab)) != NULL);
	free_string_tab(tab);
	add_extrafile_funcs("tests/matching_test.c", &interfile_funcs, &interfile_func_nb, file_limits, tree, 3);
	if (test_uint_eq(interfile_func_nb, 2)) test_exit();
	test_uint_eq(interfile_funcs[0], 0);
	test_uint_eq(interfile_funcs[1], 1);
	free(interfile_funcs);
	free_word_tree(tree);
}

static void test_delete_tree_end()
{
	change_test_section("Delete tree end");
	t_string_tab *tab = new_string_tab(9);
	t_word_tree *tree, *branch;
	UINT	remainer_pos;

	tab->tab[0] = new_string("main");
	tab->tab[1] = new_string("launch_flowers");
	tab->tab[2] = new_string("func");
	tab->tab[3] = new_string("count_flowers");
	tab->tab[4] = new_string("bernard");
	tab->tab[5] = new_string("berni");
	tab->tab[6] = new_string("gorille");
	tab->tab[7] = new_string("benzema");
	tab->tab[8] = new_string("ber");

	tree = word_tree(tab);
	free_string_tab(tab);
	branch = get_word_info_from_tree("main", sizeof("main") - 1, tree, &remainer_pos);
	delete_tree_end(branch, remainer_pos);
	branch = get_word_info_from_tree("launch_flowers", sizeof("launch_flowers") - 1, tree, &remainer_pos);
	delete_tree_end(branch, remainer_pos);
	branch = get_word_info_from_tree("func", sizeof("func") - 1, tree, &remainer_pos);
	delete_tree_end(branch, remainer_pos);
	branch = get_word_info_from_tree("count_flowers", sizeof("count_flowers") - 1, tree, &remainer_pos);
	delete_tree_end(branch, remainer_pos);
	branch = get_word_info_from_tree("bernard", sizeof("bernard") - 1, tree, &remainer_pos);
	delete_tree_end(branch, remainer_pos);
	branch = get_word_info_from_tree("berni", sizeof("berni") - 1, tree, &remainer_pos);
	delete_tree_end(branch, remainer_pos);
	branch = get_word_info_from_tree("gorille", sizeof("gorille") - 1, tree, &remainer_pos);
	delete_tree_end(branch, remainer_pos);
	branch = get_word_info_from_tree("benzema", sizeof("benzema") - 1, tree, &remainer_pos);
	delete_tree_end(branch, remainer_pos);
	branch = get_word_info_from_tree("ber", sizeof("ber") - 1, tree, &remainer_pos);
	delete_tree_end(branch, remainer_pos);
	test(TRUE);
}

static void test_free_word_tree()
{
	change_test_section("Free Word tree");
	t_string_tab *tab = new_string_tab(1);
	t_word_tree *tree;

	tab->tab[0] = new_string("main");
	tree = word_tree(tab);
	free_string_tab(tab);
	free_word_tree(tree);
	test(TRUE);
}

//This is a test for the word tree builder and searcher.
int	main(void)
{
	(void)test_word_tree;
	(void)test_next_func_call;
	(void)test_free_word_tree;
	test_delete_tree_end();
	(void)test_add_extrafile_funcs;
}