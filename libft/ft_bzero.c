/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 15:10:15 by jcouto            #+#    #+#             */
/*   Updated: 2024/05/30 14:48:18 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_bzero(void *s, size_t n)
{
	unsigned char	*p;

	p = (unsigned char *)s;
	while (n-- > 0)
	{
		*p++ = 0;
	}
}
/*
int     main()
{
        char str[10] = "abcdefghi";
    printf("Before string: %s\n", str);
    ft_bzero(str, 5);
    printf("Nuestro bzero: %s\n", str);
    strcpy(str, "abcdefghi");
    bzero(str, 5);
    printf("True bzero: %s\n", str);
        return (0);
}*/
