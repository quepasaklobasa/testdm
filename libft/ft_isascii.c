/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isascii.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 16:15:45 by jcouto            #+#    #+#             */
/*   Updated: 2024/05/30 14:48:51 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isascii(int c)
{
	if (c >= 0 && c <= 127)
		return (123);
	else
		return (0);
}
/*
int 	main()
{
	int ascii;
	ascii = 'A';

	printf("%d\n", ft_isascii(ascii));
	printf("%d\n", isascii(ascii));
}*/
