/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjeon <student.42seoul.kr>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/14 02:05:57 by cjeon             #+#    #+#             */
/*   Updated: 2021/12/18 15:32:05 by cjeon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "philo.h"

static void	set_philo_arg(unsigned int id, t_philo_arg *philo_arg)
{
	t_timestamp	now;

	now = get_timestamp_in_ms();
	philo_arg->id = id;
	philo_arg->last_eat = now;
	philo_arg->started_at = now;
	philo_arg->n_eat = 0;
}

static int	wait_end(pthread_t observer, t_shared_arg *shared_arg, \
						pid_t *pids, unsigned int n_philos)
{
	int				status;
	int				result;
	unsigned int	i;

	waitpid(-1, &status, 0);
	result = ((status >> 8) & (0x000000ff));
	i = 0;
	while (i < n_philos)
	{
		if (!kill(pids[i], SIGTERM))
			waitpid(pids[i], &status, 0);
		i++;
	}
	i = n_philos;
	while (i--)
		sem_post(shared_arg->full_philos);
	pthread_join(observer, NULL);
	return (result);
}

static int	free_and_return(pid_t *pids, int value)
{
	free(pids);
	return (value);
}

static int	run_observer(t_observer_arg *observer_arg, pthread_t *observer)
{
	if (pthread_create(observer, NULL, everyone_full_start, observer_arg))
		return (1);
	return (0);
}

int	run_simulation(t_shared_arg *shared_arg, t_main_arg *main_arg, \
					t_philo_arg *philo_arg)
{
	unsigned int	i;
	pid_t			*pids;
	int				result;
	pthread_t		observer;

	observer = NULL;
	pids = (pid_t *)malloc(sizeof(pid_t) * main_arg->n_philos);
	if (pids == NULL)
		return (1);
	i = 0;
	while (i < main_arg->n_philos)
	{
		set_philo_arg(i + 1, philo_arg);
		pids[i] = fork();
		if (pids[i] < 0)
			return (free_and_return(pids, 1));
		else if (pids[i++] == 0)
			exit(philo_start(shared_arg, main_arg, philo_arg));
	}
	if (main_arg->option_arg)
		if (run_observer(&(t_observer_arg){main_arg->n_philos, pids[0], \
			shared_arg->full_philos, shared_arg->is_end_lock}, &observer))
			return (free_and_return(pids, 1));
	result = wait_end(observer, shared_arg, pids, main_arg->n_philos);
	return (free_and_return(pids, result));
}
