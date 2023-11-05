# `microtime`

`microtime` is a tool for timing programs with execution times below a few ms.

Microtime takes two arguments:

  * The program to execute.
  * A file that should be passed to stdin.

Microtime will execute the program repeatedly until it has seen a total of at
least 500ms of execution time, after which point it will exit and print
a summary:

    Ran 789 times, mean 456us, stddev 123us

The advantages of using microtime instead of simply using `time` in your
shell are:

  * `microtime` only starts timing *after* starting a new process but
    *before* executing the program, which allows it to exclude the time it
    takes for the operating system to start the process.
  * `microtime` has microsecond precision, while `time` typically defaults to
    millisecond precision.
  * `microtime` executes many times and takes an average, which is awkward to
    achieve in a shell script and would be less accurate due to large
    overheads.