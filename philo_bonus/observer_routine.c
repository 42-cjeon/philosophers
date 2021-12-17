/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   observer_routine.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjeon <cjeon@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/14 02:09:39 by cjeon             #+#    #+#             */
/*   Updated: 2021/12/18 01:40:22 by cjeon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "philo.h"

void	*everyone_full_start(void *_arg)
{
	unsigned int	i;
	t_observer_arg	*arg;

	arg = (t_observer_arg *)_arg;
	i = 0;
	while (i < arg->n_philos)
	{
		sem_wait(arg->full_philos);
		i++;
	}
	sem_wait(arg->is_end_lock);
	kill(arg->last_philo, SIGTERM);
	return (NULL);
}

void	*self_dead_start(void *_arg)
{
	t_self_dead_arg	*arg;

	arg = (t_self_dead_arg *)_arg;
	while (42)
	{
		sem_wait(arg->philo_arg->event_lock);
		if (is_philo_dead(arg->shared_arg, arg->main_arg, arg->philo_arg))
			exit(0);
		usleep(EPSILON);
		sem_post(arg->philo_arg->event_lock);
	}
	return (NULL);
}
