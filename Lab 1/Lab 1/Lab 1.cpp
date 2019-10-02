// Yung Nguyen
// CECS 326
// Lab Assignment #1

#include <iostream>
#include<ctime>
#include<queue>
#include<iomanip>
using namespace std;
void menu();

struct MBT {
	bool MBT[1024];
	int numFreeBlock;
};
struct PCB {
	int PID;
	int tableSize;
	int* pageTable;
};
/*Initizalie MBT (first 32 index is true and the rest is false)
@return: object mbt
*/
MBT initilizeMBT(MBT mbt) {
	//first 32 blocks reserved for OS
	for (int i = 0; i < 32; i++) {
		mbt.MBT[i] = true;
	}
	//the rest of the blocks are initilized to be empty (false)
	for (int i = 32; i < 1024; i++) {
		mbt.MBT[i] = false;
	}
	//number of free blocks
	mbt.numFreeBlock = 1024 - 32; 
	return mbt;
}

/*search in MBT block if there are enough free blocks or not 
and update the number of free block if there is enough space
@param: mbt - MBT object; r - required memory blocks 
@return: bool value of whether there's enough space or not
*/
bool searchMBT(MBT& mbt, int r) {
	if (mbt.numFreeBlock < r ) {
		cout << "There is not enough space!" << endl;
		return false;
	}
	else {
		return true;
	}
}
/*allocate the corresponding memory blocks as "designated" or true
and store block numbers of the allocated blocks in the page table
@param mbt: object of struct MBT to access the memory blocks
@param pcb: object of struct PCB to access the page table
*/
void allocateMemBlock(MBT& mbt, PCB* pcb) {
	int total = 0;
	for (int i = 0; i < 1024; i++) {
		if ((mbt.MBT[i] == false) && (total < pcb->tableSize)) {
			mbt.MBT[i] = true;
			pcb->pageTable[total] = i;
			total++;
		}
	}
}
/*deallocate the corresponding memory blocks as "free" or false
and delete page table and pcb
@param mbt: object of struct MBT to access the memory blocks
@param pcb: object of struct PCB to access the page table and table size
*/
void deallocateMemBlock(MBT& mbt, PCB* pcb) {
	for (int i = 0; i < 1024; i++) {
		int maxIndex = pcb->tableSize;
		if ((i >= pcb->pageTable[0]) && (i <= (pcb->pageTable[maxIndex-1]))) {
			mbt.MBT[i] = false;
		}
	}
}

//Print the Ready Queue
void printQueue(queue<PCB*> q) {
	if (q.empty()) {
		cout << "Ready queue is empty!" << endl;
	}
	int total = 1;
	
	//while it's not empty, print the next element, takes it out and repeat until queue it's empty
	while (!q.empty()) {
		int displayTotal = 1;
		cout << "---------------------------------------------------------------------------------------------------" << endl;
		cout << "PROCESS " << total << endl;
		cout << "PID: " << q.front()->PID << endl;
		cout << "Page Table" << endl;
		//cout << "Number of blocks allocated: " << q.front()->tableSize << endl;
		for (int i = 0; i < q.front()->tableSize; i++) {
			cout << q.front()->pageTable[i]<<setw(5);
			displayTotal++;
			if (displayTotal == 20) {
				cout << "\n";
				displayTotal = 1;
			}
				
		}
		cout << "\n";
		q.pop();
		total++;
		
	}
	
}
/*check if the queue contain a pcb with the inputted PID
@param q: the queue
@param PID: the inputted PID of the process
@return: return true if PID found, return false otherwise
*/
bool doesContain(queue<PCB*> q, int PID) {
	bool contain = false;
	while (!q.empty()) {
		if (q.front()->PID == PID) {
			contain = true;
		}
		q.pop();
	}
	return contain;
}
/*check if the queue contain a pcb with the inputted PID, if it does, return that pcb
@param q: the queue
@param PID: the inputted PID of the process
@return: return the pcb
*/
PCB* doesContain2(queue<PCB*> q, int PID) {
	while (!q.empty()) {
		if (q.front()->PID == PID) {
			return q.front();
		}
		q.pop();
	}
}

/* print the MBT
@param mbt - MBT object
*/
void printMBT(MBT mbt) {
	cout << "------------------------MBT----------------------" << endl;
	cout << "Current available blocks: " << mbt.numFreeBlock << endl;
	cout << "MBT: 0 = AVAILABLE, 1 = ALLOCATED" << endl;
	int total = 1;
	for (int i = 0; i < 1024; i++) {
		
		if (mbt.MBT[i] == true) {
			cout << "|"<< setw(10) << i << setw(10) << "|" << setw(18) << "allocated"<< setw(10) << "|" << endl;
		}
		else {
			cout << "|" << setw(10) << i << setw(10) << "|" << setw(15) << "free" << setw(13) << "|" << endl;
		}
			
	}
}
/* print the PID of each PBC object in queue
@param q - the queue
*/
void printPID(queue<PCB*> q) {
	int total = 1;
	while (!q.empty()) {
		cout << total << ". PID: " << q.front()->PID << endl;
		q.pop();
		total++;
	}
}
/* update queue by copying only the pcb that's not going to be deleted into another queue and return that one
@param: q - queue to be copied
@param: pcb - the pcb that is to be deleted and deallocated
@return: q2 - the new queue without the to-be-deleted pcb
*/
queue<PCB*> updateQueue(queue<PCB*> q, PCB* pcb) {
	queue<PCB*> q2;
	while (!q.empty()) {                    
		if (q.front() != pcb) {
			q2.push(q.front());
		}
		q.pop();
	}
	return q2;
}

int main()
{
	srand(static_cast <unsigned int> (time(0))); //generate seed
	bool quit = false;
	MBT mbt = {}; //initilize MBT
	mbt = initilizeMBT(mbt);
	queue<PCB*> PCBQueue; //initilize empty queue
	int pid = 50; //PID starting point
	do {
		menu(); //print menu
		int choice;
		cin >> choice; //menu selection input
		switch (choice) {
		case 1: {
			int r = rand() % (250 - 10 + 1) + 10;; //generate random number between 10 and 250
			//search MBT for free blocks
			//return to menu if not enough space
			if (!searchMBT(mbt, r)) {
				break;
			}
			mbt.numFreeBlock -= r;
			//initialize dynamic pointer to PCB struct
			
			PCB* pcb = new PCB[1];
			//Obtain a UNIQUE PID
			pcb->PID = pid;
			pid++;
			while (doesContain(PCBQueue, pcb->PID)) {
				
				pcb->PID = rand();
			}
			//allocate dynamic memory for a page table of size 
			//equal to the number of required memory blocks
			int* pageTable = new int[r];
			//save table size and pointer to page table in PCB
			pcb->tableSize = r; 
			pcb->pageTable = pageTable;
			//designating the required number of blocks in MBT as "allocated" or true
			allocateMemBlock(mbt, pcb);
			//add PCB into ready queue
			PCBQueue.push(pcb);
			cout << "Process initiated" << endl;
			break;
		}
		case 2: {
			//for each process in ready queue, output its PID, number of blocks 
			//allocated to it, and all the blocks that are allocated to it
			printQueue(PCBQueue);
			printMBT(mbt);
			break;
		}
		case 3: {
			int PID;
			cout << "Enter PID: " << endl;
			cin >> PID;
			//check if PID is in the queue, if it does, deallocate the process information
			if (doesContain(PCBQueue, PID)) {
				cout << "It does contain it!" << endl;
				PCB* pcb = doesContain2(PCBQueue, PID);
				//update queue 
				PCBQueue = updateQueue(PCBQueue, pcb);
				//update number of free blocks
				mbt.numFreeBlock += pcb->tableSize;
				//deallocate the memory blocks, page table, and pcb
				deallocateMemBlock(mbt,pcb);
				delete[] pcb->pageTable;
				//pcb->pageTable = nullptr;
				delete[] pcb;
				//take the pcb out of the queue
				//pcb = nullptr;
			}
			else {
				cout << "There's no such PID!" << endl;
			}
			break;
		}
		case 4: {
			//out PID
			cout << "------------------------------\n";
			printPID(PCBQueue);
			//if queue is not empty ask user if they want to empty it
			if (!PCBQueue.empty()) {
				string response;
				cout << "Queue is not empty. Do you want to empty it? (Y/N)"<<endl;
				cin >> response;
				//empty the queue
				if (response == "Y") {
					while (!PCBQueue.empty()) {
						deallocateMemBlock(mbt, PCBQueue.front());
						delete[] PCBQueue.front()->pageTable;
						delete[] PCBQueue.front();
						PCBQueue.pop();
					}
					//print queue (should say that queue is empty)
					printQueue(PCBQueue); 
					quit = true;
				}
				//if they say no, return to menu
				else {
					quit = false;
				}
			}
			break;
		}
		default:
			printf("Wrong Input\n");
		}
	} while (!quit);
	
}

//Print a menu
void menu()
{
	cout <<"1. Initiate a Process"<<endl;
	cout << "2. Print System State" << endl;
	cout << "3. Terminate process with a specific PID" << endl;
	cout << "4. Exit" << endl;
}


