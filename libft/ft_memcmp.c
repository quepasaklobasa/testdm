/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 15:03:05 by jcouto            #+#    #+#             */
/*   Updated: 2024/06/01 15:08:57 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	size_t				i;
	const unsigned char	*ptr1;
	const unsigned char	*ptr2;

	ptr1 = (const unsigned char *)s1;
	ptr2 = (const unsigned char *)s2;
	if (n == 0)
		return (0);
	i = 0;
	while (i < n)
	{
		if (ptr1[i] != ptr2[i])
			return (ptr1[i] - ptr2[i]);
		i++;
	}
	return (0);
}
/*
int main()
{
    char st1[15] = "abcdef";
    char st2[15] = "abcDEF";
    int mm;
    int tm;

    mm = ft_memcmp(st1, st2, 6);
    tm = memcmp(st1, st2, 6);

    printf("my memcmp str2 is less than str1 by %d", mm);
    printf("true memcpm str2 is less than str1 by %d", tm);

    return (0);
}*/
