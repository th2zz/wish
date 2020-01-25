# wish Command Line Interpreter
A command line interpreter that runs in linux environment.

supported built-in commands:
- path  change path, path will be overwriten each time. When empty shell is provided, shell can only execute built-in commands
- history x, show x lines of user commands history
- exit exit the shell
- cd    change current directory

Otherwise unspecified, shell will try to find binary executable in current path exhaustively.

History will report error in following cases:
- more than 1 arguments
- the first argument is not a valid number
	(+,-,. can only appear once; +,- can only appear at begining, . can only appear in middle. Any other characters results in invalid number)
- In the following cases: history will print all lines recorded since the program starts
	- the first argument exceeds number of lines of current record
	- the first argument exceeds maximum buffer size for current log(the maximum is mannually set to be 2048)
If input number <= 0, history will do nothing.


- redirection of builtin commands are supported.
- piping of builtin commands are not supported.

- for piping and redirection:
	When user input commands like: "x>x"  "x|x"
	error will be thrown when:
		left x is empty or space
		right x is empty or space
		both left x and right x is empty or space
for redirction, addtionally, multiple files after > will be regarded as errors

- for piping:
	when both left x and right x are not empty:
		if BOTH left x and right x excutables are not found in current search path
			then two error will be reported
		if left x is found in current search path AND right x is not found
			it will behave like normal shell except that one error will be reported and no output will appear on stdout.
		if left x and right x are BOTH found in current search path
			output to stdout and stderr of left x will be piped to stdin of right x
		finally, if left x is not found AND right x is found, the error should be redirected to right x (Not supported, simply skip right side instruction).



  
