1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  Using the function fgets() is a good choice for this system due to the fact that it safely reads
    input from the user and the console while preventing a potential buffer overflow by putting a specific amount of
    characters read. In addition fgets() ensures the input does not exceed the specified buffer size as well, which
    is necessary for making the shell system and the buffer to store the command read from the console. This is useful
    for making a safer option for handling  user input for the shell system to be made. Fgets() use is a good choice
    due to the fact that it can handle EOF, allowing the shell system to handle input termination when running.  

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**: We needed to use malloc() to allocate memory for the cmd_buffer due to the fact, that doing this will allow for different
    input sizes to be used for the buffer to handle different kinds of commands for the system and handle different string sizes as well. 
    As opposed to a fixed-size array, it can lead to wasted memory if the input is smaller than the fixed size or can result in a buffer 
    overflow if the input exceeds the allocated size. Dynamic allocation using malloc() ensures that the shell can manage more effectively
    and can accomodate different input and commnad lengths. 


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming leading and trailing spaces from each command in the build_cmd_list() is crucial as the shell might attempt
    and try to execute an empty or incorrectly formatted command due to the extra or trailing spaces along with it, this can lead to 
    potential errors with the shell and improper input handling for the commands. Another issue that can arise is the commands might fail
    and lead to the wrong output due to incorrect string comparsions when checking for specific keywords or string parsing. By executing
    the action of trimming spaces, we can make sure that command names and arguments are correctly recognized and executed properly, this 
    will ensure an effective and efficient shell system. 

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  After doing researching and learning about redirection and piping with STDIN, STDOUT, and STDERR 3 redirections examples that we can 
    implement in our own shell is the first one is redirection regarding the stdin, or redirecting the input in the linux shell, an example of doing
    this is with the symbol, <, or like cat < test.txt. A challenge with doing this is ensuring that the correct file is read when necessary and handling
    errors if the file does not exist by any chance. Another redirection example we can use in our shell is the output redirection, > or >>, we can use
    redirection to output the shell system to a file or somewhere else. A potential challenge with htis is implementing file handling and ensuring that
    redirection does not interfere with normal command execution at runtime. Another redirection example involves with the stderr, which is used with the
    symbol x>, with x representing the exit code that needs to be returned, a potential challenge that can occur with this is differentiating between
    stdout and stderr and how exactly to handle both effectively and not get confused with what needs to be output and not. 
    

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  The key differences between redirection and piping is redirection involves and changes where input/output comes from and goes to in the
    shell, while piping connects the output of one command directly as an input to another command in the shell. Redirection works with files, while
    piping concept works with individual commands such as ls | grep text. Piping is essentially allowing commands to be chained together which can form
    a pipeline. Redirection deals with the input and output of a command directly.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  This is important to keep these seperate due to the fact that we should use the shell system to differentiate between an input
    and an error, it is important to redirect errors and input different as they are different things and are contingent on a lot of things. In addition,
    with keeping these seperate it will prevent errors and input from interfere for different processes and will maintain the shell systems robust and
    manageable structure.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**: How our custom shell should handle errors from commands that fail is that it should capture the incorrect or errored command and from
    there it should report it to STDERR from STDOUT seperately, this will make sure that the error messages will not interfere with the regular
    command output. As mentioned earlier with redirection, we can redirect the errors to a log file to store the errors with a different format as well
    to differentiate it from the actual output. Our custom shell should provide a way to merge error and outputting when necessary and this can be 
    done by using redirection which was mentioned earlier, or using techniques to redirect error and output to a single stream. This in hand will make
    debugging much easier to understand and figure out as well with this system in place.  
