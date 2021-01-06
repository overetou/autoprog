#ifndef CASSIUS_H
# define CASSIUS_H

# include <stdio.h>
# include <stdlib.h>

# define USAGE "Usage:\n./cassius [42] <command> [command object]\nAvailable commands:\n"\
"  tidy\nAvailable command objects:\n  prototypes\n\n[] = optionnal, <> = required."

# define BOOL char
# define UINT unsigned int
# define TRUE 1
# define FALSE 0

typedef struct	s_master
{
	BOOL 	ft;//Forty Two mode state.
	void	(*to_exec)(struct s_master*);
}				t_master;

typedef struct	s_string_tab
{
	char	*tab;
	int		cell_number;
	UINT	tab_size;
}				t_string_tab;

void tidy_prototypes(t_master *m);
void critical_test(char bool_val, const char *msg);
BOOL is_dot(const char *s, const char c);
BOOL is_alpha(const char c);
UINT file_len(int fd);
BOOL is_sep(const char c);
void strcpy_len(const char *src, char *dest, UINT len);
void 	print_string_tab(t_string_tab *protos);
UINT next_line_offset(const char *s, UINT i);

#endif