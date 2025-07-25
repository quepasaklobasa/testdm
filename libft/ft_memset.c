/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 14:45:10 by jcouto            #+#    #+#             */
/*   Updated: 2024/05/30 14:48:42 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*p;

	p = (unsigned char *)s;
	while (n > 0)
	{
		*p = (unsigned char)c;
		p++;
		n--;
	}
	return (s);
}
/*
int	main()
{
	char str[10] = "abcdefghi";
    printf("Before memset: %s\n", str);
    ft_memset(str, 'a', 5);
    printf("Nuestro memset: %s\n", str);
    strcpy(str, "abcdefghi");
    memset(str, 'a', 5);
    printf("True memset: %s\n", str);
	return (0);
}*/
