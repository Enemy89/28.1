#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <mutex>
#include <map>

std::mutex mtx;

class Swimmer {
    std::string name;
    double speed;
    double distance;
    std::map<double, std::string>& raceResults;
    std::thread swimThread;
public:
    Swimmer(const std::string& inName, double inSpeed,  std::map<double, std::string>& resultsMap)
            : name(inName), speed(inSpeed), distance(0), raceResults(resultsMap) {}

    void startSwimming() {
        swimThread = std::thread(&Swimmer::swim, this);
    }

    void joinSwimThread() {
        if (swimThread.joinable()) {
            swimThread.join();
        }
    }

    void swim() {
        while (distance < 100) {
            distance += speed;
            if (distance >= 100) {
                distance = 100;
            }
            mtx.lock();
            std::cout << name << " distance - " << distance << "m" << std::endl;
            mtx.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        double place = 100 / speed;
        raceResults[place] = name;
    }
};

int main() {
    std::vector<Swimmer*> swimmers;
    std::map<double, std::string> results;
    std::string user_name;
    double user_speed;

    for (int i = 0; i < 6; ++i) {
        std::cout << "Enter name and speed of swimmer: " << std::endl;
        std::cin >> user_name >> user_speed;
        Swimmer* swimmer = new Swimmer(user_name, user_speed, results);
        swimmers.push_back(swimmer);
    }

    for (int i = 0; i < swimmers.size(); ++i) {
        mtx.lock();
        swimmers[i]->startSwimming();
        mtx.unlock();
    }

    for (int i = 0; i < swimmers.size(); ++i) {
        swimmers[i]->joinSwimThread();
    }

    std::cout << "\nRace Results:" << std::endl;
    int count =1;
    mtx.lock();
    for (std::map<double,std::string>::iterator it=results.begin(); it!=results.end(); ++it) {
        std::cout << it->second << " finished at place " << count++ << std::endl;
    }
    mtx.unlock();

    for (int i = 0; i < swimmers.size(); ++i) {
        delete swimmers[i];
    }
}
