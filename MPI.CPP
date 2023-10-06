#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <limits>
#include <ctime> 
using namespace std;


struct City {
    int id;
    double x, y;
};

double calculateDistance(const City& city1, const City& city2) {
    double dx = city1.x - city2.x;
    double dy = city1.y - city2.y;
    return sqrt(dx * dx + dy * dy);
}

double calculateTotalDistance(const vector<int>& route, const vector<City>& cities) {
    double totalDistance = 0.0;
    int numCities = route.size();

    for (int i = 0; i < numCities - 1; ++i) {
        totalDistance += calculateDistance(cities[route[i]], cities[route[i + 1]]);
    }

    totalDistance += calculateDistance(cities[route[numCities - 1]], cities[route[0]]);

    return totalDistance;
}

vector<int> generateRandomRoute(int numCities) {
    vector<int> route(numCities);
    for (int i = 0; i < numCities; ++i) {
        route[i] = i;
    }
    random_shuffle(route.begin(), route.end());
    return route;
}


vector<int> crossover(const vector<int>& parent1, const vector<int>& parent2) {
    int numCities = parent1.size();
    vector<int> child(numCities, -1);

    int startPos = rand() % numCities;
    int endPos = rand() % numCities;

    if (startPos > endPos) {
        swap(startPos, endPos);
    }

    for (int i = startPos; i <= endPos; ++i) {
        child[i] = parent1[i];
    }

    int currentIndex = 0;
    for (int i = 0; i < numCities; ++i) {
        if (child[i] == -1) {
            while (find(child.begin(), child.end(), parent2[currentIndex]) != child.end()) {
                currentIndex = (currentIndex + 1) % numCities;
            }
            child[i] = parent2[currentIndex];
            currentIndex = (currentIndex + 1) % numCities;
        }
    }

    return child;
}

void mutate(vector<int>& route, double mutationRate) {
    int numCities = route.size();
    for (int i = 0; i < numCities; ++i) {
        if (rand() / static_cast<double>(RAND_MAX) < mutationRate) {
            int j = rand() % numCities;
            swap(route[i], route[j]);
        }
    }
}


int main() {
    // Initialize MPI
    int numProcesses, rank;
    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    vector<City> cities;

    srand(42); 

    const string LIGHT_BLUE = "\033[94m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string RESET = "\033[0m";


        // Record the start time
    time_t startTime = time(nullptr);

    if (rank == 0) {
        ifstream inputFile("C:\\Users\\user\\Wilson\\TARUC\\Degree Year 3\\Sem 1\\BMCS3003 Distributed Systems and Parallel\\Assignment\\SubmissionVersion\\att532.tsp");
        if (!inputFile) {
            cerr << "Failed to open input file." << endl;
            return 1;
        }

        string line;

        // Skip lines until the problem data begins
        while (inputFile >> line) {
            if (line == "NODE_COORD_SECTION") {
                break;
            }
        }

        // Read the coordinates from the TSP file
        while (inputFile >> line) {
            if (line == "EOF") {
                break;
            }
            City city;
            inputFile >> city.id >> city.x >> city.y;
            cities.push_back(city);
        }
    }

    int numCities = cities.size();

    // Broadcast the number of cities to all processes
    MPI_Bcast(&numCities, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0) {
        cities.resize(numCities);
    }

    // Broadcast city data to all processes
    MPI_Bcast(&cities[0], numCities * sizeof(City), MPI_BYTE, 0, MPI_COMM_WORLD);

    // ... rest of your variables ...
    int populationSize = 100;
    int numGenerations = 100;
    double mutationRate = 0.01;

    vector<int> localBestRoute;
    double localBestDistance = numeric_limits<double>::max();
    
    vector<vector<int>> population(populationSize / numProcesses);  // Split population

    for (int i = 0; i < populationSize / numProcesses; ++i) {
        population[i] = generateRandomRoute(numCities);
    }

    for (int generation = 0; generation < numGenerations; ++generation) {
        
        // Evaluate fitness for local population
        vector<pair<int, double>> fitness;
        for (int i = 0; i < populationSize / numProcesses; ++i) {
            double distance = calculateTotalDistance(population[i], cities);
            fitness.push_back(make_pair(i, 1.0 / distance));
        }

        // Sort based on fitness
        sort(fitness.begin(), fitness.end(), [](const pair<int, double>& a, const pair<int, double>& b) {
            return a.second > b.second;
        });

        // Check if we have a new local best
        if (1.0 / fitness[0].second < localBestDistance) {
            localBestRoute = population[fitness[0].first];
            localBestDistance = 1.0 / fitness[0].second;
        }

        // Evolve the population
        vector<vector<int>> newPopulation(populationSize / numProcesses);

        newPopulation[0] = population[fitness[0].first];  // Elitism

        for (int i = 1; i < populationSize / numProcesses; ++i) {
            int parent1 = fitness[rand() % (populationSize / numProcesses)].first;
            int parent2 = fitness[rand() % (populationSize / numProcesses)].first;
            newPopulation[i] = crossover(population[parent1], population[parent2]);
            mutate(newPopulation[i], mutationRate);
        }

        population = newPopulation;

        // Periodically share the best route with all processes
        if (generation % 10 == 0) {
            double globalBestDistance;
            MPI_Allreduce(&localBestDistance, &globalBestDistance, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
            
            int senderRank = (localBestDistance == globalBestDistance) ? rank : 0;
            if(localBestDistance != globalBestDistance) {
                localBestRoute.resize(numCities);
            }
            MPI_Bcast(localBestRoute.data(), numCities, MPI_INT, senderRank, MPI_COMM_WORLD);

        }
    }

    // Gather the best routes and distances from all processes
    vector<double> allBestDistances(numProcesses);
    MPI_Gather(&localBestDistance, 1, MPI_DOUBLE, allBestDistances.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        int bestRank = distance(allBestDistances.begin(), min_element(allBestDistances.begin(), 
            allBestDistances.end()));

        cout << GREEN + "================================================" + RESET << endl;
        cout << GREEN + "           TRAVELING SALESMAN PROBLEM     " + RESET << endl;
        cout << GREEN + "================================================" + RESET << endl << endl;

        cout << YELLOW + "\nResults:" + RESET << endl;
        cout << LIGHT_BLUE + "Best route:\n";
        for (int city : localBestRoute) {
            cout << city << " ";
        }
        cout << RESET << "\n\n";
        cout << GREEN << "Total distance: " << allBestDistances[bestRank] << RESET << endl;

        time_t endTime = time(nullptr);
        // Calculate the duration and display it
        double duration = difftime(endTime, startTime);
        cout << YELLOW + "Time taken by function: " << duration << " seconds" + RESET << endl;

        cout << GREEN + "\nThank you for using the Genetic Algorithm TSP Solver!" + RESET << endl;
        cout << GREEN + "===========================================" + RESET << endl;

    }

    MPI_Finalize();
    return 0;
}
