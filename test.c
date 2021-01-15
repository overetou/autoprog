#include "autoprog.h"

UINT	test_number;
const	char *test_section;

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

static void test_strings_eq(const char *s1, const UINT s1len, const char *s2, const UINT s2len)
{
	UINT	i;

	if (s1len != s2len)
	{
		test(FALSE);
		printf("s1 len = %u, s2 len = %u.\n", s1len, s2len);
		return ;
	}
	i = 0;
	while (i != s1len)
	{
		if (s1[i] != s2[i])
		{
			test(FALSE);
			printf("On pos %u, s1 is '%c'(val=%d) and s2 is '%c'(val=%d).\n", i, s1[i], s1[i], s2[i], s2[i]);
			return ;
		}
		i++;
	}
	test(TRUE);
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
	test_strings_eq(s + pos, len, "main", 4);
	test(next_func_call(s, &pos, &len, min_len));
	test_strings_eq(s + pos, len, "argc", 4);
	test(next_func_call(s, &pos, &len, min_len));
	test_strings_eq(s + pos, len, "char", 4);
	test(next_func_call(s, &pos, &len, min_len));
	test_strings_eq(s + pos, len, "const", 5);
	test(!next_func_call(s, &pos, &len, min_len));
	pos = 0;
	test(next_func_call(s1, &pos, &len, min_len));
	test_strings_eq(s1 + pos, len, "tenebres", 8);
}

//This is a test for the word tree builder and searcher.
int	main(void)
{
	(void)test_word_tree;
	test_next_func_call();
}