1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**: We use fork() before calling execvp() directly due to the fact that we need to create a seperate child
    process for executing the new established program while keeping the parent process running intact. This is because,
    if we called execvp() directly, the current process would be replaced with the new program, and because of this, 
    it would lose control over execution flow.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the fork system call fails, it returns -1, which indicates that no new process was created, 
    this will usually happen when the system runs out of processes or memory to run. My implementation handles this
    by checking the return value of the fork() function, and if it returns -1, it can print a message of the function
    or process failure, then we would exit or retry the process depending on the context of the usage of the software.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  Execvp() finds the command to execute by searching for the command in the directories specified
    by the path environment variable, which contains the directory path of the command. The function checks if the
    command contains a /, if it does, it executes the command directly. The path variable in the environment is crucial
    in this process as it tells the execvp() function where to look for commands with the absolute or relative path of
    its location.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**: The purpose of calling wait function in the parent process after forking is it makes sure the 
    parent process waits for the child process to complete before continuing forward. This prevents unseen processes
    to happen and helps manage process termination properly. If we didn't call the wait() function, the child process
    may finish exeution, but its exit status for it remains in the system until the parent reads it, this would
    result in consuming resources unncessarily and redundantly.  

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  Using WEXITSTATUS() function provides us with the exit status of the terminated child process
    from the status code returned by the wait function. This is important due to the fact that it tells us whether
    the child porcess executed successfully (0) or if it failed. In addition, it is important because it allows error
    handling and debugging in the parent process if needed.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  In my implementation of build cmd buff, I handle quote arguments by calling my helper function
   process quote strings, whenever it encounters a double quote, this function moves past the opening quote, stores
   the quoted string as an argument in cmd_buff->argv and then continues reading until it finds a closing quote, 
   it then replaces the closing quote with a null byte to close the argument. This is necessary because this approach
   ensures the arguments containing spaces are treated as a signle argument rather than two seperate ones. 

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  Compared to the last assignment, I improved parsing logic by introducing dedicated functions
    for handling quoted and unquoted strings as well, this approach helped make the parsing more readable and 
    adjustable for all kinds of inputs. From last assignment, I improved the quote string handling as the parser
    from before did not handle this. Some unexpected challenges I ran into was ensuring that modifying the command
    line string did not interfere with memory management or the specific argument retrieval. In addition, implementing
    the parsing logic was hard to conceptualize at first, but reusing the first parser code helped with this issue.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  The purposes of signals in linux is they are asynchronous notifications that are sent to processes
    to handle events like termination, interruption, or even user-defined inputs or actions. Signals differ from
    other forms of IPC as they are asynchronous, which means they don't require a sender and receiver to be
    in active communication, and they do not require additional memory.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  _start here_

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  _start here_
