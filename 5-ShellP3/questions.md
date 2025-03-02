1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

My shell implementation ensures that all child processes complete before the shell continues accepting the user input
by using waitpid() on each of the child processes in execute_pipeline() function. Using this function will prevent
the shell from proceeding until all child processes have terminated, this function in hand, ensures all child processes
have been executed by using waitpid() before accepting any user input further. The result of not calling waitpid() on 
all of the child processes would result in dead processes that will accumulate because their exit statuses will not be 
tracked and collected, and because of this, it will lead to resource leaks and undesired behavior of the shell system.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

For the function dup2, it is necessary to close unused pipe ends after calling that function because if leaving the unused pipe
ends occur, it can result in unused file descriptors to remain allocated. In addition, leaving the pipe ends to be unclosed will
result in data that may not be properly flushed or written as well. Another crucial issue that will occur if leaving a pipe 
unclosed is when a process waits for an input from a pipe while another process holds it open indefinitely can result in errors
and unexpected behavior. It is necessary to close unused pipe ends as it will allow for expected behavior of the system and will
prevent unexpected behavior from occuring from the system.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

cd is implemented as a built-in command rather than an external command due to the fact that it modifies the shell's working
directory, which is a process-wide action or state. If cd were implemented as an external process, the new process working with
the cd command would change it's working directory, but this in hand would not affect the parent shell process. As a result of 
this, the working directory would remain unchanged even after the command finishes processing.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

How I would modify my implementation to allow an arbitary number of piped commands while still handling the memory allocation
effectively is we can use a linked-list to store the commands. With this implementation, we would be able to store an x amount
of commands while holding memory efficiently as each command that will be added before and past the max size will link to each
other. The possible trade-offs to consider when using this is approach is the increased complexity in managing memory and process
execution, and the increase in time complexity will be due to the fact of using a linked list as searching for specific commands
will be a linear time process depending on the amount of commands stored.
