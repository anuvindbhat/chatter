# chatter
A multithreaded chat program written in C using POSIX sockets

## Dependencies
[GCC](https://gcc.gnu.org/install/)\
[ncurses](https://www.gnu.org/software/ncurses/)

### Ubuntu
```sudo apt install build-essential ncurses-dev```

## Usage
Run the server using ```./server``` and run the client using ```./client```.\
The IP address of a machine can be found using ```ip a | grep inet``` or ```hostname -I```.
