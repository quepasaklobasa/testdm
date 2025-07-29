/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 15:35:59 by jcouto            #+#    #+#             */
/*   Updated: 2024/05/30 14:48:57 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	const char	*last;
	char		target;

	target = (char )c;
	last = NULL;
	while (*s)
	{
		if (*s == target)
			last = s;
		s++;
	}
	if (target == '\0')
		return ((char *)s);
	return ((char *)last);
}
/*
int main()
{
	char *str = "Sing it with @me";

        char *find1 = strrchr(str, i);

        char *find2 = ft_strrchr(str, i);

        printf("True strrchr: %s\n", find1);
        printf("My strrchr: %s\n", find2);

        return 0;

        char *null1 = strchr(str, F);
        if (null1 ==  NULL)
                printf("TRUE NULL");

        char *null2 = ft_strchr(str, F);
        if (null2 ==  NULL)
                printf("FT NULL");
*/
