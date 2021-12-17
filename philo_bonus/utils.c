/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjeon <cjeon@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/14 01:51:39 by cjeon             #+#    #+#             */
/*   Updated: 2021/12/18 01:23:43 by cjeon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>
#include "philo.h"

size_t	ft_strlen(const char *s)
{
	size_t	len;

	len = 0;
	while (s[len])
		len++;
	return (len);
}

void	set_sem_name(char *s, unsigned int id)
{
	static const char	*base_name = "/ft_philo";
	unsigned int		i;

	i = 0;
	while (base_name[i])
	{
		s[i] = base_name[i];
		i++;
	}
	s[i++] = (id % 10) + '0';
	id /= 10;
	while (id)
	{
		s[i++] = (id % 10) + '0';
		id /= 10;
	}
	s[i] = '\0';
}

t_timestamp	get_timestamp_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	syncronized_log(t_shared_arg *shared_arg, t_philo_arg *philo_arg, \
							const char *msg)
{
	sem_wait(shared_arg->is_end_lock);
	printf("%llu %u %s\n", get_timestamp_in_ms() - \
			philo_arg->started_at, philo_arg->id, msg);
	sem_post(shared_arg->is_end_lock);
}

void	smart_sleep(unsigned long long t)
{
	t_timestamp	sleep_start;

	sleep_start = get_timestamp_in_ms();
	while (get_timestamp_in_ms() - sleep_start <= t)
		usleep(EPSILON);
}
