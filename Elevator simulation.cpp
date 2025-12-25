#include <iostream>
#include <vector>
#include <set>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <limits>
#include <cmath>

using namespace std;

class Elevator {
public:
    int id;
    atomic<int> currentFloor;
    string direction;
    set<int> upRequests;
    set<int> downRequests;
    mutex mtx;
    atomic<bool> active;
    atomic<bool> emergencyStop;
    thread runner;

    Elevator(int elevatorId) : id(elevatorId), currentFloor(0), direction("IDLE"), active(true), emergencyStop(false) {
        runner = thread(&Elevator::operate, this);
    }

    void addRequest(int floor) {
        lock_guard<mutex> lock(mtx);
        if (floor > currentFloor) upRequests.insert(floor);
        else if (floor < currentFloor) downRequests.insert(floor);
        else upRequests.insert(floor);
    }

    void triggerEmergencyStop() {
        emergencyStop = true;
    }

    void resetEmergencyStop() {
        emergencyStop = false;
    }

    void stop() {
        active = false;
        if (runner.joinable()) runner.join();
    }

    void operate() {
        while (active) {
            if (emergencyStop) {
                direction = "EMERGENCY STOP";
                showStatus();
                this_thread::sleep_for(chrono::milliseconds(500));
                continue;
            }

            int nextStop = -1;
            string newDirection = "IDLE";

            {
                lock_guard<mutex> lock(mtx);
                int minDiff = numeric_limits<int>::max();
                for (int f : upRequests) {
                    int diff = abs(f - currentFloor);
                    if (diff < minDiff) {
                        minDiff = diff;
                        nextStop = f;
                        newDirection = "UP";
                    }
                }
                for (int f : downRequests) {
                    int diff = abs(f - currentFloor);
                    if (diff < minDiff) {
                        minDiff = diff;
                        nextStop = f;
                        newDirection = "DOWN";
                    }
                }
                direction = newDirection;
            }

            if (nextStop == -1) {
                direction = "IDLE";
                this_thread::sleep_for(chrono::milliseconds(300));
                continue;
            }

            while (currentFloor != nextStop && active && !emergencyStop) {
                if (nextStop > currentFloor) currentFloor++;
                else currentFloor--;

                showStatus();
                this_thread::sleep_for(chrono::milliseconds(400));
            }

            if (!emergencyStop) {
                showStatus();
                cout << "Elevator " << id << " opened doors at floor " << currentFloor << endl;
                this_thread::sleep_for(chrono::milliseconds(500));
                cout << "Doors closing...\n";
                {
                    lock_guard<mutex> lock(mtx);
                    upRequests.erase(currentFloor);
                    downRequests.erase(currentFloor);
                }
            }
        }
    }

    void showStatus() {
        cout << "[Elevator " << id << "] at floor: " << currentFloor << " | Status: " << direction << endl;
    }

    int distanceTo(int floor) {
        return abs(currentFloor - floor);
    }

    bool isIdle() {
        lock_guard<mutex> lock(mtx);
        return upRequests.empty() && downRequests.empty();
    }

    string getDirection() {
        lock_guard<mutex> lock(mtx);
        return direction;
    }
};

class ElevatorSystem {
public:
    vector<Elevator*> elevators;
    atomic<bool> running;

    ElevatorSystem(int count) : running(true) {
        for (int i = 0; i < count; ++i) elevators.push_back(new Elevator(i + 1));
    }

    ~ElevatorSystem() {
        for (auto e : elevators) {
            e->stop();
            delete e;
        }
    }

    void triggerEmergency() {
        for (auto e : elevators) e->triggerEmergencyStop();
    }

    void resetEmergency() {
        for (auto e : elevators) e->resetEmergencyStop();
    }

    void assignElevator(int floor) {
        int choice;
        cout << "\nAvailable Elevators:\n";
        for (auto e : elevators) {
            cout << "Elevator " << e->id << " at floor " << e->currentFloor << " (" << e->getDirection() << ")\n";
        }
        cout << "Choose elevator ID (0 for auto): ";
        while (!(cin >> choice) || choice < 0 || choice > static_cast<int>(elevators.size())) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Try again: ";
        }

        if (choice == 0) {
            Elevator* best = nullptr;
            int minDistance = numeric_limits<int>::max();
            for (auto e : elevators) {
                int dist = e->distanceTo(floor);
                string dir = e->getDirection();
                if (dist < minDistance && (e->isIdle() || dir == (floor > e->currentFloor ? "UP" : "DOWN"))) {
                    best = e;
                    minDistance = dist;
                }
            }
            if (best) best->addRequest(floor);
            else elevators[0]->addRequest(floor);
        } else {
            elevators[choice - 1]->addRequest(floor);
        }
    }

    void userInput(int topFloor) {
        int num;
        while (running) {
            cout << "\nMenu:\n1. Add Requests\n2. Emergency Stop\n3. Reset Emergency\n4. Exit\nChoose option: ";
            int option;
            if (!(cin >> option)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            if (option == 4) break;
            else if (option == 2) triggerEmergency();
            else if (option == 3) resetEmergency();
            else if (option == 1) {
                cout << "\nHow many floor requests? ";
                if (!(cin >> num) || num <= 0) continue;
                vector<int> requests(num);
                cout << "Enter " << num << " floor numbers: ";
                for (int i = 0; i < num; ++i) {
                    if (!(cin >> requests[i])) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid input. Try again: ";
                        --i;
                        continue;
                    }
                }
                for (int floor : requests) {
                    if (floor >= 0 && floor <= topFloor) assignElevator(floor);
                    else cout << "Invalid floor: " << floor << endl;
                }
            }
        }
        running = false;
    }
};

int main() {
    int elevatorCount; 
    int maxFloor;

    cout << "Enter number of elevators: ";
    while (!(cin >> elevatorCount) || elevatorCount <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter a positive number: ";
    }

    cout << "Enter highest floor number: ";
    while (!(cin >> maxFloor) || maxFloor <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter a positive number: ";
    }

    ElevatorSystem system(elevatorCount);
    cout << "Elevator System Running for floors 0 to " << maxFloor << "\n";
    system.userInput(maxFloor);

   return 0;
}