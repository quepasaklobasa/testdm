/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 18:37:23 by jcouto            #+#    #+#             */
/*   Updated: 2024/06/10 14:45:00 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	counter(int num)
{
	size_t	count;

	count = 0;
	if (num <= 0)
		count = 1;
	while (num != 0)
	{
		num = num / 10;
		count++;
	}
	return (count);
}

char	*ft_itoa(int n)
{
	size_t	count;
	char	*str;
	long	num;

	num = n;
	count = counter(n);
	str = (char *) malloc((count + 1) * sizeof(char));
	if (str == NULL)
		return (NULL);
	str[count] = '\0';
	if (n < 0)
		num = -num;
	while (0 < count)
	{
		count--;
		if (n < 0 && count == 0)
		{
			str[0] = '-';
			break ;
		}
		str[count] = (num % 10) + '0';
		num = num / 10;
	}
	return (str);
}
/*
int main()
{
	char *il;

	il = ft_itoa(-1234);
	if (il)
	{
		printf("Mi itoa: %s\n", il);
		free(il);
	}
	else
	{
		printf("Memory allocation fallo boludo");
	}
	il = ft_itoa(-2147483648);
	if (il)
	{
		printf("Result: %s\n", il);
		free(il);
	}
	else
	{
		printf("Fallo la allocacion de memoria xd");
	}
	return (0);
}*/
