#ifndef STRS_H
# define STRS_H

void	split_free(char **split);
int		split_len(char **split);
char	**add_str(char **str, char *add);
char	**merge_strs(char **orig, char **new_strs);

#endif
