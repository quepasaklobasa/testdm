/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_putnbrs.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/21 14:37:20 by jcouto            #+#    #+#             */
/*   Updated: 2024/09/18 16:45:50 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_putnbr(int nb)
{
	int	count;

	count = 0;
	if (nb == -2147483648)
	{
		count += ft_putchar('-');
		count += ft_putchar('2');
		nb = 147483648;
	}
	if (nb < 0)
	{
		count += ft_putchar('-');
		nb = -nb;
	}
	if (nb >= 10)
	{
		count += ft_putnbr(nb / 10);
		nb = nb % 10;
	}
	if (nb < 10)
		count += ft_putchar(nb + '0');
	return (count);
}

size_t	ft_putunbr(unsigned int nb)
{
	int	count;

	count = 0;
	if (nb == 4294967295)
	{
		count += ft_putchar('4');
		nb = 294967295;
	}
	if (nb >= 10)
	{
		count += ft_putunbr(nb / 10);
		nb = nb % 10;
	}
	if (nb < 10)
		count += ft_putchar(nb + '0');
	return (count);
}
