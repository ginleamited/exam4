#ifndef VBC_H
# define VBC_H

# include <stdio.h>
# include <stdlib.h>
# include <ctype.h>
# include <unistd.h>

void	utils(int action, const char *msg, char c);
int	    parse_factor(void);
int	    handle_mult(void);
int	    handle_plus(void);

#endif