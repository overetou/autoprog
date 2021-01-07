#ifndef CASSIUS_H
# define CASSIUS_H

# include <stdio.h>
# include <stdlib.h>

# define USAGE "Usage:\n./cassius [42] <command> [command object]\nAvailable commands:\n"\
"  tidy\nAvailable command objects:\n  prototypes\n\n[] = optionnal, <> = required."

# define BOOL char
# define UCHAR unsigned char
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
	char	**tab;
	int		cell_number;
}				t_string_tab;

typedef struct	s_letter_branch
{
	char	is_letter;//should always be true
	char	letter;
	void	*sub_branchs;
	char	sub_branchs_number;
}				t_letter_branch;

typedef struct	s_word_end_branch
{
	char		is_letter;//should always be false
	const char	*word_end;
	void		*data;
}				t_word_end_branch;

void tidy_prototypes(t_master *m);
void critical_test(char bool_val, const char *msg);
BOOL is_dot(const char *s, const char c);
BOOL is_alpha(const char c);
UINT file_len(int fd);
BOOL is_sep(const char c);
void strcpy_len(const char *src, char *dest, UINT len);
void 	print_string_tab(t_string_tab *protos);
UINT next_line_offset(const char *s, UINT i);
BOOL strcmp_on_n(const char *s1, const char *s2, int n);

#endif