/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/10 17:00:43 by jcouto            #+#    #+#             */
/*   Updated: 2024/06/11 12:13:18 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*sub;
	size_t	real_len;
	size_t	str_len;
	size_t	i;

	if (!s)
		return (NULL);
	str_len = ft_strlen(s);
	if (start >= str_len)
		return (ft_strdup(""));
	real_len = str_len - start;
	if (len > real_len)
		len = real_len;
	sub = (char *)malloc((len + 1) * sizeof(char));
	if (!sub)
		return (NULL);
	i = 0;
	while (s[(unsigned int)i + start] && i < len)
	{
		sub[i] = s[i + start];
		i++;
	}
	sub[len] = '\0';
	return (sub);
}
/*
int main()
{
    const char *str = "Hola";
    char *find;

    find = ft_substr(str, 3, 1);
    if (find) {
        printf("Substring: %s\n", find);
        free(find);
    } else {
        printf("Memory allocation failed\n");
    }
//gcc ft_strlen.c ft_strdup.c ft_memcpy.c ft_substr.c -o ft_substr
    return 0;
}*/
