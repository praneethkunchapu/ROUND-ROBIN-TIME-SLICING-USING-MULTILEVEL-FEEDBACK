#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>

using namespace std;

struct Process {
    int pid;
    int burstTime;
    int arrivalTime;
    int remainingTime;
    int queueLevel = 0;  // Start at highest priority (level 0)
    int waitingTime = 0;
    int completionTime = 0;
};

bool arrivalTimeComparator(const Process &a, const Process &b) {
    return a.arrivalTime < b.arrivalTime;
}

void printProcess(Process p) {
    cout << "PID: " << p.pid << ", Burst Time: " << p.burstTime 
         << ", Arrival Time: " << p.arrivalTime << ", Remaining Time: " << p.remainingTime 
         << ", Queue Level: " << p.queueLevel << ", Waiting Time: " << p.waitingTime 
         << ", Completion Time: " << p.completionTime << endl;
}

int main() {
    int n; // Number of processes
    cout << "Enter the number of processes: ";
    cin >> n;

    vector<Process> processes(n);
    for (int i = 0; i < n; ++i) {
        cout << "Enter Burst Time and Arrival Time for process " << (i + 1) << ": ";
        cin >> processes[i].burstTime >> processes[i].arrivalTime;
        processes[i].pid = i + 1;  // Assigning unique PIDs
        processes[i].remainingTime = processes[i].burstTime;
    }

    // Time quantum for each queue level
    int quantum[2] = {4, 8};  

    // Sort processes by arrival time
    sort(processes.begin(), processes.end(), arrivalTimeComparator);

    vector<queue<Process>> queues(2); // Two queues for MLFQ
    int currentTime = 0, completed = 0;
    bool allArrived = false;

    while (completed != processes.size()) {
        // Add newly arrived processes to the highest priority queue (level 0)
        if (!allArrived) {
            for (Process &p : processes) {
                if (p.arrivalTime <= currentTime && p.remainingTime > 0 && p.queueLevel == 0) {
                    queues[0].push(p);
                }
            }
            allArrived = (currentTime >= processes.back().arrivalTime);
        }

        bool executed = false;
        
        // Check highest priority queue first
        for (int level = 0; level < 2; ++level) {
            if (!queues[level].empty()) {
                Process currentProcess = queues[level].front();
                int timeSlice = min(quantum[level], currentProcess.remainingTime);
                
                currentTime += timeSlice;
                currentProcess.remainingTime -= timeSlice;
                
                if (currentProcess.remainingTime <= 0) {
                    currentProcess.completionTime = currentTime;
                    currentProcess.waitingTime = currentProcess.completionTime - currentProcess.burstTime - currentProcess.arrivalTime;
                    completed++;

                    // Update the process in the original vector
                    for (Process &p : processes) {
                        if (p.pid == currentProcess.pid) {
                            p = currentProcess;
                            break;
                        }
                    }
                    queues[level].pop();
                } else {
                    queues[level].pop();
                    currentProcess.queueLevel++;
                    queues[min(1, currentProcess.queueLevel)].push(currentProcess);  // Ensure we don't exceed 1 for queue level
                }
                executed = true;
                break;  // Only one process per time slice in this implementation
            }
        }

        // If no process was executed, move time forward
        if (!executed) currentTime++;
    }

    // Output results
    cout << "Process\tBurst Time\tArrival Time\tCompletion Time\tWaiting Time\n";
    for (const Process &p : processes) {
        cout << p.pid << "\t\t" << p.burstTime << "\t\t" << p.arrivalTime 
             << "\t\t" << p.completionTime << "\t\t" << p.waitingTime << endl;
    }

    return 0;
}