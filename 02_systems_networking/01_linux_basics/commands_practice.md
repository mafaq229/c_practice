# Linux Command Line Practice

Complete these exercises to build command line proficiency needed for GIOS.

## File Operations

### Exercise 1: Navigation
```bash
# TODO: Navigate to your home directory
cd ~

# TODO: List all files including hidden ones
ls -a

# TODO: Show current directory
pwd

# TODO: Create a directory called 'test_dir'
mkdir test_dir

# TODO: Navigate into test_dir
cd test_dir
```

### Exercise 2: File Creation and Viewing
```bash
# TODO: Create an empty file called 'example.txt'
touch example.txt

# TODO: Write "Hello, World" to the file (overwrite)
echo "Hello, World" > example.txt

# TODO: Append "Another line" to the file
echo "Another line" >> example.txt

# TODO: View the file contents
cat filename.txt

# TODO: View just the first 5 lines of a file
head -n 5 /etc/passwd

# TODO: View just the last 5 lines
tail -n 5 /etc/passwd
```

### Exercise 3: File Permissions
```bash
# TODO: Show detailed file info including permissions
ls -l example.txt

# TODO: Make the file executable
chmod +x example.txt

# TODO: Make the file readable only by owner
chmod u=r example.txt # 400 does the same but alsp clears group/other permissions. (u=r, go=)

# TODO: Restore read/write for owner
chmod u=rw example.txt # 600 does the same but alsp clears group/other (u=rw,go=)
```

## Process Management

### Exercise 4: Viewing Processes
```bash
# TODO: Show all running processes
ps aux

# TODO: Show processes in a tree format
ps auxf # or ps aux --forest or ps -ax -o pid,ppid,command


# TODO: Find processes by name (e.g., find all 'bash' processes)
ps aux | grep bash # zsh

# TODO: Show real-time process info
top
# (Press 'q' to quit)
```

### Exercise 5: Process Control
```bash
# TODO: Run a command in the background
sleep 60 &

# TODO: List background jobs
jobs # ps shows processes (all or a subset, depending on flags). jobs shows background/suspended jobs started from the current shell only.

# TODO: Bring a background job to foreground
fg %1 # if there is a job numered 1 in my current shell. use "jobs"

# TODO: Kill a process by PID
kill [PID] # PID from ps aux

# TODO: Force kill a process
kill -9 [PID]
```

## Searching

### Exercise 6: Finding Files
```bash
# TODO: Find all .c files in current directory tree
find . -name "*.c"

# TODO: Find all files modified in last 24 hours
find . -mtime -1

# TODO: Find all files larger than 1MB
find . -size +1M

# TODO: Find all directories named 'test'
find . -type d -name "test"
```

### Exercise 7: Searching File Contents
```bash
# TODO: Search for 'TODO' in all .c files
grep -r "TODO" *.c

# TODO: Search recursively in all subdirectories
grep -r "TODO" .

# TODO: Show line numbers with matches
grep -n "TODO" *.c

# TODO: Case-insensitive search
grep -i "error" *.c

# TODO: Show 2 lines before and after each match
grep -C 2 "TODO" *.c
```

# Exercise 10
ip addr
ping -c 3 google.com
ss -tlnp
curl -O http://example.com/file
wget http://example.com/file

## Pipes and Redirection

### Exercise 8: Pipes
```bash
# TODO: Count number of files in current directory
ls | wc -l

# TODO: Find all .c files and count them
find . -name "*.c" | wc -l

# TODO: List processes and find ones using most CPU
ps aux | sort -k3 -rn | head

# TODO: Get unique values from a file
cat file.txt | sort | uniq
```

### Exercise 9: Redirection
```bash
# TODO: Redirect stdout to a file (overwrite)
echo "test" > output.txt

# TODO: Redirect stdout to a file (append)
echo "test" >> output.txt

# TODO: Redirect stderr to a file
./program 2> error.log

# TODO: Redirect both stdout and stderr to a file
./program > all.log 2>&1
```

## Network Commands

### Exercise 10: Network Basics
```bash
# TODO: Show network interfaces
ifconfig
# or
ip addr

# TODO: Check if a host is reachable
ping -c 3 google.com

# TODO: Check which ports are listening
netstat -tlnp
# or
ss -tlnp

# TODO: Download a file
curl -O http://example.com/file
# or
wget http://example.com/file
```

## Answers Key

```bash
# Exercise 1
cd ~                    # or cd $HOME
ls -la
pwd
mkdir test_dir
cd test_dir

# Exercise 2
touch example.txt
echo "Hello, World!" > example.txt
echo "Another line" >> example.txt
cat example.txt
head -n 5 /etc/passwd
tail -n 5 /etc/passwd

# Exercise 3
ls -l example.txt
chmod +x example.txt
chmod 400 example.txt   # or chmod u=r example.txt
chmod 600 example.txt   # or chmod u=rw example.txt

# Exercise 4
ps aux
ps auxf                 # or pstree
ps aux | grep bash
top

# Exercise 5
sleep 60 &
jobs
fg %1
kill [PID]
kill -9 [PID]

# Exercise 6
find . -name "*.c"
find . -mtime -1
find . -size +1M
find . -type d -name "test"

# Exercise 7
grep -r "TODO" *.c
grep -r "TODO" .
grep -n "TODO" *.c
grep -i "error" *.c
grep -C 2 "TODO" *.c

# Exercise 8
ls | wc -l
find . -name "*.c" | wc -l
ps aux | sort -k3 -rn | head
cat file.txt | sort | uniq

# Exercise 9
echo "test" > output.txt
echo "test" >> output.txt
./program 2> error.log
./program > all.log 2>&1

# Exercise 10
ip addr
ping -c 3 google.com
ss -tlnp
curl -O http://example.com/file
wget http://example.com/file
```

## GIOS-Specific Commands

These commands are particularly useful for GIOS:

```bash
# Compile with debug symbols
gcc -g -Wall -Wextra program.c -o program

# Run with Valgrind (memory check)
valgrind --leak-check=full ./program

# Watch a file for changes
watch -n 1 ls -l

# Monitor network traffic on port 8080
tcpdump -i any port 8080

# Check open file descriptors for a process
lsof -p [PID]

# Create a test file of specific size
dd if=/dev/urandom of=testfile bs=1M count=10

# Calculate checksum to verify file transfer
md5sum filename
```
