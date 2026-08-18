# Homework - Synchronization

My submission for Computer Science CPSC 351, section 07, HW week 7

# My information

* Name: Jose Mejias
* CWID: 820507572
* Email: jmejias0@csu.fullerton.edu

# Instructions

Here I will write the steps I am going to take based on the professor's instructions to complete this assignment

1. First check what the program is supposed to do. When beginWritting() returns, both threads must 
be finished and all file handles must be closed.

a. Store the incomming use_mutexes value into G_USE_MUTEXES
b. clear/create sync.txt before the thread start
c. create Thread-A and Thread-B
d. throw std::runtime_error if thread creation fails
e. join both threads before returning

2. Decide what each thread writes threadAEntry():

a. Writes number of lines
b. outer loop runs G_LINE_COUNT_PER_TYPE
c. inner loop writes 0 through 9
d. each character must be written individually
e. after each character write a space
f. then end the line with a newline

For threadBEntry(): same idea but A through Z

3. Choose correct critical section, most important design part because the mutex should protect 
the whole line, not one whole thread lifetime because that will print 500,000 lines, also one 
character at the time so the lines could mix.

the correct critical section is to lock, then write one full line and then unlock.

5. How each thread should behave with and without mutexes inside each thread function:

for each line:
	a. if G_USE_MUTEXES is true, lock mutex
	b. write the full line character by character
	c. if G_USE_MUTEXES is true, unlock mutex

Before starting threads we need to truncate the file onece, then each thread opens the file 
in append mode so both threads keep writing to the same file
