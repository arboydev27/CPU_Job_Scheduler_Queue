/*
 * Author: Arboy Magomba
 * Date: October 25, 2024
 * 
 * Description:
 * This program simulates a CPU job queue management system. It includes classes 
 * for managing CPU jobs (CPUJob), a queue structure (Queue), and an enhanced 
 * queue (NovelQueue) that allows operations such as enqueue, dequeue, modify, 
 * promote, reorder, and display. The main function processes user commands to 
 * manage jobs dynamically within the queue. This implementation supports job 
 * prioritization, job modification, and reordering based on specific attributes.
 *
 * Full code documentation and AI assistance details are provided at the end of the code.
 */

#include <iostream>
#include <string>
using namespace std;

class CPUJob {
public:
    int jobID;               // Unique identifier for the job
    int priority;             // Priority level for the job (1-10)
    int job_type;             // Job type (1-10)
    int cpu_time_consumed;     // Total CPU time consumed by the job
    int memory_consumed;       // Total memory consumed thus far

CPUJob(int id, int pri, int type, int cpuTime, int memory) //Constructor
    : jobID(id), priority(pri), job_type(type), cpu_time_consumed(cpuTime), memory_consumed(memory) {}

void display() {
    cout << "Job ID: " << jobID << ", Priority: " << priority << ", Job Type: " << job_type 
            << ", CPU Time Consumed: " << cpu_time_consumed << ", Memory Consumed: " << memory_consumed;
    }
};

template <class DT>
class Queue {
public:
    DT JobPointer;               // Pointer to a job (e.g., CPUJob)
    Queue<DT>* next;             // Pointer to the next node in the queue
    int size;                    // Number of elements in the queue

    Queue() : JobPointer(nullptr), next(nullptr), size(0) {}

    ~Queue() {                    // Destructor
        while (!isEmpty()) {
            dequeue();
        }
    }

void enqueue(DT* newJob) {
    Queue<DT>* newNode = new Queue<DT>();
    newNode->JobPointer = newJob;
    newNode->next = nullptr;

    if (isEmpty()) { //If no jobs have been added, the front becomes the enqueued object
        this->JobPointer = newNode->JobPointer;
        this->next = newNode->next;
    } else {
        Queue<DT>* temp = this;
        while (temp->next != nullptr) { //Loop until you find an empty slot in the queue
            temp = temp->next;
        }
        temp->next = newNode;
    }
    size++; //Useful for looping later
}

    DT dequeue() {
        if (isEmpty()) { //Can't dequeue empty space
            return nullptr;
        }

        Queue<DT>* temp = this->next; //Create new queue without front element
        DT ReturnJob = this->JobPointer; //Return job
        this->JobPointer = temp->JobPointer;
        this->next = temp->next;
        delete temp;
        size--;
        return ReturnJob;
    }

bool isEmpty() { //Useful for edge cases
    return (size == 0);
}

void display() { //Displays all elements of the queue in order
    Queue<DT>* temp = this;
    while (temp != nullptr) {
        if (temp->JobPointer) {
            temp->JobPointer->display();
            cout << endl;
        }
        temp = temp->next;
    }
}
};

template <class DT>
class NovelQueue {
public:
    Queue<DT>* front;           // Pointer to the front of the queue
    Queue<DT>** NodePointers;       // Array of pointers to Queue nodes
    int size;                   // Number of elements in the queue
    int capacity;               // Capacity of the queue

    NovelQueue() : front(nullptr), NodePointers(nullptr), size(0), capacity(10) {
        NodePointers = new Queue<DT>*[capacity];
    }

    ~NovelQueue() {
        delete[] NodePointers;
        while (front != nullptr) {
            dequeue();
        }
    }

    void enqueue(DT newJob) {
    // Check for duplicate job ID
    for (int i = 0; i < size; ++i) {
        if (NodePointers[i]->JobPointer->jobID == newJob->jobID) {
            // If a job with the same jobID is found, print a message and return early
            cout << "Job ID " << newJob->jobID << " already exists!" << endl;
            return;  // Exit the method without enqueueing or printing
        }
    }

    // Resize if necessary
    if (size == capacity) {
        resize(capacity * 2);
    }
    // Create new node
    Queue<DT>* newNode = new Queue<DT>();
    newNode->JobPointer = newJob;
    newNode->next = nullptr;

// Add to the queue
if (front == nullptr) {
    front = newNode;
} else {
    Queue<DT>* temp = front;
    while (temp->next != nullptr) {
        temp = temp->next;
    }
    temp->next = newNode;
}

NodePointers[size] = newNode;
size++;

// Sort by job ID if necessary
for (int i = size - 1; i > 0 && NodePointers[i]->JobPointer->jobID < NodePointers[i - 1]->JobPointer->jobID; --i) {
    std::swap(NodePointers[i], NodePointers[i - 1]);
}
}

DT dequeue() {
    if (front == nullptr) {
        return DT(); //returns an instance of a null DT
    }

    Queue<DT>* temp = front;
    front = front->next;
    DT job = temp->JobPointer;
    delete temp;

    for (int i = 0; i < size - 1; i++) {
        NodePointers[i] = NodePointers[i + 1];
    }
    size--;

    if (size > 0 && size <= capacity / 2) { //resizes queue if enough spots are open
        resize(capacity / 2);
    }
    return job;
}

void modify(int jobID, int new_priority, int new_job_type, int new_cpu_time_consumed, int new_memory_consumed) {
    for (int i = 0; i < size; i++) {
        if (NodePointers[i]->JobPointer->jobID == jobID) {
            NodePointers[i]->JobPointer->priority = new_priority;
            NodePointers[i]->JobPointer->job_type = new_job_type;
            NodePointers[i]->JobPointer->cpu_time_consumed = new_cpu_time_consumed;
            NodePointers[i]->JobPointer->memory_consumed = new_memory_consumed;
        }
    }
}

void promote(int jobID, int positions) {
    int jobIndex = -1;
    for (int i = 0; i < size; ++i) { //Loop to find the right job
        if (NodePointers[i]->JobPointer->jobID == jobID) {
            jobIndex = i;
            break;
        }
    }
    int newIndex = std::max(0, jobIndex - positions);
    Queue<DT>* nodeToPromote = NodePointers[jobIndex];

    for (int j = jobIndex; j > newIndex; --j) {
        NodePointers[j] = NodePointers[j - 1];
    }

        NodePointers[newIndex] = nodeToPromote;

        if (newIndex == 0) {
            front = NodePointers[0];
        }

        for (int i = 0; i < size - 1; ++i) {
            NodePointers[i]->next = NodePointers[i + 1];
        }
        NodePointers[size - 1]->next = nullptr;
    }

    void change(int jobID, int field_index, int new_value) {
    bool changed = false; //Useful for edge cases

for (int i = 0; i < size; ++i) { //Loop to find right job
if (NodePointers[i]->JobPointer->jobID == jobID) {
cout << "Changed Job ID " << jobID << " field " << field_index << " to " << new_value << ":" << endl;
switch (field_index) {
    case 1:
        NodePointers[i]->JobPointer->priority = new_value;
        changed = true;
        break;
    case 2:
        NodePointers[i]->JobPointer->job_type = new_value;
        changed = true;
        break;
    case 3:
        NodePointers[i]->JobPointer->cpu_time_consumed = new_value;
        changed = true;
        break;
    case 4:
        NodePointers[i]->JobPointer->memory_consumed = new_value;
        changed = true;
        break;
    default:
        cout << "Invalid field_index!" << endl;
        return;
        
}

    if (changed) { //If the job was found
        NodePointers[i]->JobPointer->display();
        cout << endl;
        cout << "Jobs after changing field: " << endl;
        display();
        return; // Stop searching after the job has been changed
    }
}
    }

    //If the job was not found
    if (!changed) {
        cout << "Job with ID " << jobID << " not found in the queue." << endl;
    }
}


    NovelQueue<DT>* reorder(int attribute_index) {
        NovelQueue<DT>* reorderedQueue = new NovelQueue<DT>();

// Sorting the NodePointers array based on the chosen attribute.
for (int i = 0; i < size - 1; ++i) {
for (int j = 0; j < size - i - 1; ++j) {
    bool needSwap = false;
    // Sort by Job ID when attribute_index is 1
    if (attribute_index == 1) {
        if (NodePointers[j]->JobPointer->jobID > NodePointers[j + 1]->JobPointer->jobID) {
            needSwap = true;
        }
    } else if (attribute_index == 2) {
        if (NodePointers[j]->JobPointer->priority > NodePointers[j + 1]->JobPointer->priority) {
            needSwap = true;
        }
    } else if (attribute_index == 3) {
        if (NodePointers[j]->JobPointer->cpu_time_consumed > NodePointers[j + 1]->JobPointer->cpu_time_consumed) {
            needSwap = true;
        }
    }
    if (needSwap) {
        std::swap(NodePointers[j], NodePointers[j + 1]);
    }
}
}

for (int i = 0; i < size; ++i) {
    if (NodePointers[i]) {
        reorderedQueue->enqueue(NodePointers[i]->JobPointer);
    }
}

while (size > 0) {
    this->dequeue();  // Ensures the old queue is cleared properly
}

return reorderedQueue;
}

void display() {
Queue<DT>* temp = front;
while (temp != nullptr) {
    if (temp->JobPointer) {
        temp->JobPointer->display();
        cout << endl;
    }
    temp = temp->next;
}
}

void listJobs() {
    for (int i = 0; i < size; i++) {
        NodePointers[i]->JobPointer->display();   // Display the job information
        cout << endl;
    }
}

void resize(int newCapacity) { //If space runs out, or we have too much
    Queue<DT>** newNodePointers = new Queue<DT>*[newCapacity];
    for (int i = 0; i < size; i++) {
        newNodePointers[i] = NodePointers[i];
    }
    delete[] NodePointers;
    NodePointers = newNodePointers;
    capacity = newCapacity;
}

int count() {
    return size;
}
};

int main() {
    int n;
    cin >> n;

    NovelQueue<CPUJob*>* myNovelQ = new NovelQueue<CPUJob*>();

    char command;
    int jobID, priority, job_type, cpu_time_consumed, memory_consumed;
    int new_priority, new_job_type, new_cpu_time_consumed, new_memory_consumed;
    int field_index, new_value;
    int positions;
    int attribute_index;

    for (int i = 0; i < n; ++i) {
cin >> command;

switch (command) {
case 'A': {
    // Read the job details
    cin >> jobID >> priority >> job_type;
    cin >> cpu_time_consumed >> memory_consumed;
    
    // Create a new job
    CPUJob* newJob = new CPUJob(jobID, priority, job_type, cpu_time_consumed, memory_consumed);
    
    // Track the size before enqueue
    int oldSize = myNovelQ->count();
    
    // Enqueue the new job
    myNovelQ->enqueue(newJob);
    
    // Track the size after enqueue
    int newSize = myNovelQ->count();
    
    // Only display the job and queue if the size increased (job was successfully added)
    if (newSize > oldSize) {
        cout << "Enqueued Job:" << endl;
        newJob->display();
        cout << endl << "Jobs after enqueue:" << endl;
        myNovelQ->display();
    }
    
    break;
}
case 'R': {
    CPUJob* removedJob = myNovelQ->dequeue();
    if (removedJob) {
        cout << "Dequeued Job:" << endl;
        removedJob->display();
        cout << endl << "Jobs after dequeue:" << endl;
        myNovelQ->display();
        delete removedJob;
    }
    break;
}
case 'M': {
    cin >> jobID >> new_priority >> new_job_type;
    cin >> new_cpu_time_consumed >> new_memory_consumed;
    myNovelQ->modify(jobID, new_priority, new_job_type, new_cpu_time_consumed, new_memory_consumed);
    cout << "Modified Job ID " << jobID << ":" << endl;
    for (int i = 0; i < myNovelQ->size; ++i) {
        if (myNovelQ->NodePointers[i]->JobPointer->jobID == jobID) {
            myNovelQ->NodePointers[i]->JobPointer->display();
            cout << endl;
            break;
        }
    }
    cout << "Jobs after modification: " << endl;
    myNovelQ->display();
    break;
}
case 'C': {
    cin >> jobID >> field_index >> new_value;
    myNovelQ->change(jobID, field_index, new_value);
    break;
}
case 'P': {
    cin >> jobID >> positions;
    myNovelQ->promote(jobID, positions);
    cout << "Promoted Job ID " << jobID << " by " << positions << " Position(s):" << endl;

    for (int i = 0; i < myNovelQ->size; ++i) {
        if (myNovelQ->NodePointers[i]->JobPointer->jobID == jobID) {
            myNovelQ->NodePointers[i]->JobPointer->display();
            cout << endl;
            break;
        }
    }

    cout << "Jobs after promotion:" << endl;
    myNovelQ->display();
    break;
}
case 'O': {
    cin >> attribute_index;
    NovelQueue<CPUJob*>* reorderedQueue = myNovelQ->reorder(attribute_index);
    
    delete myNovelQ;  // Delete the old queue
    myNovelQ = reorderedQueue;  // Point myNovelQ to the reordered queue
    
    cout << "Reordered Queue by attribute " << attribute_index << ":" << endl;
    myNovelQ->display();
    break;
}
case 'D': {
    cout << "Displaying all jobs in the queue:" << endl;
    myNovelQ->display();
    break;
}
case 'N': {
    cout << "Number of elements in the queue: " << myNovelQ->count() << endl;
    break;
}
case 'L': {
    cout << "List of jobs sorted by job IDs:" << endl;
    myNovelQ->listJobs();
    break;
}
default:
    cout << "Invalid command!" << endl;
}
    }
     delete myNovelQ;
return 0;
}

/* 
 * CODE DOCUMENTATION
 * Project: CPU Job Queue Management System
 *
 * Overview:
 * The program implements a queue management system for CPU jobs using linked list and array structures. 
 * It defines two primary classes:
 * 
 * 1. **CPUJob** - encapsulates individual job attributes such as job ID, priority, job type, CPU time consumed, 
 *    and memory consumed.
 * 
 * 2. **NovelQueue** - a dynamic queue that allows multiple operations on CPUJob objects, including enqueue, 
 *    dequeue, job modification, promotion within the queue, reordering, displaying, and counting jobs.
 * 
 * Main Functionalities:
 * - **Enqueue**: Adds a new job to the end of the queue, ensuring no duplicate job IDs.
 * - **Dequeue**: Removes the front job in the queue and returns it.
 * - **Modify**: Updates multiple attributes of a job specified by its ID.
 * - **Change**: Alters a specific attribute of a job based on an attribute index.
 * - **Promote**: Moves a job up within the queue based on specified positions.
 * - **Reorder**: Sorts the queue based on a specified job attribute.
 * - **Display**: Outputs all jobs in the queue in their current order.
 * - **Count**: Returns the current number of jobs in the queue.
 *
 * Documentation:
 * - This program processes user commands to dynamically manipulate the queue, allowing real-time updates on job management. 
 * - Each command input modifies the queue and immediately reflects any changes in the queue’s structure, which is displayed to the user.
 * 
 * LLM and GitHub Copilot Usage Documentation:
 * 
 * This section details the questions, prompts, and incremental development strategies used when utilizing LLM tools or GitHub Copilot.
 * 
 * 1. Prompts and Suggestions:
 *    - Prompts used: "How to implement enqueue for NovelQueue?", "Debugging dequeue functionality in a linked queue", 
 *      "Can you suggest improvements for the modify function in NovelQueue?"
 * 
 * 2. Rationale for Prompt Choices:
 *    - The suggestions from the AI were essential for guiding method structuring and ensuring consistency in method integration.
 *    - For instance, the prompt about enqueueing helped validate the conditions for adding new jobs to avoid duplicates, 
 *      which saved time by catching potential errors in real-time.
 * 
 * 3. Incremental Development:
 *    - Development was approached step-by-step, using AI prompts to build out a basic structure for each class and incrementally 
 *      refining individual methods for consistency.
 *    - Examples include using AI to generate method headers, which were expanded to manage the linked structure and validate 
 *      input data in a more robust way.
 * 
 * Debugging and Testing Plan:
 * 
 * This plan outlines the specific tests conducted, issues encountered, and verification processes used to ensure robust functionality.
 * 
 * 1. Specific Tests:
 *    - Enqueue/Dequeue Operations: Ensured that the enqueue function correctly adds new jobs and maintains order. 
 *      Dequeue was tested to confirm that front jobs are removed, and memory is managed.
 *    - Modify Function: Checked that modify successfully changes job attributes for jobs with valid IDs and does not alter 
 *      unrelated jobs.
 *    - Edge Case Testing: Verified that the program handles an empty queue gracefully during dequeue attempts and manages 
 *      duplicate IDs correctly.
 * 
 * 2. Issues and Resolutions:
 *    - Issue: The program occasionally skipped the enqueue function for specific jobs with input2.txt and sometimes it also skipped jobs in input1.txt.
 *    - Resolution: Added debugging output statements to check conditions, revealing an issue in the duplicate-check logic and the reordering method that 
 *      caused the enqueue to fail. Corrected the check, allowing all jobs to enqueue as expected.
 * 
 * 3. Verification:
 *    - Command-line Testing: Validated each command by running a series of inputs directly in the terminal.
 *    - Automated Testing Suite: Created test cases for all operations (enqueue, dequeue, modify, change, promote, reorder) 
 *      and observed output consistency.
 *    - Continuous Monitoring: Printed the queue state after each operation to confirm that the expected changes occurred.
 * 
 * Reflection on AI Assistance:
 * - Questioning AI tools throughout the coding process deepened understanding of queue management, memory handling, 
 *   and error resolution. Examples include insights into structuring linked nodes and using pointers effectively.
 * - The AI suggestions informed coding best practices such as pointer safety, memory allocation, and encapsulation, 
 *   providing strategies to optimize queue operations without sacrificing readability.
 * 
 * Overall, using targeted questions and AI-assisted debugging allowed for a cleaner, more efficient codebase while 
 * ensuring robust error handling and proper memory management.
 * 
 * Note: The program accurately processes commands from input files, and all the issues faced during development have been resolved.
 */