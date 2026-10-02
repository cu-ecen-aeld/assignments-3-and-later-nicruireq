#include "systemcalls.h"
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
 */
bool do_system(const char *cmd)
{

    /*
     * TODO  add your code here
     *  Call the system() function with the command set in the cmd
     *   and return a boolean true if the system() call completed with success
     *   or false() if it returned a failure
     */

    bool exit_status = false;

    int ret = system(cmd);

    if (ret == -1)
    {
        exit_status = false;
        perror("do_system");
    }
    else
    {
        if (WIFEXITED(ret))
        {
            if (WEXITSTATUS(ret) != 0)
            {
                exit_status = false;
            }
            else
            {
                exit_status = true;
            }
        }
        else
        {
            exit_status = false;
        }
    }

    return exit_status;
}

/**
 * @param count -The numbers of variables passed to the function. The variables are command to execute.
 *   followed by arguments to pass to the command
 *   Since exec() does not perform path expansion, the command to execute needs
 *   to be an absolute path.
 * @param ... - A list of 1 or more arguments after the @param count argument.
 *   The first is always the full path to the command to execute with execv()
 *   The remaining arguments are a list of arguments to pass to the command in execv()
 * @return true if the command @param ... with arguments @param arguments were executed successfully
 *   using the execv() call, false if an error occurred, either in invocation of the
 *   fork, waitpid, or execv() command, or if a non-zero return value was returned
 *   by the command issued in @param arguments with the specified arguments.
 */

bool do_exec(int count, ...)
{
    pid_t pid_child;
    pid_t waited_pid;
    int wait_status = 0;
    bool ret = false;
    //int exec_ret = -1;
    char *pathname = NULL;
    va_list args;
    va_start(args, count);
    char *command[count + 1];
    int i;
    for (i = 0; i < count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

    /*
     * TODO:
     *   Execute a system command by calling fork, execv(),
     *   and wait instead of system (see LSP page 161).
     *   Use the command[0] as the full path to the command to execute
     *   (first argument to execv), and use the remaining arguments
     *   as second argument to the execv() command.
     *
     */
    // Validate arguments
    if (count < 2)
    {
        ret = false;
        va_end(args);
        return ret;
    }
    else
    {
        for (i = 0; i < count; ++i)
        {
            if (command[i] == NULL)
            {
                ret = false;
                va_end(args);
                return ret;
            }
        }
    }

    pathname = command[0];

    // forking and wait
    pid_child = fork();

    if (pid_child == -1)
    {
        ret = false;
        perror("do_exec fork failed");
    }
    else if (pid_child == 0)
    {
        execv(pathname, command);
        //exec_ret = execv(pathname, command);
        //if (exec_ret == -1)
        //{
            ret = false;
            perror("do_exec: execv failed, binary not found");
            _exit(127);
        //}
    }
    else
    {
        waited_pid = waitpid(pid_child, &wait_status, 0);

        if (waited_pid == -1)
        {
            ret = false;
            perror("do_exec waiting pid failed");
        }
        else
        {
            if (WIFEXITED(wait_status))
            {
                if (WEXITSTATUS(wait_status) == 127)
                {
                    ret = false;
                    perror("do_exec child process: execv binary not found");
                }
                else if (WEXITSTATUS(wait_status) == 0)
                {
                    ret = true;
                }
                else
                {
                    ret = false;
                    perror("do_exec child process: return non zero value");
                }
            }
            else
            {
                ret = false;
                perror("do_exec child process termination failed");
            }
        }
    }

    va_end(args);

    return ret;
}

/**
 * @param outputfile - The full path to the file to write with command output.
 *   This file will be closed at completion of the function call.
 * All other parameters, see do_exec above
 */
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    pid_t pid_child;
    pid_t waited_pid;
    int wait_status = 0;
    bool ret = false;
    int exec_ret = -1;
    char *pathname = NULL;
    int fd_redirection = -1;

    va_list args;
    va_start(args, count);
    char *command[count + 1];
    int i;
    for (i = 0; i < count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

    /*
     * TODO
     *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
     *   redirect standard out to a file specified by outputfile.
     *   The rest of the behaviour is same as do_exec()
     *
     */
    // Validate arguments
    if (outputfile == NULL)
    {
        va_end(args);
        return false;
    }

    if (count < 2)
    {
        ret = false;
        va_end(args);
        return ret;
    }
    else
    {
        for (i = 0; i < count; ++i)
        {
            if (command[i] == NULL)
            {
                ret = false;
                va_end(args);
                return ret;
            }
        }
    }

    pathname = command[0];

    // forking and wait
    pid_child = fork();
    fd_redirection = open(outputfile, O_WRONLY | O_TRUNC | O_CREAT, 0644);

    if (fd_redirection < 0)
    {
        ret = false;
        perror("open");
    }
    else
    {
        // forking and wait
        pid_child = fork();

        if (pid_child == -1)
        {
            ret = false;
            perror("do_exec fork failed");
        }
        else if (pid_child == 0)
        {
            // Creates a copy of file descriptor for outputfile 
            // pointed by the file descriptor number of the standard output
            // atomically, avoiding race conditions (fd 1 is closed silently before reusing) 
            if (dup2(fd_redirection, 1) < 0)
            {
                perror("dup2 redirection to stdout failed");
                ret = false;
            }
            else
            {
                close(fd_redirection);

                exec_ret = execv(pathname, command);
                if (exec_ret == -1)
                {
                    ret = false;
                    perror("do_exec execv failed");
                }
            }
        }
        else
        {
            waited_pid = waitpid(pid_child, &wait_status, 0);

            if (waited_pid == -1)
            {
                ret = false;
                perror("do_exec waiting pid failed");
            }
            else
            {
                if (WIFEXITED(wait_status))
                {
                    if (WEXITSTATUS(wait_status) != 0)
                    {
                        ret = false;
                        perror("do_exec child process termination OK but returned value is not successful");
                    }
                    else
                    {
                        ret = true;
                    }
                }
                else
                {
                    ret = false;
                    perror("do_exec child process termination failed");
                }
            }
        }
    }

    va_end(args);

    return ret;
}
