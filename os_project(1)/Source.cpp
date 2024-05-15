#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>

using namespace std;

struct Process {
    int id;
    int burstTime;
    int arrivalTime;
    int remainingTime;
    int finishTime;
    int waitingTime;
    int turnaroundTime;
    bool isFinished;
};

void fcfs(vector<Process>& processes, int csTime) {
    int currentTime = 0;
    cout << "\nGantt Chart:\n";
    for (auto& p : processes) {
        if (currentTime != 0) {
            currentTime += csTime;
        }
        p.waitingTime = currentTime;
        currentTime += p.burstTime;
        p.finishTime = currentTime;
        p.turnaroundTime = p.finishTime;
        cout << "| P" << p.id << " (" << p.waitingTime << "-" << p.finishTime << ") ";
    }
    cout << "|\n";

    cout << "Process\tFinish Time\tWaiting Time\tTurnaround Time\n";
    for (const auto& p : processes) {
        cout << "P" << p.id << "\t" << p.finishTime << "\t\t" << p.waitingTime << "\t\t" << p.turnaroundTime << "\n";
    }

    double totalBurstTime = 0;
    for (const auto& p : processes) {
        totalBurstTime += p.burstTime;
    }
    double cpuUtilization = (totalBurstTime / (currentTime + csTime * (processes.size() - 1))) * 100;
    cout << "CPU Utilization = " << cpuUtilization << "%" << endl;
}

void srt(vector<Process>& processes, int csTime) {
    int currentTime = 0;
    int completed = 0;
    int lastProcessId = 0;
    int startPrintTime = 0;
    cout << "\nGantt Chart:\n";

    while (completed < processes.size()) {
        int idx = -1;
        int minTime = numeric_limits<int>::max();

        for (int i = 0; i < processes.size(); i++) {
            if (processes[i].arrivalTime <= currentTime && !processes[i].isFinished &&
                processes[i].remainingTime < minTime) {
                minTime = processes[i].remainingTime;
                idx = i;
            }
        }

        if (idx == -1) {
            currentTime++;
        }
        else {
            if (lastProcessId != processes[idx].id && lastProcessId != 0) {
                currentTime += csTime;
            }
            if (lastProcessId != processes[idx].id) {
                if (lastProcessId != 0) {
                    cout << "|" << "P" << lastProcessId << "(" << startPrintTime << "-" << currentTime << ")";
                }
                startPrintTime = currentTime;
                lastProcessId = processes[idx].id;
            }
            processes[idx].remainingTime--;
            currentTime++;

            if (processes[idx].remainingTime == 0) {
                processes[idx].finishTime = currentTime;
                processes[idx].turnaroundTime = currentTime - processes[idx].arrivalTime;
                processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
                processes[idx].isFinished = true;
                completed++;
            }
        }
    }
    if (lastProcessId != 0) {
        cout << "|" << "P" << lastProcessId << "(" << startPrintTime << "-" << currentTime << ")";
    }
    cout << "|\n";

    cout << "Process\tFinish Time\tWaiting Time\tTurnaround Time\n";
    for (const auto& p : processes) {
        cout << "P" << p.id << "\t" << p.finishTime << "\t\t" << p.waitingTime << "\t\t" << p.turnaroundTime << "\n";
    }
    double totalBurstTime = 0;
    for (const auto& p : processes) {
        totalBurstTime += p.burstTime;
    }
    double cpuUtilization = (totalBurstTime / (currentTime + csTime * (processes.size() - 1))) * 100;
    cout << "CPU Utilization = " << cpuUtilization << "%" << endl;
}

void RR(vector<Process>& processes, int quantum, int csTime) {
    queue<int> q;
    int currentTime = 0;
    int lastProcessId = 0;
    int startPrintTime = 0;
    cout << "\nGantt Chart:\n";

    for (int i = 0; i < processes.size(); i++) {
        if (processes[i].arrivalTime <= currentTime) {
            q.push(i);
        }
    }

    while (!q.empty()) {
        int idx = q.front();
        q.pop();

        if (lastProcessId != 0 && lastProcessId != processes[idx].id) {
            currentTime += csTime;
        }

        int runTime = min(quantum, processes[idx].remainingTime);
        cout << "| P" << processes[idx].id << "(" << currentTime << "-" << (currentTime + runTime) << ")";
        processes[idx].remainingTime -= runTime;
        currentTime += runTime;

        vector<int> tempQueue;
        while (!q.empty()) {
            tempQueue.push_back(q.front());
            q.pop();
        }


        for (int i = 0; i < processes.size(); i++) {
            if (!processes[i].isFinished && processes[i].arrivalTime > processes[idx].arrivalTime && processes[i].arrivalTime <= currentTime && find(tempQueue.begin(), tempQueue.end(), i) == tempQueue.end()) {
                tempQueue.push_back(i);
            }
        }

        if (processes[idx].remainingTime > 0) {
            tempQueue.push_back(idx);
        }
        else {
            processes[idx].isFinished = true;
            processes[idx].finishTime = currentTime;
            processes[idx].turnaroundTime = currentTime - processes[idx].arrivalTime;
            processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
        }


        for (int id : tempQueue) {
            q.push(id);
        }
    }

    cout << "|\n";
    cout << "Process\tFinish Time\tWaiting Time\tTurnaround Time\n";
    for (const auto& p : processes) {
        cout << "P" << p.id << "\t" << p.finishTime << "\t\t" << p.waitingTime << "\t\t" << p.turnaroundTime << "\n";
    }
    double totalBurstTime = 0;
    for (const auto& p : processes) {
        totalBurstTime += p.burstTime;
    }
    double cpuUtilization = (totalBurstTime / (currentTime + csTime * (processes.size() - 1))) * 100;
    cout << "CPU Utilization = " << cpuUtilization << "%" << endl;
}







double calculateCPUUtilization(const vector<Process>& processes, int csTime, int currentTime) {
    double totalBurstTime = 0;
    for (const auto& p : processes) {
        totalBurstTime += p.burstTime;
    }
    double cpuUtilization = (totalBurstTime / (currentTime + csTime * (processes.size() - 1))) * 100;
    return cpuUtilization;
}

int main() {
    string filePath = "C:/Users/97056/Desktop/os_project(1)/os_input.txt";
    ifstream inFile(filePath);
    if (!inFile) {
        cerr << "Error opening file: " << filePath << endl;
        return 1;
    }

    int choice;
    cout << "Choose the scheduling algorithm:\n";
    cout << "1. First-Come First-Served (FCFS)\n";
    cout << "2. Shortest Remaining Time (SRT)\n";
    cout << "3. Round-Robin (RR)\n";
    cout << "Enter your choice (1-3): ";
    cin >> choice;

    int numberOfProcesses, csTime, quantum;

    inFile >> numberOfProcesses >> csTime >> quantum;

    vector<Process> processes(numberOfProcesses);
    for (int i = 0; i < numberOfProcesses; i++) {
        inFile >> processes[i].arrivalTime >> processes[i].burstTime;
        processes[i].id = i + 1;
        processes[i].remainingTime = processes[i].burstTime;
        processes[i].isFinished = false;
    }

    int currentTime = 0;
    switch (choice) {
    case 1:
        fcfs(processes, csTime);
        break;
    case 2:
        srt(processes, csTime);
        break;
    case 3:
        RR(processes, quantum, csTime);
        break;
    default:
        cout << "Invalid choice. Please select a valid option (1-3).\n";
        break;
    }

    return 0;
}

