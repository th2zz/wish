# wish Command Line Interpreter

History will report error in following cases:
more than 1 arguments
the first argument is not a valid number
	(+,-,. can only appear once; +,- can only appear at begining, . can only appear in middle. Any other characters results in invalid number)
In the following cases: history will print all lines recorded since the program starts
	the first argument exceeds number of lines of current record
	the first argument exceeds maximum buffer size for current log(the maximum is mannually set to be 2048)
If input number <= 0, history will do nothing.


redirection of builtin commands are supported.
piping of builtin commands are not supported.

for piping and redirection:
	When user input commands like: "x>x"  "x|x"
	error will be thrown when:
		left x is empty or space
		right x is empty or space
		both left x and right x is empty or space
for redirction, addtionally, multiple files after > will be regarded as errors

for piping:
	when both left x and right x are not empty:
		if BOTH left x and right x excutables are not found in current search path
			then two error will be reported
		if left x is found in current search path AND right x is not found
			it will behave like normal shell except that one error will be reported and no output will appear on stdout.
		if left x and right x are BOTH found in current search path
			output to stdout and stderr of left x will be piped to stdin of right x
		finally, if left x is not found AND right x is found, the error should be redirected to right x (Not supported, simply skip right side instruction).


<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>CS 537 Introduction to Operating Systems</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="description" content="">

    <link href="css/bootstrap.css" rel="stylesheet">
    <style>
      body {
        padding-top: 20px;
      }
    </style>
    <link href="css/bootstrap-responsive.css" rel="stylesheet">
		<script type="text/javascript" src="http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML"></script>


    <!-- HTML5 shim, for IE6-8 support of HTML5 elements -->
    <!--[if lt IE 9]>
      <script src="http://html5shim.googlecode.com/svn/trunk/html5.js"></sc\
ript>
    <![endif]-->
  </head>

  <body>

    <!--div class="navbar navbar-inverse navbar-fixed-top">
      <div class="navbar-inner">
        <div class="container">
          <a class="btn btn-navbar" data-toggle="collapse" data-target=".na\
v-collapse">
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </a>
          <a class="brand" href="#">CS 537 Introduction to Operating Systems</a>
          <div class="nav-collapse collapse">
            <ul class="nav">
              <li class="active"><a href="/">Home</a></li>
            </ul>
          </div>
        </div>
      </div>
    </div-->

    <div class="container">

      <h1 id="cs-537-spring-2019-project-2a-unix-shell">CS 537 Spring 2019, Project 2a: Unix Shell</h1>

<p><strong>Change Log:</strong></p>
<ul>
  <li><strong>2/9/19 -</strong> Defined behavior of <code>history</code> command when executed as first argument.</li>
  <li><strong>2/11/19 -</strong> Clarified piping for built-in and error behaviors.</li>
  <li><strong>2/15/19 -</strong> Added FAQ.</li>
</ul>

<p><strong><em>Important Note:</em></strong> Due to concerns raised on Piazza additional clarifying requirements 
and examples have been added for the <code>history</code> built-in command.</p>

<h2 id="faq">FAQ</h2>
<ul>
  <li><strong>Piping when first command is empty</strong> 
  <a href="https://piazza.com/class/jql5yyu0x8a12k?cid=487">Q1</a></li>
  <li><strong>Redirection when there is no command, <code>&gt; out.txt</code></strong>
  <a href="https://piazza.com/class/jql5yyu0x8a12k?cid=531">Q1</a></li>
  <li><strong>History Arguments</strong>
  <a href="https://piazza.com/class/jql5yyu0x8a12k?cid=413">Q1</a>
  <a href="https://piazza.com/class/jql5yyu0x8a12k?cid=591">Q2</a>
  <a href="https://piazza.com/class/jql5yyu0x8a12k?cid=404">Q3</a>
  <a href="https://piazza.com/class/jql5yyu0x8a12k?cid=313">Q4</a>
  <a href="https://piazza.com/class/jql5yyu0x8a12k?cid=313">Q5</a></li>
  <li><strong>‘cd ‘ , cd with zero argument</strong>
  <a href="https://piazza.com/class/jql5yyu0x8a12k?cid=540">Q1</a></li>
  <li><strong>‘wish&gt; ‘ apears late</strong>
  <a href="https://piazza.com/class/jql5yyu0x8a12k?cid=452">Q1</a></li>
  <li><strong>History skips empty lines</strong>
  <a href="https://piazza.com/class/jql5yyu0x8a12k?cid=580">Q1</a></li>
  <li><strong>Redirection errors</strong>
  <a href="https://piazza.com/class/jql5yyu0x8a12k?cid=527">Q1</a></li>
</ul>

<p>In this project, you’ll build a simple Unix shell. The shell is the heart of
the command-line interface, and thus is central to the Unix/C programming
environment. Mastering use of the shell is necessary to become proficient in
this world; knowing how the shell itself is built is the focus of this
project.</p>

<p>There are three specific objectives to this assignment:</p>

<ul>
  <li>To further familiarize yourself with the Linux programming environment.</li>
  <li>To learn how processes are created, destroyed, and managed.</li>
  <li>To gain exposure to the necessary functionality in shells.</li>
</ul>

<h2 id="overview">Overview</h2>

<p>In this assignment, you will implement a <em>command line interpreter (CLI)</em> or,
as it is more commonly known, a <em>shell</em>. The shell should operate in this
basic way: when you type in a command (in response to its prompt), the shell
creates a child process that executes the command you entered and then prompts
for more user input when it has finished.</p>

<p>The shell that you implement will be similar to, but simpler than, the one you run
every day in Unix. If you don’t know what shell you are running, it’s probably
<code>bash</code>. One thing you should do on your own time is learn more about your
shell, by reading the man pages or other online materials.</p>

<h2 id="program-specifications">Program Specifications</h2>

<h3 id="basic-shell-wish">Basic Shell: <code>wish</code></h3>

<p>Your basic shell, called <code>wish</code> (short for Wisconsin Shell, naturally), is
basically an interactive loop: it repeatedly prints a prompt <code>wish&gt; </code> (note
the space after the greater-than sign), parses the input, executes the command
specified on that line of input, and waits for the command to finish. This is
repeated until the user types <code>exit</code>.  The name of your final executable
should be <code>wish</code>.</p>

<p>The shell can be invoked with either no arguments or a single argument;
anything else is an error. Here is the no-argument way:</p>

<pre><code>prompt&gt; ./wish
wish&gt; 
</code></pre>

<p>At this point, <code>wish</code> is running, and ready to accept commands. Type away!</p>

<p>The mode above is called <em>interactive</em> mode, and allows the user to type
commands directly. The shell also supports a <em>batch mode</em>, which instead reads
input from a batch file and executes commands from therein. Here is how you
run the shell with a batch file named <code>batch.txt</code>:</p>

<pre><code>prompt&gt; ./wish batch.txt
</code></pre>

<p>One difference between batch and interactive modes: in interactive mode, a
prompt is printed (<code>wish&gt; </code>). In batch mode, no prompt should be printed.</p>

<p>You should structure your shell such that it creates a process for each new
command (the exceptions are <em>built-in commands</em>, discussed below).  Your basic
shell should be able to parse a command and run the program corresponding to
the command.  For example, if the user types <code>ls -la /tmp</code>, your shell should
run the program <code>/bin/ls</code> with the given arguments <code>-la</code> and <code>/tmp</code> (how does
the shell know to run <code>/bin/ls</code>? It’s something called the shell <strong>path</strong>;
more on this below).</p>

<h2 id="structure">Structure</h2>

<h3 id="basic-shell">Basic Shell</h3>

<p>The shell is very simple (conceptually): it runs in a while loop, repeatedly
asking for input to tell it what command to execute. It then executes that
command. The loop continues indefinitely, until the user types the built-in
command <code>exit</code>, at which point it exits. That’s it!</p>

<p>For reading lines of input, you should use <code>getline()</code>. This allows you to
obtain arbitrarily long input lines with ease. Generally, the shell will be
run in <em>interactive mode</em>, where the user types a command (one at a time) and
the shell acts on it. However, your shell will also support <em>batch mode</em>, in
which the shell is given an input file of commands; in this case, the shell
should not read user input (from <code>stdin</code>) but rather from this file to get the
commands to execute.</p>

<p>In either mode, if you hit the end-of-file marker (EOF), you should call
<code>exit(0)</code> and exit gracefully.</p>

<p>To parse the input line into constituent pieces, you might want to use
<code>strtok()</code> (or, if doing nested tokenization, use <code>strtok_r()</code>). Read the man
page (carefully) for more details.</p>

<p>To execute commands, look into <code>fork()</code>, <code>exec()</code>, and <code>wait()/waitpid()</code>.
See the man pages for these functions, and also read the relevant <a href="http://www.ostep.org/cpu-api.pdf">book
chapter</a> for a brief overview.</p>

<p>You will note that there are a variety of commands in the <code>exec</code> family; for
this project, you must use <code>execv</code>. You should <strong>not</strong> use the <code>system()</code>
library function call to run a command.  Remember that if <code>execv()</code> is
successful, it will not return; if it does return, there was an error (e.g.,
the command does not exist). The most challenging part is getting the
arguments correctly specified.</p>

<h3 id="paths">Paths</h3>

<p>In our example above, the user typed <code>ls</code> but the shell knew to execute the
program <code>/bin/ls</code>. How does your shell know this?</p>

<p>It turns out that the user must specify a <strong>path</strong> variable to describe the
set of directories to search for executables; the set of directories that
comprise the path are sometimes called the <em>search path</em> of the shell. The
path variable contains the list of all directories to search, in order, when
the user types a command.</p>

<p><strong>Important:</strong> Note that the shell itself does not <em>implement</em> <code>ls</code> or other
commands (except built-ins). All it does is find those executables in one of
the directories specified by <code>path</code> and create a new process to run them.</p>

<p>To check if a particular file exists in a directory and is executable,
consider the <code>access()</code> system call. For example, when the user types <code>ls</code>,
and path is set to include both <code>/bin</code> and <code>/usr/bin</code>, try <code>access("/bin/ls",
X_OK)</code>. If that fails, try “/usr/bin/ls”. If that fails too, it is an error.</p>

<p>Your initial shell path should contain one directory: `/bin’</p>

<p>Note: Most shells allow you to specify a binary specifically without using a
search path, using either <strong>absolute paths</strong> or <strong>relative paths</strong>. For
example, a user could type the <strong>absolute path</strong> <code>/bin/ls</code> and execute the
<code>ls</code> binary without a search path being needed. A user could also specify a
<strong>relative path</strong> which starts with the current working directory and
specifies the executable directly, e.g., <code>./main</code>. In this project, you <strong>do
not</strong> have to worry about these features.</p>

<h3 id="built-in-commands">Built-in Commands</h3>

<p>Whenever your shell accepts a command, it should check whether the command is
a <strong>built-in command</strong> or not. If it is, it should not be executed like other
programs. Instead, your shell will invoke your implementation of the built-in
command. For example, to implement the <code>exit</code> built-in command, you simply
call <code>exit(0);</code> in your wish source code, which then will exit the shell.</p>

<p>In this project, you should implement <code>exit</code>, <code>cd</code>, <code>history</code>, and <code>path</code> as built-in
commands.</p>

<ul>
  <li>
    <p><code>exit</code>: When the user types <code>exit</code>, your shell should simply call the <code>exit</code>
system call with 0 as a parameter. It is an error to pass any arguments to
<code>exit</code>.</p>
  </li>
  <li>
    <p><code>cd</code>: <code>cd</code> always take one argument (0 or &gt;1 args should be signaled as an
error). To change directories, use the <code>chdir()</code> system call with the argument
supplied by the user; if <code>chdir</code> fails, that is also an error.</p>
  </li>
  <li>
    <p><code>history</code>: When the user enters the <code>history</code> command with no arguments, then
the shell should print a list of all the lines of input which the user has
entered since the shell was started <strong>including the <code>history</code> command</strong>. The user
may also supply a single argument to the <code>history</code> command <code>n</code> where <code>n</code> is a number.
When this happens the shell should print the previous <code>n</code> lines of input <em>(if <code>n</code> is
not an integer, then round its value up to the next greatest integer)</em>. If <code>n</code> is
greater than than the total number of lines input by the user, then all previous lines 
of input should be printed. If the first arguement to the history command is not an 
integer or if the number of arguments is greater than one, then an error should be 
printed and the input ignored. Examples of using the history command are shown below:</p>

    <p><em>Example #1</em></p>
    <pre><code>prompt&gt; ./wish
wish&gt; ls
my_dir my_file.txt
wish&gt; cd my_dir
wish&gt; ls
my_other_file.txt
wish&gt; cat my_other_file.txt
Hello, world!
wish&gt; history
ls
cd my_dir
ls
cat my_other_file.txt
history
wish&gt; history 3
cat my_other_file.txt
history
history 3
wish&gt; history 10
ls
cd my_dir
ls
cat my_other_file.txt
history
history 3
history 10
wish&gt;
</code></pre>

    <p><em>Example #2</em></p>
    <pre><code>prompt&gt; ./wish
wish&gt; history
history
wish&gt;
</code></pre>
  </li>
  <li>
    <p><code>path</code>: The <code>path</code> command takes 0 or more arguments, with each argument
separated by whitespace from the others. A typical usage would be like this:
<code>wish&gt; path /bin/ /usr/bin</code>, which would add <code>/bin/</code> and <code>/usr/bin</code> to the
search path of the shell. If the user sets path to be empty, then the shell
should not be able to run any programs (except built-in commands). The
<code>path</code> command always overwrites the old path with the newly specified
path. Also, notice in the previous example that one path ends with a <code>/</code>
while the other does not. The shell should be robust enough to run programs
in both directories.</p>
  </li>
</ul>

<h3 id="redirection">Redirection</h3>

<p>Many times, a shell user prefers to send the output of a program to a file
rather than to the screen. Usually, a shell provides this nice feature with
the <code>&gt;</code> character. Formally this is named as redirection of standard
output. To make your shell users happy, your shell should also include this
feature, but with a slight twist (explained below).</p>

<p>For example, if a user types <code>ls -la /tmp &gt; output</code>, nothing should be printed
on the screen. Instead, the standard output of the <code>ls</code> program should be
rerouted to the file <code>output</code>. In addition, the standard error output of
the program should be rerouted to the file <code>output</code> (the twist is that this
is a little different than standard redirection).</p>

<p>If the <code>output</code> file exists before you run your program, you should simple
overwrite it (after truncating it).</p>

<p>The exact format of redirection is a command (and possibly some arguments)
followed by the redirection symbol followed by a filename. Multiple
redirection operators or multiple files to the right of the redirection sign
are errors.</p>

<p>Note: don’t worry about redirection for built-in commands (e.g., we will
not test what happens when you type <code>path /bin &gt; file</code>).</p>

<h3 id="piping">Piping</h3>

<p>Another popular feature which shells offer is the ability to send the output of
one command onto a second command in a single line of input. This allows users
to chain commands together without using intermediate files. This ability is
commonly known as <em>piping</em> because the “pipe” character <code>|</code> is often used to
represent the command.</p>

<p>For example, if a user types <code>ls -la | more</code>, then the standard output and
standard error output of <code>ls -la</code> should be given as the standard input of <code>more</code>.
Then the output of <code>more</code> should be printed to the screen. Note, this is to be
done without the use of redirection through an intermediate file.</p>

<p>Note: don’t worry about piping for built-in commands (e.g., we will
not test what happens when you type <code>history | grep ls</code>).</p>

<p><strong><em>Important Note 1:</em></strong> In our shell, no more than two commands will be piped in a
single line of input from the user. So, if the user enters a command with more
than one <code>|</code>, then an error should be printed and the input ignored.</p>

<p><strong><em>Important Note 2:</em></strong> The use of a pipe and output redirection in a single line
of input is not supported by our shell. So, if the user submits input which
contains both a <code>|</code> and <code>&gt;</code>, an error should be printed to the screen and the
input ignored.</p>

<p><strong><em>Important Note 3:</em></strong> If a line of input contains <code>|</code> with no following command
(ex. <code>ls |</code>), then the input should be ignored and an error be printed.</p>

<h3 id="program-errors">Program Errors</h3>

<p><strong>The one and only error message.</strong> You should print this one and only error
message whenever you encounter an error of any type:</p>

<pre><code>    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
</code></pre>

<p>The error message should be printed to stderr (standard error), as shown
above.</p>

<p>After <del><em>any</em></del> most errors, your shell should simply <em>continue processing</em> after
printing the one and only error message. However, if the shell is invoked with
more than one file, or if the shell is passed a bad batch file, it should exit
by calling <code>exit(1)</code>.</p>

<p>There is a difference between errors that your shell catches and those that
the program catches. Your shell should catch all the syntax errors specified
in this project page. If the syntax of the command looks perfect, you simply
run the specified program. If there are any program-related errors (e.g.,
invalid arguments to <code>ls</code> when you run it, for example), the shell does not
have to worry about that (rather, the program will print its own error
messages and exit).</p>

<h3 id="miscellaneous-hints">Miscellaneous Hints</h3>

<p>Remember to get the <strong>basic functionality</strong> of your shell working before
worrying about all of the error conditions and end cases. For example, first
get a single command running (probably first a command with no arguments, such
as <code>ls</code>).</p>

<p>Next, add built-in commands. Then, try working on redirection. Each of these
requires a little more effort on parsing, but each should not be too hard to
implement.</p>

<p>At some point, you should make sure your code is robust to white space of
various kinds, including spaces (<code> </code>) and tabs (<code>\t</code>). In general, the user
should be able to put variable amounts of white space before and after
commands, arguments, and various operators; however, the operators
(for example, the redirection command) do not require whitespace.</p>

<p>Check the return codes of all system calls from the very beginning of your
work. This will often catch errors in how you are invoking these new system
calls. It’s also just good programming sense.</p>

<p>Beat up your own code! You are the best (and in this case, the only) tester of
this code. Throw lots of different inputs at it and make sure the shell
behaves well. Good code comes through testing; you must run many different
tests to make sure things work as desired. Don’t be gentle – other users
certainly won’t be.</p>

<p>Finally, keep versions of your code. More advanced programmers will use a
source control system such as git. Minimally, when you get a piece of
functionality working, make a copy of your .c file (perhaps a subdirectory
with a version number, such as v1, v2, etc.). By keeping older, working
versions around, you can comfortably work on adding new functionality, safe in
the knowledge you can always go back to an older, working version if need be.</p>

<h2 id="test">Test</h2>
<p>To test your Unix Shell:
1) Navigate to the directory that contains your wish.c.
2) Run the script “runtests” that can be found in /u/c/s/cs537-1/tests/p2a. You may
   copy it to another location if you prefer.</p>

<h2 id="due-date">Due Date</h2>

<p>This project is due on February, 15th, 2019 at 11:59PM.</p>

<h3 id="submiting-your-implementation">Submiting Your Implementation</h3>

<p>It is possible to implement the shell in a single <code>.c</code> file, so your handin
directory should contain at minimum:</p>
<ul>
  <li>1 make file named <code>Makefile</code></li>
  <li>1 README file named <code>README.md</code></li>
  <li>1 <code>.c</code> file containing the source code for your implementation of the shell</li>
</ul>

<p>The directory may contain more files than these three, but extra files should
be either more source files <code>.c</code> and <code>.h</code>. If you use multiple source files,
then please use <code>.h</code> files appropriately to maitain a clean and well-formed
code base.</p>

<p>Use the README file for any explanation you feel would be helpful to someone
trying to understand the structure of your code and files (if you have
multiple source files).</p>

<p>The make file included in your submission should be behave in the following
mannner. When the <code>make</code> command is executed in your handin directory an
executeable file named <code>wish</code> should be created such that when <code>./wish</code> is
executed in the handin directory your implementation of the shell begins
running in interactive mode.</p>

<p>When the make file compiles source code it should use the <code>gcc</code> compiler
followed by both the <code>-Wall</code> and <code>-Werror</code> flags. So, any build targets in
your make file should match the following regular expression:
<code>[A-z0-9]+:\n\tgcc -Wall -Werror</code>. If you would like to confirm that the
build targets in your make file are correct, then you may use
<a href="https://regexr.com/">RegExr</a> to verify that the build targets match the
expression.</p>

<p>Below is an example of a make file for an implementation which is contained
in a single <code>.c</code> file:</p>
<pre><code>default:
    gcc -Wall -Werror wish.c -o wish

clean:
    rm wish
</code></pre>
<p>If you are unfamiliar with make files, then you can learn more about on the <a href="https://www.gnu.org/software/make/manual/html_node/Introduction.html">GNU website</a>.</p>


    </div>

  </body>
</html>

