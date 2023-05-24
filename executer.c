#include "shell.h"


/**
 * blt_cmd_h - handles builtins and commands
 * @data: struct
 * Return: builtin if it is passed otherwise commands
 */
int blt_cmd_h(sh_dt *data)
{
	int (*bltn)(sh_dt *data) = _builtin(data->args[0]);

	if (data->args[0] == NULL)
	{
		return (1);
	}

	return ((bltn != NULL) ? bltn(data) : exec_cmd(data));

}


/**
 * check_exec - it checks whether it is executable
 * @data: shell data containing all data
 * Return: 0 if it is not executable, positive int otherwise
 */
int check_exec(sh_dt *data)
{
	struct stat st;
	int i = 0;
	char *str = data->args[0];

	for (; str[i]; i++)
	{
		if (str[i] == '.')
		{
			if (str[i + 1] == '.')
				return (0);
			if (str[i + 1] == '/')
				continue;
			else
				break;

		}
		else if (str[i] == '/' && i != 0)
		{
			if (str[i + 1] == '.')
				continue;
			i++;
			break;
		}
		else
			break;
	}

	if (i == 0)
		return (0);

	if (stat(str + i, &st) == 0)
		return (i);

	get_err(data, 127);
	return (-1);
}

/**
 * exec_cmd - executes and runs cmd lines
 * @data: shell containing data
 * Return: 1 on success
 */
int exec_cmd(sh_dt *data)
{
	char *exec_path = NULL;
	pid_t pid;
	int state, run;

	run = check_exec(data);
	if (run == -1)
		return (1);

	if (run == 0)
	{
		printf("The data->args 0 before loc_exec is %s\n", data->args[0]);
		exec_path = loc_exec(data->args[0], data->env);
		printf("The path to execute is %s\n", exec_path);
		if (err_checker(exec_path, data) == 1)
			return (1);
	}

	printf("The data->args 0 before fork is %s\n", data->args[0]);

	pid = fork();
	printf("The data->args 0 after fork is %s\n", data->args[0]);
	if (pid == 0)
	{
		if (run == 0)
		{
			exec_path = loc_exec(data->args[0], data->env);
			printf("The path to execute is %s\n", exec_path + run);
		}
		else
			exec_path = data->args[0];
		printf("The data->args 0 is %s\n", data->args[0]);
		execve(exec_path + run, data->args, data->env);
	}
	else if (pid < 0)
	{
		perror(data->av[0]);
		return (1);
	}
	else
	{
		do {
			waitpid(pid, &state, WUNTRACED);
		} while (!WIFEXITED(state) && !WIFSIGNALED(state));
	}

	data->status = state / 256;
	return (1);
}
