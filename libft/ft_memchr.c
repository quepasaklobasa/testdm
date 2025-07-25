/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 15:12:50 by jcouto            #+#    #+#             */
/*   Updated: 2024/06/01 15:13:23 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t				i;
	const unsigned char	*str;
	unsigned char		uc;

	str = (const unsigned char *) s;
	uc = (unsigned char) c;
	i = 0;
	while (i < n)
	{
		if (str[i] == uc)
			return ((void *)(str + i));
		i++;
	}
	return (NULL);
}
/*
int main(void)
{
	char data[] = {'q', 'r', 's', 't', 'u', 'v', 'w', 'x'};

	char *pos = memchr(data, 't', 7);
	char *sit = ft_memchr(data, 't', 7);

	printf("pos[0] = %c\n", pos[0]);
	printf("pos[1] = %c\n", pos[1]);

	printf("sit[0] = %c\n", sit[0]);
	printf("sit[1] = %c\n", sit[1]);

	return 0;
}*/
