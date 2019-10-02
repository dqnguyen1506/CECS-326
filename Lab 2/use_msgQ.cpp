// use_msgQ.cpp

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

using namespace std;
// declare my global message buffer
struct buf {
	long mtype; // message type (required) 
	char greeting[50]; // message content
};
//-----------child 1-----------
//this child receives the message, sends a reply, and exits
void child_proc_one(int qid) {
	buf msg;
	int size = sizeof(msg) - sizeof(long);
    /*
    Receive the message from child 2
    @param - qid: the process id of the child 
    @param - msgbuf*: struct msgbuf pointer
    @param - msg: an instance of the buf struct
    @param - (struct msgbuf*) & msg: pointing msgbuf 
    to the msg struct
    @param - size: the maximum size in bytes of the member
    greeting of the structure pointed to by msg
    @param - 113: (greater than 0) -> the first message
    of this type mtype (message from child 2) will be recieved
    @param - 0: the calling thread will suspend execution until
    a message of the desired type is placed on the queue
    */
	msgrcv(qid, (struct msgbuf*) & msg, size, 113, 0);
    //once message is received, notify 
    //that it has received the message
    //and print the message
	cout << getpid() << ": gets message" << endl; 
	cout << "message: " << msg.greeting << endl;
    /*
    appends a copy the second parameter (char) to msg.greeting (char)
    msg.greeting += " and Adios."
    */
	strcat(msg.greeting, " and Adios.");
	cout << getpid() << ": sends reply" << endl;
    // set type mtype = 114 so child 2 can receive
    // the message when msgrcv() is called with the
    // same type mtype (114)	
    msg.mtype = 114;
	// prepare message with type mtype = 114
    /*
    send the new greeting message to child 2
    @param - qid: the process id of the child 
    @param - msgbuf*: pointer of type msgbuf
    @param - msg: an instance of the buf struct
    @param - (struct msgbuf*) & msg: pointing msgbuf 
    to the msg struct
    @param - size: the maximum size in bytes of the member
    greeting of the structure pointed to by msg
    @param - 0: the calling thread will suspend execution until
    the message is sent
    */
	msgsnd(qid, (struct msgbuf*) & msg, size, 0);
	cout << getpid() << ": now exits" << endl;
}
//-----------child 2-----------
//this child sends a message, receives the reply and exits
void child_proc_two(int qid) {
	buf msg;
	int size = sizeof(msg) - sizeof(long);
	// sending a message that will never be read
	msg.mtype = 12;
    //copy the the second parameter (char) into msg.greeting
    //msg.greeting = "Fake message"
	strcpy(msg.greeting, "Fake message");
    /*
    send a message to child 1 (child 1 won't receive it 
    because mtype does not match)
    @param - qid: the process id of the child 
    @param - msgbuf*: pointer of type msgbuf
    @param - msg: an instance of the buf struct
    @param - (struct msgbuf*) & msg: pointing msgbuf 
    to the msg struct
    @param - size: the maximum size in bytes of the member
    greeting of the structure pointed to by msg
    @param - 0: the calling thread will suspend execution until
    the message is sent
    */
	msgsnd(qid, (struct msgbuf*) & msg, size, 0);
	// prepare my message to send
    //msg.greating = "Hello there"
	strcpy(msg.greeting, "Hello there");
    //notify that child 2 (its pid) is sending a message
	cout << getpid() << ": sends greeting" << endl;
    // set message type mtype = 113	
    msg.mtype = 113;
	 /*
    send the new greeting message to child 1
    @param - qid: the process id of the child 
    @param - msgbuf*: pointer of type msgbuf
    @param - msg: an instance of the buf struct
    @param - (struct msgbuf*) & msg: pointing msgbuf 
    to the msg struct
    @param - size: the maximum size in bytes of the member
    greeting of the structure pointed to by msg
    @param - 0: the calling thread will suspend execution until
    the message is sent
    */
	msgsnd(qid, (struct msgbuf*) & msg, size, 0);
    /*
    Receive the message from child 1
    @param - qid: the process id of the child 
    @param - msgbuf*: struct msgbuf pointer
    @param - msg: an instance of the buf struct
    @param - (struct msgbuf*) & msg: pointing msgbuf 
    to the msg struct
    @param - size: the maximum size in bytes of the member
    greeting of the structure pointed to by msg
    @param - 114: (greater than 0) -> the first message
    of this type mtype (message from child 1) will be recieved
    @param - 0: the calling thread will suspend execution until
    a message of the desired type is placed on the queue
    */
	msgrcv(qid, (struct msgbuf*) & msg, size, 114, 0);
    //notify that child 2 has received the message
    //prints the message and exits
	cout << getpid() << ": gets reply" << endl;
	cout << "reply: " << msg.greeting << endl;
	cout << getpid() << ": now exits" << endl;
}
int main() {
    /*
    creates a new message queue with certain conditions
    @param - IPC_PRIVATE: specify the generatation of a unique key and      
    guarantee that no other process will have the same key
    @param - IPC_EXCL | IPC_CREAT | 0600: either a new queue 
    is created, or if the queue exists, the call fails with -1
    @returns - a unique message queue id
    */
	int qid = msgget(IPC_PRIVATE, IPC_EXCL | IPC_CREAT | 0600);
	//spawning two child processes
	pid_t cpid = fork();
	if (cpid == 0) {
		child_proc_one(qid);//child one
		exit(0); //child exits
	}
	cpid = fork();
	if (cpid == 0) {
		child_proc_two(qid);//child two
		exit(0); // child exits
	}
	while (wait(NULL) != -1); // waiting for both children to terminate
    /*
    provides message control operations provided by the qid
    @param - qid: message queue id
    @param - IPC_RMID: Mark the segment to be destroyed.
    The segment will actually be destroyed only after the
    last process detaches it
    @param - NULL: the data structure pointed to by *buf is ignored
    and is NULL
    */
	msgctl(qid, IPC_RMID, NULL);
	cout << "parent proc: " << getpid()
		<< " now exits" << endl;
	exit(0); //exits the program
}
