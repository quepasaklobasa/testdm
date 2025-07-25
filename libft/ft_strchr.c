/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/26 15:08:08 by jcouto            #+#    #+#             */
/*   Updated: 2024/06/09 21:55:15 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	unsigned int	i;

	i = 0;
	while (s[i])
	{
		if (s[i] == (char) c)
			return ((char *)&s[i]);
		i++;
	}
	if ((char) c == s[i])
		return ((char *)&s[i]);
	return (NULL);
}
/*
int main()
{
	const char *str = "Fuck this shit";

	char *result1;
       	char *result2;

	result1 =  strchr(str, 'h');
	result2 = ft_strchr(str, 'h');
	printf("True strchr: %s\n", result1);
	printf("My strchr: %s\n", result2);
 	
	result1 = strchr(str, 'z');
        result2 = ft_strchr(str, 'z');
        printf("True strchr: %s\n", result1);
        printf("My strchr: %s\n", result2);
        
	result1 = strchr(str, '\0');
        result2 = ft_strchr(str, '\0');
        printf("True strchr: %s\n", result1);
        printf("My strchr: %s\n", result2);
	return 0;
}*/
