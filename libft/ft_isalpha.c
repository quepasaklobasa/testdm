/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 14:28:37 by jcouto            #+#    #+#             */
/*   Updated: 2024/05/30 14:48:54 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalpha(int c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
	{
		return (1024);
	}
	else
	{
		return (0);
	}
}
/*
int     main(void)
{
        int alpha;
        alpha = '\n';
        printf("%d\n", ft_isalpha(alpha));
        printf("%d\n", isalpha(alpha));
        
}*/
