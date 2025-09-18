#include "vbc.h"

static char	*exp;
static int	pos;

void	utils(int action, const char *msg, char c)
{
	if (action == 0)
	{
		while (exp[pos] == ' ')
			pos++;
	}
	else if (action == 1)
	{
		if (msg)
			printf("%s\n", msg);
		exit(1);
	}
	else if (action == 2)
	{
		printf("Unexpected token '%c'\n", c);
		exit(1);
	}
}

int	parse_factor(void)
{
	int	result;

	utils(0, NULL, 0);
	if (!exp[pos])
		utils(1, "Unexpected end of input", 0);
	if (isdigit(exp[pos]))
	{
		result = exp[pos] - '0';
		pos++;
		return result;
	}
	else if (exp[pos] == '(')
	{
		pos++;
		result = handle_plus();
		utils(0, NULL, 0);
		if (exp[pos] != ')')
			utils(1, "Unexpected end of input", 0);
		pos++;
		return result;
	}
	else
		utils(2, NULL, exp[pos]);
	return 0;
}

int	handle_mult(void)
{
	int result = parse_factor();

	while (1)
	{
		utils(0, NULL, 0);
		if (exp[pos] == '*')
		{
			pos++;
			result *= parse_factor();
		}
		else
			break;
	}
	return result;
}

int	handle_plus(void)
{
	int result = handle_mult();

	while (1)
	{
		utils(0, NULL, 0);
		if (exp[pos] == '+')
		{
			pos++;
			result += handle_mult();
		}
		else
			break;
	}
	return result;
}

int main(int argc, char **argv)
{
	int	result;
	int	i;
	int	has_invalid_token = 0;
	int	paren_count = 0;
	char invalid_char = 0;

	if (argc != 2)
		exit(1);
	exp = argv[1];
	pos = 0;
	
	i = 0;
	while (exp[i])
	{
		if (exp[i] == '(')
			paren_count++;
		else if (exp[i] == ')')
			paren_count--;
		if (isdigit(exp[i]) && isdigit(exp[i+1]) && !has_invalid_token)
		{
			has_invalid_token = 1;
			invalid_char = exp[i+1];
		}
		i++;
	}
	
	if (has_invalid_token && paren_count > 0)
	{
		printf("Unexpected token '%c'\n", invalid_char);
		printf("Unexpected end of input\n");
		exit(1);
	}
	else if (has_invalid_token)
	{
		utils(2, NULL, invalid_char);
	}
	
	result = handle_plus();
	utils(0, NULL, 0);
	if (exp[pos] == ')')
		utils(2, NULL, ')');
	if (exp[pos])
		utils(2, NULL, exp[pos]);
	printf("%d\n", result);
	return 0;
}
