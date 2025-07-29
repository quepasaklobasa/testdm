/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/27 12:47:09 by jcouto            #+#    #+#             */
/*   Updated: 2024/06/09 20:22:16 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	unsigned int	i;

	if (n == 0)
		return (0);
	i = 0;
	while ((s1[i] || s2[i]) && i < n)
	{
		if (s1[i] != s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return (0);
}
/*
int main()
{
	int num;
	num = ft_strncmp("test", "testss", 7);
	printf("%d\n", num);
        num = strncmp("test", "testss", 7);
        printf("%d\n", num);
        num = ft_strncmp("", "test", 4);
        printf("%d\n", num);
        num = strncmp("", "test", 4);
        printf("%d\n", num);
	return 0;
}*/
