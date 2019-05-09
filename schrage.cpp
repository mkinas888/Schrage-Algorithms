#include <iostream>
#include <fstream>
#include <queue>
#include <chrono>
#include <unistd.h>

using namespace std;

// IMPORTANT !!
//Compile with -std=c++11 flag due to chrono and stoi usage

 // Class created to store Task parameters
class Task {
public:
    int r, p, q, C, id, delay;

    // assignement operator overload
    Task & operator = (const Task& task) {
        this->r = task.r;
        this->p = task.p;
        this->q = task.q;
        this->C = task.C;
        this->id = task.id;
        this->delay = task.delay;
        return * this;
    }
};

// structure made simply to compare task by given parameter r or q
struct compareR {
    bool operator()(const Task& Task1, const Task& Task2)
    {
        if(Task1.r > Task2.r) {
            return true;
        } else if(Task1.r < Task2.r) {
            return false;
        } else if(Task1.r == Task2.r && Task1.id < Task2.id) {
            return false;
        } else {
            return true;
        }
    }
};

struct compareQ {
    bool operator()(const Task& Task1, const Task& Task2)
    {
        if(Task1.q < Task2.q) {
            return true;
        } else if(Task1.q > Task2.q) {
            return false;
        } else if(Task1.r == Task2.r && Task1.id < Task2.id) {
            return false;
        } else {
            return true;
        }
    }
};

// function to display priority queue in order to given structure
void showpq(priority_queue <Task, vector<Task>, compareR> pq)
{
    priority_queue <Task, vector<Task>, compareR> PQ = pq;
    while (!PQ.empty())
    {
        cout << PQ.top().p << " ";
        PQ.pop();
    }
    cout << endl;
}

// global table for storing optimal permutation
Task piTable[100];

/*******************************************************************************************
*       Psuedo code used to implement both schrageMethod and preSchrageMethod taken from   *
*       http://dominik.zelazny.staff.iiar.pwr.wroc.pl/materialy/Algorytm_Schrage.pdf       *
*                                                                                          *
*******************************************************************************************/

int schrageMethod(int n, Task taskTable[], Task piTable[]) {
    Task e;
    int t=0, k=0, Cmax=0;
    priority_queue<Task, vector<Task>, compareR> N;
    priority_queue<Task, vector<Task>, compareQ> G;
    for(int i=0;i<n;i++) {
        N.push(taskTable[i]);
        // showpq(N);
    }
    //until both queues are not empty
    while(!(G.empty()) || !(N.empty())) {
        // if N has any elements and there is ready task
        while(!(N.empty()) && N.top().r <= t) {
            e = N.top();
            G.push(e);
            N.pop();
        }
        // if there is no task ready
        if(G.empty()) {
            // wait until first task is ready
            t = N.top().r;
        } else {
            e = G.top();
            G.pop();
            // if task is ready assign it to permutation table
            piTable[k] = e;
            // update time
            t = t + e.p;
            Cmax = max(Cmax, t + e.q);
            piTable[k].C = t;
            k++;
        }
    }
    return Cmax;
}

int preSchrageMethod1(int n, Task taskTable[], Task piTable[]) {
    Task e, l;
    l.p = 0;
    l.r = 0;
    l.q = 99999999;
    int t =0, Cmax=0;
    priority_queue<Task, vector<Task>, compareR> N;
    priority_queue<Task, vector<Task>, compareQ> G;
    //priority_queue<Task, vector<Task>, compareQ> M;
    for(int i=0;i<n;i++) {
        N.push(taskTable[i]);
    }
    while(!(G.empty()) || !(N.empty())) {
        if(G.empty()) {
            t = N.top().r;
        }
        while(!(N.empty()) && N.top().r <= t) {
            e = N.top();
            G.push(e);
            N.pop();
            if(e.q > l.q) {
                l.p = t - e.r;
                t = e.r;
            }
            if(l.p > 0) {
                G.push(l);
            }
        }
        e = G.top();
        l = e;
        G.pop();
        t = t + e.p;
        Cmax = max(Cmax, t + e.q);
    }
    return Cmax;
}

int preSchrageMethod(int n, Task taskTable[], Task piTable[]) {
    Task e, l;
    int t =0, Cmax=0;
    priority_queue<Task, vector<Task>, compareR> N;
    priority_queue<Task, vector<Task>, compareQ> G;
    //priority_queue<Task, vector<Task>, compareQ> M;
    for(int i=0;i<n;i++) {
        N.push(taskTable[i]);
    }
    while(!(G.empty()) || (!N.empty())) {
        while((!N.empty()) && (N.top().r <= t)) {
            e = N.top();
            G.push(e);
            N.pop();
            if(G.top().q > l.q) {
                l.p = t - e.r;
                t = e.r;
                if(l.p > 0) {
                    G.push(l);
                }
            }
        }
        if(G.empty()) {
            t = N.top().r;
        } else {
            e = G.top();
            l = e;
            G.pop();
            t = t + e.p;
            Cmax = max(Cmax, t + e.q);
        }
    }
    return Cmax;
}


int countCmax(int n, Task taskTable[]) {
    int t=0, Cmax=0;
    for(int i=0;i<n;i++){
        t += taskTable[i].p;
        t = max(t, taskTable[i].r + taskTable[i].p);
        Cmax = max(Cmax, t + taskTable[i].q);
    }
    return Cmax;
}

// simple function to swap two tasks by their ids
void swapTasks(int task1Id, int task2Id, Task taskTable[]) {
    Task temp;
    temp = taskTable[task1Id];
    taskTable[task1Id] = taskTable[task2Id];
    taskTable[task2Id] = temp;
}

int main () {
    // start calculating time
    auto start = chrono::steady_clock::now();
    ifstream data;
    data.open("rpq.data.txt");
    int n, Cmax, CmaxSum = 0;
    string str, dataName = "data.00", part2 = ":";
    string dataNames[9];
    for(int i = 0; i<9;i++) {
        dataNames[i] = dataName + to_string(i) + part2;
    }
    for(int i = 0;i<9;i++) {
        while(str != dataNames[i]) {
        // while(str != "data.003:"){
            data >> str;
        }
        data >> str;
        n = stoi(str);
        Task taskTable1[n-1];
        // load dataset to each taskTable
        for(int j=0;j<n;j++) {
            data >> taskTable1[j].r >> taskTable1[j].p >> taskTable1[j].q;
            taskTable1[j].id = j+1;
        }

        // Cmax = countCmax(n, piTable);
        // cout << endl;
        // cout << dataNames[i] << endl;
        cout << "Cmax schrpmtn : " << endl;
        cout << preSchrageMethod(n, taskTable1, piTable) << endl << endl;
        // cout << "Cmax : " << endl;
        // cout << schrageMethod(n, taskTable1, piTable) << endl;
        // cout << "Optymalna kolejność uszeregowania zadań :" << endl;
        // for(int i=0;i<n;i++) {
        //     cout << piTable[i].id << " ";
        // }
        // cout << endl;
    }

    auto end = chrono::steady_clock::now();
	cout << "Elapsed time in seconds : "
		<< chrono::duration_cast<chrono::seconds>(end - start).count()
		<< " s" << endl;
}