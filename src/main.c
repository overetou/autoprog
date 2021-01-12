#include "cassius.h"

/*
1 vérifier si on est en mode 42
2 
*/

static void process_command(int argc, const char **argv, t_master *m, int len, int i)
{
	critical_test(strcmp_n(argv[i], len, "tidy", 4), "Invalid command.");
	i++;
	critical_test(i < argc, "The given command needs an object.");
	critical_test(strcmp_n(argv[i], slen(argv[i]), "prototypes", 10), "Invalid command object.");
	m->to_exec = tidy_prototypes;
}

static void process_args(int argc,	const char **argv, t_master *m)
{
	UINT len;

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
}
