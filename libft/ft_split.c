/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 17:43:20 by jcouto            #+#    #+#             */
/*   Updated: 2024/09/18 16:21:47 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	ft_substr_count(char const *s, char c)
{
	size_t	res;

	res = 0;
	while (*s)
	{
		if (*s != c)
		{
			res++;
			while (*s && *s != c)
				s++;
		}
		else
			s++;
	}
	return (res);
}

static char	**ft_destroy_str_arr(char **s, int i)
{
	while (i-- >= 0 && s[i])
	{
		free(s[i]);
		s[i] = NULL;
	}
	free(s);
	s = NULL;
	return (NULL);
}

char	**ft_split(char const *s, char c)
{
	int		i;
	char	*from;
	char	**buf;

	i = 0;
	buf = malloc((ft_substr_count(s, c) + 1) * sizeof(char *));
	if (!s || !buf)
		return (NULL);
	while (*s)
	{
		if (*s != c)
		{
			from = (char *)s;
			while (*s && *s != c)
				s++;
			buf[i++] = ft_substr(from, 0, (s - from));
			if (!buf)
				return (ft_destroy_str_arr(buf, i));
		}
		else
			s++;
	}
	buf[i] = NULL;
	return (buf);
}
/*
int main()
{
    char **tabstr;
    int i = 0;

    if (!(tabstr = ft_split("lorem ipsum dolor sit amet, 
			consectetur adipiscing elit. Sed non risus. Suspendisse", ' ')))
	ft_putstr_fd("NULL\n", 1);
    else
    {
        while (tabstr[i] != NULL)
        {
            ft_putstr_fd(tabstr[i], 1);
            ft_putstr_fd("\n", 1);
            i++;
        }
    }
    return (0);
}*/