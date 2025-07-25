/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalnum.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 16:02:37 by jcouto            #+#    #+#             */
/*   Updated: 2024/05/30 14:48:11 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalnum(int c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
		|| (c >= '0' && c <= '9'))
		return (8);
	else
		return (0);
}
/*
int	main(void)
{
	int alnum;
	alnum = ',';

	printf("%d\n", ft_isalnum(alnum));
	printf("%d\n", isalnum(alnum));
}
*/
