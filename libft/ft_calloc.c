/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 15:01:15 by jcouto            #+#    #+#             */
/*   Updated: 2024/06/10 14:14:38 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*p;

	if (nmemb == 0 || size == 0)
		return (malloc(0));
	p = (void *)malloc(nmemb * size);
	if (p == NULL)
		return (0);
	ft_bzero(p, nmemb * size);
	return (p);
}
/*
int	main()
{
 	int size = 8539;

 	void * d1 = ft_calloc(size, sizeof(int));
 	void * d2 = calloc(size, sizeof(int));
 	if (memcmp(d1, d2, size * sizeof(int)) == 0)
 		printf("Todo es identico!");
	else
		printf("Memory differs pelotudo");
 	free(d1);
 	free(d2);
	return (0);
//	void * d1 = ft_calloc(0, sizeof(int));
//	void * d2 = calloc(0, sizeof(int));
//	if (d1 == NULL && d2 == NULL)
//		printf("Ambos kaloks son NULL para elementos vacios");
//	else if (d1 != NULL & d2 == NULL)
//		printf("Ninguno regreso NULL para cero elementos D:");
//	else
//		printf("Una si y una no xd");
}*/
