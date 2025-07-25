/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_puthex.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/21 17:23:38 by jcouto            #+#    #+#             */
/*   Updated: 2024/09/12 15:38:37 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_puthexu(unsigned int nb)
{
	int		i;
	char	*hex;

	i = 0;
	hex = "0123456789ABCDEF";
	if (nb >= 16)
		i += ft_puthexu(nb / 16);
	i += ft_putchar(hex[nb % 16]);
	return (i);
}

int	ft_puthexl(unsigned int nb)
{
	int		i;
	char	*hex;

	i = 0;
	hex = "0123456789abcdef";
	if (nb >= 16)
		i += ft_puthexl(nb / 16);
	i += ft_putchar(hex[nb % 16]);
	return (i);
}

int	ft_pointer(unsigned long ptr)
{
	int		i;

	i = 2;
	if (ptr == 0)
	{
		write(1, "(nil)", 5);
		return (5);
	}
	ft_putchar('0');
	ft_putchar('x');
	i += ft_puthexptr(ptr);
	return (i);
}

int	ft_puthexptr(unsigned long nb)
{
	int		i;
	char	*hex;

	hex = "0123456789abcdef";
	i = 0;
	if (nb >= 16)
		i += ft_puthexptr(nb / 16);
	i += ft_putchar(hex[nb % 16]);
	return (i);
}
