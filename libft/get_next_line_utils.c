/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 17:24:35 by jcouto            #+#    #+#             */
/*   Updated: 2024/09/18 16:31:02 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_g_strchr(const char *s, int c)
{
	unsigned int	i;

	if (!s)
		return (NULL);
	i = 0;
	while (s[i])
	{
		if (s[i] == (char) c)
			return ((char *)&s[i]);
		i++;
	}
	if ((char) c == s[i])
	{
		return ((char *)&s[i]);
	}
	return (NULL);
}

size_t	ft_gstrlen(const char *s)
{
	unsigned int	i;

	if (!s)
		return (0);
	i = 0;
	while (*s != '\0')
	{
		s++;
		i++;
	}
	return (i);
}

char	*ft_g_strjoin(char *s1, char *s2)
{
	size_t	i;
	size_t	a;
	char	*n;

	if (!s1)
	{
		s1 = (char *)malloc(1 * sizeof(char));
		s1[0] = '\0';
	}
	if (!s2)
		return (NULL);
	n = (char *)malloc((ft_gstrlen(s1) + ft_gstrlen(s2) + 2) * sizeof(char));
	if (!n)
		return (NULL);
	i = -1;
	a = 0;
	if (s1)
		while (s1[++i] != '\0')
			n[i] = s1[i];
	while (s2[a] != '\0')
		n[i++] = s2[a++];
	n[ft_gstrlen(s1) + ft_gstrlen(s2)] = '\0';
	free(s1);
	return (n);
}
