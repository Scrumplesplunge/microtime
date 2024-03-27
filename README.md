# `microtime`

`microtime` is a tool for timing programs with execution times below a few ms.

Microtime will execute the program repeatedly until it has seen a total of at
least 500ms of execution time, after which point it will exit and print
a summary:

    # Make sure the CPU is in performance mode.
    $ sudo cpupower frequency-set --governor performance
    $ microtime mystdin.txt myprogram arg1 arg2
    Ran 789 times, mean 456us, stddev 123us
    # Restore the CPU to power saving mode.
    $ sudo cpupower frequency-set --governor powersave

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
