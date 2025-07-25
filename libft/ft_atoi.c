/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/30 15:05:23 by jcouto            #+#    #+#             */
/*   Updated: 2024/06/09 22:57:21 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_atoi(const char *nptr)
{
	int	minc;
	int	i;
	int	num;

	num = 0;
	minc = 1;
	i = 0;
	while ((nptr[i] >= 9 && nptr[i] <= 13) || nptr[i] == 32)
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			minc = -1;
		i++;
	}
	while (nptr[i])
	{
		if (nptr[i] >= '0' && nptr[i] <= '9')
			num = (nptr[i] - '0') + (num * 10);
		else
			return (minc * num);
		i++;
	}
	return (minc * num);
}
/*
int main(void)
{
	char *input;

      	input = "--123";
	printf("%d\n", ft_atoi(input));
	printf("%d\n", atoi(input));
	input = "-+123";
        printf("%d\n", ft_atoi(input));
        printf("%d\n", atoi(input));
        input = "+-123";
        printf("%d\n", ft_atoi(input));
        printf("%d\n", atoi(input));
        input = "++123";
        printf("%d\n", ft_atoi(input));
        printf("%d\n", atoi(input));
	return 0;
}*/
