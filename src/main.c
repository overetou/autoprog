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
	puts(msg);
	exit(0);
}

//returns true if both strings are the same.
char strcmp_n(const char *s1, int s1_size, const char *s2, int s2_size)
{
	int i;

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

static void process_command(int argc, const char **argv, t_master *m, int len, int i)
{
	critical_test(strcmp_n(argv[i], len, "tidy", 4), "Invalid command." USAGE);
	i++;
	critical_test(i < argc, "The given command needs an object.");
	critical_test(strcmp_n(argv[i], strlen(argv[i]), "prototypes", 10), "Invalid command object." USAGE);
	m->to_exec = tidy_prototypes;
}

static void process_args(int argc,	const char **argv, t_master *m)
{
	int len;

	critical_test(argc >= 2 && argc <= 4, "Invalid argument number." USAGE);
	len = strlen(argv[1]);
	if (strcmp_n(argv[1], len, "42", 2))
	{
		puts("42 mode activated.");
		m->ft = TRUE;
		critical_test(argc < 3, "No command were given." USAGE);
		process_command(argc, argv, m, strlen(argv[2]), 2);
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
	puts("Success.");
	return 0;
}
