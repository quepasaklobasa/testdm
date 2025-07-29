/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42prague.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/19 13:04:39 by jcouto            #+#    #+#             */
/*   Updated: 2024/09/12 15:08:51 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*read_file(int fd, char *tmp)
{
	ssize_t	bytes_read;
	char	*buffer;

	buffer = (char *)malloc((BUFFER_SIZE + 1) * sizeof(char));
	if (!tmp)
	{
		tmp = (char *)malloc(1 * sizeof(char));
		tmp[0] = '\0';
	}
	bytes_read = 1;
	while (!(ft_g_strchr(tmp, '\n')) && bytes_read != 0)
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read < 0)
		{
			free(buffer);
			free(tmp);
			return (NULL);
		}
		buffer[bytes_read] = '\0';
		tmp = ft_g_strjoin(tmp, buffer);
	}
	free(buffer);
	return (tmp);
}

char	*adjust_buffer(char *sbuf)
{
	char	*nb;
	int		i;
	int		j;

	i = 0;
	while (sbuf[i] && sbuf[i] != '\n')
		i++;
	if (!sbuf[i])
	{
		free(sbuf);
		return (NULL);
	}
	nb = (char *)malloc((ft_gstrlen(sbuf) - i + 1) * sizeof(char));
	if (!nb)
	{
		free(sbuf);
		return (NULL);
	}
	i++;
	j = 0;
	while (sbuf[i])
		nb[j++] = sbuf[i++];
	nb[j] = '\0';
	free (sbuf);
	return (nb);
}

char	*extract_line(char *buf)
{
	int		i;
	char	*line;

	i = 0;
	if (!buf[i])
		return (NULL);
	while (buf[i] && buf[i] != '\n')
		i++;
	if (buf[i] == '\n')
		i++;
	line = (char *)malloc((i + 1) * sizeof(char));
	i = 0;
	while (buf[i] && buf[i] != '\n')
	{
		line[i] = buf[i];
		i++;
	}
	if (buf[i] == '\n')
		line[i++] = '\n';
	line[i] = '\0';
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*static_buffer[4096];
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	static_buffer[fd] = read_file(fd, static_buffer[fd]);
	if (!static_buffer[fd])
		return (NULL);
	line = extract_line(static_buffer[fd]);
	static_buffer[fd] = adjust_buffer(static_buffer[fd]);
	return (line);
}
