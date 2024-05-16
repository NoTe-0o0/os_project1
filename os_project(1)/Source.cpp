#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

using namespace std;

struct Task {
    int id;
    int burst;
    int arrival;
    int remaining;
    int finish;
    int waiting;
    int turnaround;
    bool isFinished;
};

void fcfs(vector<Task>& tasks, int contextSwitch) {
    int currentTime = 0;
    cout << "\nGantt Chart:\n";
    for (auto& t : tasks) {
        if (currentTime != 0) {
            currentTime += contextSwitch;
        }
        t.waiting = currentTime;
        currentTime += t.burst;
        t.finish = currentTime;
        t.turnaround = t.finish;
        cout << "| Task" << t.id << " (" << t.waiting << "-" << t.finish << ") ";
    }
    cout << "|\n";

    cout << "Task\tFinish Time\tWaiting Time\tTurnaround Time\n";
    for (const auto& t : tasks) {
        cout << "Task" << t.id << "\t" << t.finish << "\t\t" << t.waiting << "\t\t" << t.turnaround << "\n";
    }

    double totalBurst = 0;
    for (const auto& t : tasks) {
        totalBurst += t.burst;
    }
    double cpuUtilization = (totalBurst / (currentTime + contextSwitch * (tasks.size() - 1))) * 100;
    cout << "CPU Utilization = " << cpuUtilization << "%" << endl;
}

void srt(vector<Task>& tasks, int contextSwitch) {
    int currentTime = 0;
    int completed = 0;
    int lastId = 0;
    int startPrint = 0;
    cout << "\nGantt Chart:\n";

    while (completed < tasks.size()) {
        int idx = -1;
        int minTime = INT_MAX;

        for (int i = 0; i < tasks.size(); i++) {
            if (tasks[i].arrival <= currentTime && !tasks[i].isFinished &&
                tasks[i].remaining < minTime) {
                minTime = tasks[i].remaining;
                idx = i;
            }
        }

        if (idx == -1) {
            currentTime++;
        }
        else {
            if (lastId != tasks[idx].id && lastId != 0) {
                currentTime += contextSwitch;
            }
            if (lastId != tasks[idx].id) {
                if (lastId != 0) {
                    cout << "|Task" << lastId << "(" << startPrint << "-" << currentTime << ")";
                }
                startPrint = currentTime;
                lastId = tasks[idx].id;
            }
            tasks[idx].remaining--;
            currentTime++;

            if (tasks[idx].remaining == 0) {
                tasks[idx].finish = currentTime;
                tasks[idx].turnaround = currentTime - tasks[idx].arrival;
                tasks[idx].waiting = tasks[idx].turnaround - tasks[idx].burst;
                tasks[idx].isFinished = true;
                completed++;
            }
        }
    }
    if (lastId != 0) {
        cout << "|Task" << lastId << "(" << startPrint << "-" << currentTime << ")";
    }
    cout << "|\n";

    cout << "Task\tFinish Time\tWaiting Time\tTurnaround Time\n";
    for (const auto& t : tasks) {
        cout << "Task" << t.id << "\t" << t.finish << "\t\t" << t.waiting << "\t\t" << t.turnaround << "\n";
    }
    double totalBurst = 0;
    for (const auto& t : tasks) {
        totalBurst += t.burst;
    }
    double cpuUtilization = (totalBurst / (currentTime + contextSwitch * (tasks.size() - 1))) * 100;
    cout << "CPU Utilization = " << cpuUtilization << "%" << endl;
}

void RR(vector<Task>& tasks, int quantum, int contextSwitch) {
    queue<int> q;
    int currentTime = 0;
    int lastId = 0;
    int startPrint = 0;
    cout << "\nGantt Chart:\n";

    for (int i = 0; i < tasks.size(); i++) {
        if (tasks[i].arrival <= currentTime) {
            q.push(i);
        }
    }

    while (!q.empty()) {
        int idx = q.front();
        q.pop();

        if (lastId != 0 && lastId != tasks[idx].id) {
            currentTime += contextSwitch;
        }

        int runTime = min(quantum, tasks[idx].remaining);
        cout << "| Task" << tasks[idx].id << "(" << currentTime << "-" << (currentTime + runTime) << ")";
        tasks[idx].remaining -= runTime;
        currentTime += runTime;

        vector<int> tempQueue;
        while (!q.empty()) {
            tempQueue.push_back(q.front());
            q.pop();
        }

        for (int i = 0; i < tasks.size(); i++) {
            if (!tasks[i].isFinished && tasks[i].arrival > tasks[idx].arrival && tasks[i].arrival <= currentTime && find(tempQueue.begin(), tempQueue.end(), i) == tempQueue.end()) {
                tempQueue.push_back(i);
            }
        }

        if (tasks[idx].remaining > 0) {
            tempQueue.push_back(idx);
        }
        else {
            tasks[idx].isFinished = true;
            tasks[idx].finish = currentTime;
            tasks[idx].turnaround = currentTime - tasks[idx].arrival;
            tasks[idx].waiting = tasks[idx].turnaround - tasks[idx].burst;
        }

        for (int id : tempQueue) {
            q.push(id);
        }
    }

    cout << "|\n";
    cout << "Task\tFinish Time\tWaiting Time\tTurnaround Time\n";
    for (const auto& t : tasks) {
        cout << "Task" << t.id << "\t" << t.finish << "\t\t" << t.waiting << "\t\t" << t.turnaround << "\n";
    }
    double totalBurst = 0;
    for (const auto& t : tasks) {
        totalBurst += t.burst;
    }
    double cpuUtilization = (totalBurst / (currentTime + contextSwitch * (tasks.size() - 1))) * 100;
    cout << "CPU Utilization = " << cpuUtilization << "%" << endl;
}

double calculateCPUUtilization(const vector<Task>& tasks, int contextSwitch, int currentTime) {
    double totalBurst = 0;
    for (const auto& t : tasks) {
        totalBurst += t.burst;
    }
    double cpuUtilization = (totalBurst / (currentTime + contextSwitch * (tasks.size() - 1))) * 100;
    return cpuUtilization;
}

int main() {
    string filePath = "C:/Users/97056/Desktop/os_project(1)/os_project(1)/os_input.txt";
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

    int numberOfTasks, contextSwitch, quantum;

    inFile >> numberOfTasks >> contextSwitch >> quantum;

    vector<Task> tasks(numberOfTasks);
    for (int i = 0; i < numberOfTasks; i++) {
        inFile >> tasks[i].arrival >> tasks[i].burst;
        tasks[i].id = i + 1;
        tasks[i].remaining = tasks[i].burst;
        tasks[i].isFinished = false;
    }

    int currentTime = 0;
    switch (choice) {
    case 1:
        fcfs(tasks, contextSwitch);
        break;
    case 2:
        srt(tasks, contextSwitch);
        break;
    case 3:
        RR(tasks, quantum, contextSwitch);
        break;
    default:
        cout << "Invalid choice. Please select a valid option (1-3).\n";
        break;
    }

    return 0;
}
