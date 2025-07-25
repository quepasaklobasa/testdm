/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/18 15:55:26 by jcouto            #+#    #+#             */
/*   Updated: 2024/09/18 16:46:38 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	handle_format(char c, va_list ap)
{
	if (c == 'c')
		return (ft_putchar(va_arg(ap, int)));
	else if (c == 's')
		return (ft_putstr(va_arg(ap, char *)));
	else if (c == 'p')
		return (ft_pointer(va_arg(ap, unsigned long)));
	else if (c == 'd' || c == 'i')
		return (ft_putnbr(va_arg(ap, int)));
	else if (c == 'u')
		return (ft_putunbr(va_arg(ap, unsigned int)));
	else if (c == 'x')
		return (ft_puthexl(va_arg(ap, unsigned int)));
	else if (c == 'X')
		return (ft_puthexu(va_arg(ap, unsigned int)));
	else if (c == '%')
		return (ft_putchar('%'));
	return (0);
}

int	ft_printf(const char *fmt, ...)
{
	va_list	ap;
	int		count;

	count = 0;
	va_start(ap, fmt);
	while (*fmt)
	{
		if (*fmt == '%')
		{
			fmt++;
			count += handle_format(*fmt, ap);
		}
		else
		{
			count += ft_putchar(*fmt);
		}
		fmt++;
	}
	va_end(ap);
	return (count);
}
/*
int main()
{
    int count, count1;
	
    count = ft_printf("1:%u\n 2:%u\n", 6, 6); 
	count1 = printf("1:%u\n 2:%u\n", 6, 6);
	
	ft_printf("%d %d\n", count, count1);

}*/