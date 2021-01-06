#ifndef CASSIUS_H
# define CASSIUS_H

# include <stdio.h>
# include <stdlib.h>

# define USAGE "Usage:\n./cassius [42] <command> [command object]\nAvailable commands:\n"\
"  tidy\nAvailable command objects:\n  prototypes\n\n[] = optionnal, <> = required."

# define BOOL char
# define TRUE 1
# define FALSE 0

typedef struct	s_master
{
	BOOL 	ft;//Forty Two mode state.
	void	(*to_exec)(struct s_master*);
}				t_master;

void tidy_prototypes(t_master *m);
void critical_test(char bool_val, const char *msg);

#endif