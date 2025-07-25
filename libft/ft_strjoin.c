/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 14:21:51 by jcouto            #+#    #+#             */
/*   Updated: 2024/06/05 14:39:50 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	i;
	size_t	a;
	char	*new;

	i = 0;
	a = 0;
	new = (char *)malloc((ft_strlen(s1) + ft_strlen(s2) + 1) * sizeof(char));
	if (new == NULL)
		return (NULL);
	while (s1[i])
	{
		new[a] = s1[i];
		a++;
		i++;
	}
	i = 0;
	while (s2[i])
	{
		new[a] = s2[i];
		a++;
		i++;
	}
	new[a] = '\0';
	return (new);
}
