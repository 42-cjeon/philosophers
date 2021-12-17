/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjeon <cjeon@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/14 01:51:42 by cjeon             #+#    #+#             */
/*   Updated: 2021/12/18 01:22:56 by cjeon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include "philo.h"

static void	philo_eat(t_shared_arg *shared_arg, t_main_arg *main_arg, \
						t_philo_arg *philo_arg)
{
	philo_take_forks(shared_arg, philo_arg);
	sem_wait(philo_arg->event_lock);
	philo_arg->last_eat = get_timestamp_in_ms();
	sem_post(philo_arg->event_lock);
	syncronized_log(shared_arg, philo_arg, "is eating");
	smart_sleep(main_arg->tte);
	sem_wait(philo_arg->event_lock);
	if (++(philo_arg->n_eat) == main_arg->n_eat)
		sem_post(shared_arg->full_philos);
	sem_post(philo_arg->event_lock);
	philo_release_forks(shared_arg);
}

static void	philo_sleep(t_shared_arg *shared_arg, t_main_arg *main_arg, \
							t_philo_arg *philo_arg)
{
	syncronized_log(shared_arg, philo_arg, "is sleeping");
	smart_sleep(main_arg->tts);
}

static void	philo_think(t_shared_arg *shared_arg, t_philo_arg *philo_arg)
{
	syncronized_log(shared_arg, philo_arg, "is thinking");
	usleep(EPSILON);
}

int	is_philo_dead(t_shared_arg *shared_arg, t_main_arg *main_arg, \
					t_philo_arg *philo_arg)
{
	int	is_dead;

	is_dead = (get_timestamp_in_ms() - philo_arg->last_eat > main_arg->ttd);
	if (is_dead)
	{
		sem_wait(shared_arg->is_end_lock);
		printf("%llu %u died\n", \
			get_timestamp_in_ms() - philo_arg->started_at, philo_arg->id);
	}
	return (is_dead);
}

int	philo_start(t_shared_arg *shared_arg, t_main_arg *main_arg, \
					t_philo_arg *philo_arg)
{
	char		sem_name[20];
	sem_t		*event_lock;
	pthread_t	self_dead_observer;

	set_sem_name(sem_name, philo_arg->id);
	if (open_semaphore(sem_name, 1, &event_lock))
		return (1);
	philo_arg->event_lock = event_lock;
	if (pthread_create(&self_dead_observer, NULL, self_dead_start, \
				&(t_self_dead_arg){shared_arg, main_arg, philo_arg}))
		return (1);
	while (42)
	{
		philo_eat(shared_arg, main_arg, philo_arg);
		philo_sleep(shared_arg, main_arg, philo_arg);
		philo_think(shared_arg, philo_arg);
	}
}
