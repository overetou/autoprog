#ifndef AUTOPROG_H
# define AUTOPROG_H

# include <stdio.h>
# include <stdlib.h>

# define USAGE "Usage:\n./autoprog [42] <command> [command object]\nAvailable commands:\n"\
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
	UINT	cell_number;
}				t_string_tab;

typedef struct	s_word_tree
{
	char	letter;
	void	**kids;
	UCHAR	kids_nb;
	struct	s_word_tree	*parent;
}				t_word_tree;

typedef	struct	s_remainer
{
	char	fake_letter;//must always be 0
	char	*remainer;
	UINT	len;
	UINT	pos;
}				t_remainer;

typedef struct	s_floor_data
{
	t_string_tab		*s_tab;
	UINT				pos;
	t_word_tree			*parent_branch;
	struct s_floor_data	*next;
}				t_floor_data;

void critical_test(char bool_val, const char *msg);
BOOL strcmp_n(const char *s1, int s1_size, const char *s2, int s2_size);
UINT slen(const char *s);
void tidy_prototypes(t_master *m);
UINT next_line_offset(const char *s, UINT i);
void strcpy_len(const char *src, char *dest, UINT len);
t_string_tab	*new_string_tab(UINT cell_number);
void 	print_string_tab(t_string_tab *protos);
UINT	get_chunk_len(const char *s, BOOL(*func)(const char));
UINT	get_sep_len(const char *s);
BOOL is_word_material(const char c);
t_word_tree	*word_tree(t_string_tab *s_tab);
void	free_string_tab(t_string_tab *to_free);
BOOL is_digit(const char c);
BOOL is_maj(const char c);
UINT	skip_brackets(const char *s, UINT pos);
BOOL is_sep(const char c);
t_word_tree	*get_word_info_from_tree(const char *word, UCHAR word_len, t_word_tree *root, UINT *remainer_pos);
void	delete_tree_end(t_word_tree *parent_branch, UINT remainer_pos);
BOOL is_dot(const char *s, const char c);
char *new_string(const char *s);
UINT get_dir_files_number(void);
BOOL strcmp_on_n(const char *s1, const char *s2, int n);
UINT file_len(int fd);
BOOL is_type_material(const char c);
void	free_word_tree(t_word_tree *tree);
void	store_proto_names(const char *file_name, t_string_tab *protos);
BOOL	is_word_in_tree(const char *word, UCHAR word_len, t_word_tree *root);

#endif
