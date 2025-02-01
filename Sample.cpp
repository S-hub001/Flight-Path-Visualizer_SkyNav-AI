#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cmath>
#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

////////////////////////// FLIGHT CLASS //////////////////////
class flight
{
public:
    string departureTime, cost, airline, depdate, destination, arrivalTime, city1, city2;

    flight(string dt, string c, string al, string depd, string dest, string at, string c1, string c2)
    {
        departureTime = dt;
        cost = c;
        airline = al;
        depdate = depd;
        destination = dest;
        arrivalTime = at;
        city1 = c1;
        city2 = c2;
    }

    void print()
    {
        cout << "Source --> " << city1;
        cout << " Destination: " << city2
            << ", Airline: " << airline
            << ", Cost: " << cost
            << ", Departure Time: " << departureTime
            << ", Arrival Time : " << arrivalTime
            << ", Date: " << depdate << endl;
    }
};

////////////////// FLIGHT NODE CLASS ///////////////////
class flightNode
{
public:
    string city;  // City name
    flightNode* next;  // Next city in adjacency list
    flight* flightInfo;  // Flight details between cities

    flightNode(string c)
    {
        city = c;
        next = NULL;
        flightInfo = NULL;
    }
};

////////////// Node representing each segment of the journey ///////////////
class JourneyNode
{
public:
    string sourceCity;
    string destinationCity;
    string layoverCity;
    int layoverDuration;  // Layover time in minutes
    flight* firstFlight;  // Pointer to the first flight
    flight* secondFlight; // Pointer to the second flight
    JourneyNode* next;    // Pointer to the next journey node

    // Constructor for initializing a journey node
    JourneyNode(string source, string destination, string layover = "", flight* first = nullptr, flight* second = nullptr, int layoverTime = 0) : sourceCity(source), destinationCity(destination), layoverCity(layover), layoverDuration(layoverTime), firstFlight(first), secondFlight(second), next(nullptr) {}
};

/////////// LinkedList class to manage the journey ///////////////
class LinkedList
{
public:
    JourneyNode* head;
    JourneyNode* tail;

    LinkedList() : head(nullptr), tail(nullptr) {}

    // Add a new journey segment
    void addSegment(string source, string destination, string layover, flight* firstFlight, flight* secondFlight, int layoverDuration) 
    {
        JourneyNode* newNode = new JourneyNode(source, destination, layover, firstFlight, secondFlight, layoverDuration);

        if (head == nullptr) 
        {
            head = tail = newNode;
        }
        else {
            tail->next = newNode;
            tail = newNode;
        }
        cout << "\nSegment added: " << source << " -> " << layover << " -> " << destination << endl;
    }

    // Remove a segment by source and destination
    void removeSegment(string source, string destination) 
    {
        if (head == nullptr)
        {
            cout << "\nJourney list is empty.\n";
            return;
        }

        JourneyNode* temp = head;
        JourneyNode* prev = nullptr;

        while (temp != nullptr)
        {
            if (temp->sourceCity == source && temp->destinationCity == destination)
            {
                if (prev == nullptr)
                {
                    // Head node to be removed
                    head = head->next;
                }
                else 
                {
                    prev->next = temp->next;
                }

                if (temp == tail)
                {
                    // Tail node to be removed
                    tail = prev;
                }

                delete temp;
                cout << "\nSegment removed: " << source << " -> " << destination << endl;
                return;
            }
            prev = temp;
            temp = temp->next;
        }

        cout << "\nSegment not found: " << source << " -> " << destination << endl;
    }

    // Display the journey
    void displayJourney() 
    {
        if (head == nullptr)
        {
            cout << "\nJourney is empty.\n";
            return;
        }

        JourneyNode* temp = head;
        while (temp != nullptr) 
        {
            cout << temp->sourceCity << " -> ";
            if (!temp->layoverCity.empty()) 
            {
                cout << temp->layoverCity << " -> ";
            }
            cout << temp->destinationCity;

            if (temp->layoverDuration > 0)
            {
                cout << " (Layover: " << temp->layoverDuration / 60 << " hours and " << temp->layoverDuration % 60 << " minutes)";
            }

            cout << endl;
            temp = temp->next;
        }
    }

    int calculateTotalCost(string layoverCity)
    {
        int totalCost = 0;
        JourneyNode* temp = head;

        // Calculate flight costs
        while (temp != nullptr)
        {
            if (temp->firstFlight != nullptr)
            {
                totalCost += stoi(temp->firstFlight->cost); // Assuming cost is stored as string
            }
            if (temp->secondFlight != nullptr)
            {
                totalCost += stoi(temp->secondFlight->cost);
            }
            temp = temp->next;
        }

        // Read hotel costs from file
        ifstream hotelFile("Hotel.txt");
        if (!hotelFile.is_open())
        {
            cout << "Error: Unable to open Hotel.txt file." << endl;
            return totalCost; // Return the current total if the file cannot be opened
        }

        string line;
        while (getline(hotelFile, line))
        {
            stringstream ss(line);
            string cityName;
            int hotelCost;

            ss >> cityName >> hotelCost; // Read city name and cost

            if (cityName == layoverCity) // Compare with the layover city
            {
                totalCost += hotelCost; // Add hotel cost to the total
                break; // Stop searching after finding a match
            }
        }

        hotelFile.close();
        return totalCost;
    }
};

//////////////// QUEUE CLASS /////////////////////////
class Queue 
{
public:
    struct LayoverNode
    {
        string intermediateCity;
        flight* firstFlight;
        flight* secondFlight;
        int layoverMinutes;
        LayoverNode* next;

        LayoverNode(string city, flight* f1, flight* f2, int layover) : intermediateCity(city), firstFlight(f1), secondFlight(f2), layoverMinutes(layover), next(nullptr) {}
    };

private:
    LayoverNode* front;
    LayoverNode* rear;
    LayoverNode* current;
    int size;

public:
    Queue() : front(nullptr), rear(nullptr), current(nullptr), size(0) {}

    // Enqueue a layover
    void enqueue(string city, flight* f1, flight* f2, int layoverMinutes)
    {
        LayoverNode* newNode = new LayoverNode(city, f1, f2, layoverMinutes);
        if (rear == nullptr) 
        {
            front = rear = newNode;
        }
        else
        {
            rear->next = newNode;
            rear = newNode;
        }
        size++;
    }

    // Dequeue a layover
    void dequeue() 
    {
        if (isEmpty())
        {
            cout << "Queue is empty!" << endl;
            return;
        }
        LayoverNode* temp = front;
        front = front->next;
        if (front == nullptr) 
        {
            rear = nullptr;
        }
        delete temp;
        size--;
    }

    // Check if the queue is empty
    bool isEmpty() const 
    {
        return front == nullptr;
    }

    // Reset the traversal to the front of the queue
    void resetTraversal()
    {
        current = front;
    }

    // Display all layovers
    void display() const 
    {
        if (isEmpty())
        {
            cout << "\nNo layovers available in the queue!" << endl;
            return;
        }
        cout << "\n\n \033[1;4;34m***** Layover Details (stored in Queue)*****\033[0m" << endl;
        LayoverNode* current = front;
        while (current != nullptr)
        {
            cout << "Intermediate City: " << current->intermediateCity << endl;
            cout << "\033[1;32m First Flight: \033[0m" << endl;
            current->firstFlight->print();
            cout << "\033[1;32m Second Flight: \033[0m" << endl;
            current->secondFlight->print();
            cout << "Layover Duration: " << current->layoverMinutes / 60 << " hours and "
                << current->layoverMinutes % 60 << " minutes" << endl;
            cout << "---------------------------------" << endl;
            current = current->next;
        }
    }

    // Get the size of the queue
    int getSize() const
    {
        return size;
    }

    //gettor
    LayoverNode* getFront()
    {
        return front;
    }

    // Check if there is another element to traverse
    bool hasNext() const
    {
        return current != nullptr;
    }

    // Move to the next node in the queue
    LayoverNode* next()
    {
        if (current != nullptr)
        {
            LayoverNode* temp = current;
            current = current->next;
            return temp;
        }
        return nullptr;
    }
};

/////// ADJACENCY LIST CLASS /////////////
class AdjacentList
{
public:
    flightNode* head;  // Head of the linked list of cities
    flightNode* tail;

    AdjacentList()
    {
        head = NULL;
        tail = NULL;
    }

    // Find the city node
    flightNode* findCity(string city)
    {
        flightNode* temp = head;
        while (temp != NULL)
        {
            if (temp->city == city)
            {
                return temp;
            }
            temp = temp->next;
        }
        return NULL;
    }

    // Insert city only if it doesn't exist
    void insertCityL(string c)
    {

        if (!findCity(c))  // Check if the city already exists
        {
            flightNode* temp = new flightNode(c);
            if (head == NULL && tail == NULL)
            {
                head = temp;
            }
            else
            {
                tail->next = temp;
                tail = temp;
            }
        }
    }

    void insertEdgeL(string source, string depTime, string cost, string airline, string depdate, string dest, string ArrTime)
    {
        // Find the source city node
        flightNode* srcNode = findCity(source);
        if (srcNode == NULL)
        {
            cout << "Error: Source city not found." << endl;
            return;
        }

        // Add the new edge at the end of the adjacency list
        flight* newFlight = new flight(depTime, cost, airline, depdate, dest, ArrTime, source, dest);
        flightNode* newFlightNode = new flightNode(dest);
        newFlightNode->flightInfo = newFlight;

        if (srcNode->next == NULL)
        {
            srcNode->next = newFlightNode; // If no outgoing edges, add the first edge
        }
        else
        {
            flightNode* edgeTemp = srcNode;
            while (edgeTemp->next != NULL)
            {
                edgeTemp = edgeTemp->next;
            }
            edgeTemp->next = newFlightNode; // Add to the end of the list
        }
    }

    // Print the adjacency list (directed graph)
    void printList()
    {
        flightNode* temp = head;

        while (temp != NULL)
        {
            // Check if flight info exists, then print it
            if (temp->flightInfo != NULL)
            {
                temp->flightInfo->print(); // Corrected method call to `print()`
            }
            temp = temp->next;
        }
    }
};

///////// DIRECTED GRAPH CLASS /////////
class graph
{
private:
    AdjacentList* adjList;  // Array of linked list
    string* cities;  // Array of cities easy for add cities and check on which node which city is avaialable
    int cityCount; // act as an index for cities jaisay jaisay cities add kartay jain gain index increase ho ga also jitni cities add ho chuki hain
    int maxCities;   // wo size jis pai hum nai graph banana hai
    const int INF = 99999999;

public:
    graph(int size) // Initialize graph with a fixed size
    {
        maxCities = size;
        adjList = new AdjacentList[size]; // dynamic
        cities = new string[size]; // dynamic
        cityCount = 0;
    }

    // Insert city into the graph
    void insertCity(string city)
    {

        // Check if the city already exists
        for (int i = 0; i < cityCount; i++)
        {
            if (cities[i] == city)
                return; // City already exists, so we skip adding it again
        }

        if (cityCount < maxCities)
        {
            cities[cityCount] = city;
            adjList[cityCount].insertCityL(city);

            cityCount++;
        }
    }

    void insertEdge(string city1, string city2, string depTime, string cost, string airline, string depdate, string ARRtime)
    {
        //cout << "in insert edge :" << city2 << endl;
        if (city1 != city2)  // Avoid self-loops
        {
            for (int i = 0; i < cityCount; i++)
            {
                if (cities[i] == city1)
                {
                    adjList[i].insertEdgeL(city1, depTime, cost, airline, depdate, city2, ARRtime);
                    break;
                }
            }
        }

    }

    // Function to convert arrival  time of first flight and departure time of 2nd flight for connecting flights
    int timeToMinutes(const string& time)
    {
        int hours = stoi(time.substr(0, 2));
        int minutes = stoi(time.substr(3, 2));

        return hours * 60 + minutes;
    }

    // searching function
    void searchFlight(string source, string destination, string date)
    {
        bool directFlight = false;
        bool connectingFlight = false;
        int totalCount = 0;
        int totalCountDirect = 0;

        // Search for direct flights
        cout << "\n\n\t\t\t\t\t\t\t\033[36m ALL Direct Flights From " << source << " To " << destination << " \033[0m" << endl << endl;

        for (int i = 0; i < cityCount; i++)
        {
            if (cities[i] == source)
            {
                flightNode* temp = adjList[i].head;
                while (temp != NULL)
                {
                    if (temp->flightInfo != NULL && temp->flightInfo->destination == destination && temp->flightInfo->depdate == date) 
                    {
                        directFlight = true;
                        totalCountDirect++;
                        temp->flightInfo->print();
                    }
                    temp = temp->next;
                }
            }
        }

        cout << "\n\t\033[35m There Are Total: " << totalCountDirect << " Direct Flights from " << source << " To " << destination << " \033[0m" << endl;
        cout << "\n\n\t\t\t\033[1;35m****************************************************************************************************************";
        cout << "\n\n\t\t\t\t\t\t\t\033[36m ALL Connecting Flights From " << source << " To " << destination << " \033[0m" << endl << endl;

        // Search for connecting flights
        for (int i = 0; i < cityCount; i++)
        {
            if (cities[i] == source)
            {
                flightNode* sourceTemp = adjList[i].head;

                // Traverse all direct flights from the source city
                while (sourceTemp != NULL) 
                {
                    if (sourceTemp->flightInfo != NULL && sourceTemp->flightInfo->depdate == date)
                    {
                        // If source flight date matches input date, get the middle city
                        string middleCity = sourceTemp->flightInfo->destination;

                        for (int j = 0; j < cityCount; j++)
                        {
                            if (cities[j] == middleCity) 
                            {
                                flightNode* destTemp = adjList[j].head;

                                while (destTemp != NULL)
                                {
                                    if (destTemp->flightInfo != NULL && destTemp->flightInfo->destination == destination && destTemp->flightInfo->depdate == date)
                                    {
                                        // Check if arrival time of the first flight is before the departure of the second flight
                                        int arrivalTimeFirstFlight = timeToMinutes(sourceTemp->flightInfo->arrivalTime);
                                        int departureTimeSecondFlight = timeToMinutes(destTemp->flightInfo->departureTime);


                                        if (sourceTemp->flightInfo->arrivalTime == "1:00" || sourceTemp->flightInfo->arrivalTime == "2:00" || sourceTemp->flightInfo->arrivalTime == "3:00" || sourceTemp->flightInfo->arrivalTime == "4:00" ||
                                            sourceTemp->flightInfo->arrivalTime == "5:00" || sourceTemp->flightInfo->arrivalTime == "6:00" || sourceTemp->flightInfo->arrivalTime == "7:00" || sourceTemp->flightInfo->arrivalTime == "8:00" ||
                                            sourceTemp->flightInfo->arrivalTime == "9:00" || sourceTemp->flightInfo->arrivalTime == "10:00" || sourceTemp->flightInfo->arrivalTime == "11:00" || sourceTemp->flightInfo->arrivalTime == "12:00" && sourceTemp->flightInfo->depdate == destTemp->flightInfo->depdate)
                                        {
                                            break;
                                        }


                                        // If the first flight lands on the next day (i.e., past midnight), ignore this connecting flight
                                        if (arrivalTimeFirstFlight >= 1440)
                                        { 
                                            // 1440 minutes = 24 hours
                                            continue; // Skip the flight if it lands after midnight
                                        }

                                        totalCount++;

                                        cout << "\n\n\t\t\t\t\t\t\t\t\033[1;35m ******* CONNECTING FLIGHT " << "******* ::: " << totalCount << "\033[0m" << endl;
                                        // Print the first flight (source to intermediate)
                                        cout << "\nConnecting Flight 1:" << endl;
                                        sourceTemp->flightInfo->print();

                                        // Print the second flight (intermediate to destination)
                                        cout << "Connecting Flight 2:" << endl;
                                        destTemp->flightInfo->print();

                                        connectingFlight = true;
                                    }
                                    destTemp = destTemp->next;
                                }
                            }
                        }
                    }
                    sourceTemp = sourceTemp->next;
                }
            }
        }

        cout << "\n\n\t\033[35m There Are Total: " << totalCount << " Connecting Flights from " << source << " To " << destination << "\033[0m " << endl;

        if (!directFlight)
        {
            cout << "\n\n\n\t\033[31m There is no Direct Flight from: " << source << " To " << destination << endl;
        }

        if (!connectingFlight) 
        {
            cout << "\n\n\n\t\033[31m There is no Connecting Flight from: " << source << " To " << destination << endl;
        }
    }

    // Fuction to calculate layover time 
    int calculateLayoverTime(string arrival, string departure)
    {
        // Extract hours and minutes from arrival time
        int arrivalHours = stoi(arrival.substr(0, 2));
        int arrivalMinutes = stoi(arrival.substr(3, 2));

        // Extract hours and minutes from departure time
        int departureHours = stoi(departure.substr(0, 2));
        int departureMinutes = stoi(departure.substr(3, 2));

        // Convert to total minutes
        int totalArrivalMinutes = arrivalHours * 60 + arrivalMinutes;
        int totalDepartureMinutes = departureHours * 60 + departureMinutes;

        // Calculate the difference
        int layoverMinutes = totalDepartureMinutes - totalArrivalMinutes;

        // Handle the case where layover spans to the next day
        if (layoverMinutes < 0)
        {
            layoverMinutes += 24 * 60; // Add 24 hours in minutes
        }

        return layoverMinutes;

    }

    //booking function
    int booking(graph& flightGraph, string source, string destination, string date)
    {
        bool directFlight = false;
        bool connectingFlight = false;
        int totalCount = 0;
        int totalCountDirect = 0;
        string layover1;
        string layover2;
        char choice;
        int number = 0;
        if (source == destination)
        {
            cout << "NO FLIGHT FROM SAME source : " << source << " TO same destination : " << destination << endl;
        }

        else
        {
            // Search for direct flights
            cout << "\n\n\t\t\t\t\t\t\t\033[36m ALL Direct Flights From " << source << " To " << destination << " \033[0m" << endl << endl;
            int flightNumber = 1;  // Start numbering flights
            for (int i = 0; i < cityCount; i++)
            {
                if (cities[i] == source)
                {
                    flightNode* temp = adjList[i].head;
                    while (temp != NULL)
                    {
                        if (temp->flightInfo != NULL && temp->flightInfo->destination == destination && temp->flightInfo->depdate == date)
                        {
                            directFlight = true;
                            totalCountDirect++;
                            cout << "\n\t\033[1; 32m Direct Flight " << flightNumber << ": \033[0m" << endl;
                            temp->flightInfo->print();
                            flightNumber++;
                        }
                        temp = temp->next;
                    }
                }
            }
            cout << "\n\t\033[35m There Are Total: " << totalCountDirect << " Direct Flights from " << source << " To " << destination << " \033[0m" << endl;
            cout << "\n\n\t\t\t\033[1;35m****************************************************************************************************************";


            if (!directFlight)
            {
                cout << "\n\n\n\t\033[31m There is no Direct Flight from : " << source << " To " << destination << " " << endl << endl;
                cout << "\n\n\t\t\t\t\t\t\t\033[36m ALL Connecting Flights From " << source << " To " << destination << " \033[0m" << endl << endl;
                flightNumber = totalCountDirect + 1; // Start numbering connecting flights after direct ones
                // Search for connecting flights
                for (int i = 0; i < cityCount; i++)
                {
                    if (cities[i] == source)
                    {
                        flightNode* sourceTemp = adjList[i].head; // head assigned here

                        // Traverse all direct flights from the source city
                        while (sourceTemp != NULL)
                        {
                            if (sourceTemp->flightInfo != NULL && sourceTemp->flightInfo->depdate == date) // If the date matches the source flight
                            {
                                string middleCity = sourceTemp->flightInfo->destination; // Intermediate city between source and destination

                                for (int j = 0; j < cityCount; j++)
                                {
                                    if (cities[j] == middleCity) // If the intermediate city exists in the list
                                    {
                                        flightNode* destTemp = adjList[j].head;

                                        while (destTemp != NULL)
                                        {
                                            if (destTemp->flightInfo != NULL && destTemp->flightInfo->destination == destination && destTemp->flightInfo->depdate == date)
                                            {
                                                // Check if the layover time is sufficient for a connecting flight
                                                int arrivalTimeFirstFlight = timeToMinutes(sourceTemp->flightInfo->arrivalTime);
                                                int departureTimeSecondFlight = timeToMinutes(destTemp->flightInfo->departureTime);

                                                if (sourceTemp->flightInfo->arrivalTime == "1:00" || sourceTemp->flightInfo->arrivalTime == "2:00" || sourceTemp->flightInfo->arrivalTime == "3:00" || sourceTemp->flightInfo->arrivalTime == "4:00" ||
                                                    sourceTemp->flightInfo->arrivalTime == "5:00" || sourceTemp->flightInfo->arrivalTime == "6:00" || sourceTemp->flightInfo->arrivalTime == "7:00" || sourceTemp->flightInfo->arrivalTime == "8:00" ||
                                                    sourceTemp->flightInfo->arrivalTime == "9:00" || sourceTemp->flightInfo->arrivalTime == "10:00" || sourceTemp->flightInfo->arrivalTime == "11:00" || sourceTemp->flightInfo->arrivalTime == "12:00" && sourceTemp->flightInfo->depdate == destTemp->flightInfo->depdate)
                                                {
                                                    break;
                                                }
                                                if (arrivalTimeFirstFlight < departureTimeSecondFlight)
                                                {
                                                    totalCount++;

                                                    cout << "\n\n\t\t\t\t\t\t\t\t\033[1;35m ******* CONNECTING FLIGHT " << "******* ::: " << totalCount << "\033[0m" << endl;

                                                    // Print the first flight (source to intermediate)
                                                    cout << "\n\n\t\033[1;32m Flight " << " Connects " << source << " TO " << middleCity << ": \033[0m" << endl;
                                                    sourceTemp->flightInfo->print();
                                                    layover1 = sourceTemp->flightInfo->arrivalTime;

                                                    // Print the second flight (intermediate to destination)
                                                    cout << "\n\n\t\033[1;32m Flight " << " Connects " << middleCity << " To " << destination << " : \033[0m" << endl;
                                                    destTemp->flightInfo->print(); // Intermediate to destination
                                                    layover2 = destTemp->flightInfo->departureTime;

                                                    int layoverMinutes = calculateLayoverTime(layover1, layover2);
                                                    int layoverHours = layoverMinutes / 60;
                                                    int remainingMinutes = layoverMinutes % 60;

                                                    cout << "\n\n\033[33m LAYOVER TIME: " << setw(2) << setfill('0') << layoverHours << " hours and " << setw(2) << setfill('0') << remainingMinutes << " minutes  - Sufficient For Layover\033[0m" << endl;
                                                    connectingFlight = true;
                                                    flightNumber += 2; // Increase by 2 for connecting flights
                                                }
                                            }
                                            destTemp = destTemp->next;
                                        }
                                    }
                                }
                            }
                            sourceTemp = sourceTemp->next;
                        }
                    }
                }

                cout << "\n\n\033[35m There Are Total: " << totalCount << " Connecting Flights from " << source << " To " << destination << "\033[0m " << endl;
            }
            if (!connectingFlight)
            {
                cout << "\n\n\n\t\033[31m There is no CONNECTING Flight from : " << source << " To " << destination << " " << endl << endl;
            }

            if (connectingFlight || directFlight)
            {
                //visualize the graph 
                flightGraph.visualizeGraphForSearch(flightGraph, 1000, 800, source, destination, date);

                cout << "\n\n\t\033[36m DO YOU WANT TO BOOK A FLIGHT (Y/N) : \033[0m";
                cin >> choice;
                if (choice == 'y' || choice == 'Y')
                {
                    cout << "\n\nEnter the number of the flight you want to book: ";
                    cin >> number;

                    if (number < 1 || number >= flightNumber)
                    {
                        cout << "\n\t\t\t\033[31m Invalid flight number! Please try again\033[0m" << endl;
                    }
                    else
                    {
                        // Display the flight booking information (ticket)
                        cout << "\n\n\t\t\t\t\t\t\t\t\t\033[34m ***** FLIGHT CONFIRMATION ***** \033[0m" << endl;
                        cout << "\n\t\t\t\t\t\t\t\t\033[32m ----------------------------------------------" << endl;
                        cout << "\t\t\t\t\t\t\t\tBooking Confirmation for Flight Number: \033[0m" << number << "\033[32m IS DONE !! " << endl;
                        cout << "\t\t\t\t\t\t\t\t----------------------------------------------\033[0m" << endl;




                        cout << "\n\t\t\t\t\t\t\t\t\t\t\033[35m Enjoy your flight! \033[0m" << endl;
                    }
                }
            }
            else if (!connectingFlight && !directFlight)
            {
                cout << "\n\t\033[31m There are no flights available from " << source << " to " << destination << " on " << date << " \033[0m" << endl << endl;
            }

        }
        return number;
    }

    //function for custom user preference
    void CustomUserPreference(string al, string layoverCity)
    {
        bool check = false;
        for (int i = 0; i < cityCount; i++)
        {

            flightNode* srcCity = adjList[i].head;
            if (cities[i] == srcCity->city)
            {
                // Step 1: Check for flights matching the specified airline
                while (srcCity != NULL)
                {
                    if (srcCity->flightInfo != NULL && srcCity->flightInfo->airline == al)
                    {
                        string destcity = srcCity->flightInfo->destination;

                        // Step 2: Look for connecting flights with layover city
                        for (int j = 0; j < cityCount; j++)
                        {
                            if (cities[j] == destcity)
                            {

                                flightNode* destNode = adjList[j].head;

                                while (destNode != NULL)
                                {
                                    // Step 3: Check if the layover matches and dates align
                                    if (destNode->flightInfo != NULL && destNode->flightInfo->city1 == layoverCity && srcCity->flightInfo->depdate == destNode->flightInfo->depdate && destNode->flightInfo->airline == al /*&& srcCity->flightInfo->city1 != destNode->flightInfo->destination*/)
                                    {
                                        // Check if the layover time is sufficient for a connecting flight
                                        int arrivalTimeFirstFlight = timeToMinutes(srcCity->flightInfo->arrivalTime);
                                        int departureTimeSecondFlight = timeToMinutes(destNode->flightInfo->departureTime);
                                        if (srcCity->flightInfo->arrivalTime == "1:00" || srcCity->flightInfo->arrivalTime == "2:00" || srcCity->flightInfo->arrivalTime == "3:00" || srcCity->flightInfo->arrivalTime == "4:00" ||
                                            srcCity->flightInfo->arrivalTime == "5:00" || srcCity->flightInfo->arrivalTime == "6:00" || srcCity->flightInfo->arrivalTime == "7:00" || srcCity->flightInfo->arrivalTime == "8:00" ||
                                            srcCity->flightInfo->arrivalTime == "9:00" || srcCity->flightInfo->arrivalTime == "10:00" || srcCity->flightInfo->arrivalTime == "11:00" || srcCity->flightInfo->arrivalTime == "12:00" && srcCity->flightInfo->depdate == destNode->flightInfo->depdate)
                                        {
                                            break;
                                        }
                                        if (arrivalTimeFirstFlight < departureTimeSecondFlight)

                                        {
                                            cout << "\n\n\t\t\033[1;35m Flight From Airline " << al << " And Layover city (" << layoverCity << ") is : \033[0m" << endl;

                                            cout << "\n\033[1; 4 ; 32m FLIGHT 1 :: \033[0m";
                                            srcCity->flightInfo->print();

                                            cout << "\n\033[1; 4 ;33m FLIGHT 2 :: \033[0m";
                                            destNode->flightInfo->print();
                                            check = true;
                                        }
                                    }
                                    destNode = destNode->next;
                                }
                            }
                        }
                    }
                    srcCity = srcCity->next;
                }
            }
        }

        if (check == false)
        {
            cout << "\n\n\t\t\033[1;31m NO AIRLINE OR LAYOVER CITY FOUND \033[0m\n\n";
        }
    }

    //visualization graph of custom user preferences showing the paths of prefered airline via a layover city
    void visualizeCustomFlights(graph& flightGraph, const string& airline, const string& layoverCity, int windowWidth, int windowHeight)
    {
        const double PI = 3.14159265358979323846;
        RenderWindow window(VideoMode(windowWidth, windowHeight), "Custom Flight Path Visualizer");
        window.setFramerateLimit(60);

        // Load background image
        Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("background.jpg"))
        {
            cout << "\nError loading background image" << endl;
        }
        backgroundTexture.setSmooth(true);
        Sprite backgroundSprite;
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(
            static_cast<float>(windowWidth) / backgroundTexture.getSize().x,
            static_cast<float>(windowHeight) / backgroundTexture.getSize().y);

        const int maxCities = 11; // Assuming max 11 cities as per the graph initialization
        CircleShape cityNodes[maxCities];
        Vector2f cityPositions[maxCities];
        string cityNames[maxCities];

        float centerX = static_cast<float>(windowWidth) / 2, centerY = static_cast<float>(windowHeight) / 2;
        float radius = 300.0f;

        // Assign positions to cities in a circular layout
        for (int i = 0; i < flightGraph.cityCount; i++)
        {
            float angle = static_cast<float>((2 * PI / flightGraph.cityCount) * i);
            cityPositions[i] = Vector2f(centerX + radius * cos(angle), centerY + radius * sin(angle));
            cityNames[i] = flightGraph.cities[i];

            cityNodes[i].setRadius(20);
            cityNodes[i].setFillColor(Color::Green);
            cityNodes[i].setOrigin(15, 15);
            cityNodes[i].setPosition(cityPositions[i]);
        }

        Font font;
        if (!font.loadFromFile("Arial.ttf"))
        {
            cout << "Error loading default font." << endl;
        }

        Text cityText, flightText;
        cityText.setFont(font);
        cityText.setCharacterSize(18);
        cityText.setFillColor(Color::White);
        flightText.setFont(font);
        flightText.setCharacterSize(12);
        flightText.setFillColor(Color::Yellow);

        Texture airplaneTexture;
        if (!airplaneTexture.loadFromFile("airplanered.png"))
        {
            cout << "Error loading airplane icon." << endl;
        }

        Sprite airplaneSprite(airplaneTexture);
        airplaneSprite.setScale(0.03f, 0.03f);

        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
            }

            window.clear(Color::Black);
            window.draw(backgroundSprite);

            // Iterate through the adjacency list to draw relevant paths
            for (int i = 0; i < flightGraph.cityCount; i++)
            {
                flightNode* srcNode = flightGraph.adjList[i].head;

                while (srcNode != NULL)
                {
                    if (srcNode->flightInfo != NULL && srcNode->flightInfo->airline == airline)
                    {
                        string layoverDest = srcNode->flightInfo->destination;

                        // Find flights originating from the layover city
                        for (int j = 0; j < flightGraph.cityCount; j++)
                        {
                            if (flightGraph.cities[j] == layoverDest)
                            {
                                flightNode* layoverNode = flightGraph.adjList[j].head;

                                while (layoverNode != NULL)
                                {
                                    if (layoverNode->flightInfo != NULL && layoverNode->flightInfo->city1 == layoverCity && srcNode->flightInfo->depdate == layoverNode->flightInfo->depdate && layoverNode->flightInfo->airline == airline)
                                    {
                                        // Check if the layover time is sufficient for a connecting flight
                                        int arrivalTimeFirstFlight = timeToMinutes(srcNode->flightInfo->arrivalTime);
                                        int departureTimeSecondFlight = timeToMinutes(layoverNode->flightInfo->departureTime);

                                        if (srcNode->flightInfo->arrivalTime == "1:00" || srcNode->flightInfo->arrivalTime == "2:00" || srcNode->flightInfo->arrivalTime == "3:00" || srcNode->flightInfo->arrivalTime == "4:00" ||
                                            srcNode->flightInfo->arrivalTime == "5:00" || srcNode->flightInfo->arrivalTime == "6:00" || srcNode->flightInfo->arrivalTime == "7:00" || srcNode->flightInfo->arrivalTime == "8:00" ||
                                            srcNode->flightInfo->arrivalTime == "9:00" || srcNode->flightInfo->arrivalTime == "10:00" || srcNode->flightInfo->arrivalTime == "11:00" || srcNode->flightInfo->arrivalTime == "12:00" && srcNode->flightInfo->depdate == layoverNode->flightInfo->depdate)
                                        {
                                            break;
                                        }

                                        if (arrivalTimeFirstFlight < departureTimeSecondFlight)
                                        {
                                            // Draw the first flight path
                                            Vector2f srcPos = cityPositions[i];
                                            Vector2f layoverPos = cityPositions[j];

                                            sf::Color neonPink(255, 140, 0);
                                            Vertex firstLine[] =
                                            {
                                                Vertex(srcPos, Color::Yellow),
                                                Vertex(layoverPos, neonPink)
                                            };
                                            window.draw(firstLine, 2, Lines);

                                            // Place airplane for the first flight
                                            float angle1 = atan2(layoverPos.y - srcPos.y, layoverPos.x - srcPos.x) * 180 / PI;
                                            Vector2f airplanePos1 = srcPos + 0.5f * (layoverPos - srcPos);
                                            airplaneSprite.setPosition(airplanePos1.x - airplaneSprite.getGlobalBounds().width / 2,
                                                airplanePos1.y - airplaneSprite.getGlobalBounds().height / 2);
                                            airplaneSprite.setRotation(angle1);
                                            window.draw(airplaneSprite);

                                            // Draw the second flight path
                                            string finalDest = layoverNode->flightInfo->destination;
                                            Vector2f finalDestPos;

                                            for (int k = 0; k < flightGraph.cityCount; k++)
                                            {
                                                if (flightGraph.cities[k] == finalDest)
                                                    finalDestPos = cityPositions[k];
                                            }

                                            Vertex secondLine[] =
                                            {
                                                Vertex(layoverPos, neonPink),
                                                Vertex(finalDestPos, neonPink)
                                            };
                                            window.draw(secondLine, 2, Lines);

                                            // Place airplane for the second flight
                                            float angle2 = atan2(finalDestPos.y - layoverPos.y, finalDestPos.x - layoverPos.x) * 180 / PI;
                                            Vector2f airplanePos2 = layoverPos + 0.5f * (finalDestPos - layoverPos);
                                            airplaneSprite.setPosition(airplanePos2.x - airplaneSprite.getGlobalBounds().width / 2,
                                                airplanePos2.y - airplaneSprite.getGlobalBounds().height / 2);
                                            airplaneSprite.setRotation(angle2);
                                            window.draw(airplaneSprite);
                                        }
                                    }

                                    layoverNode = layoverNode->next;
                                }
                            }
                        }
                    }

                    srcNode = srcNode->next;
                }
            }

            // Draw city nodes
            for (int i = 0; i < flightGraph.cityCount; i++)
            {
                window.draw(cityNodes[i]);
                cityText.setString(flightGraph.cities[i]);
                cityText.setPosition(cityPositions[i].x - 20, cityPositions[i].y - 30);
                window.draw(cityText);
            }

            window.display();
        }
    }

     // Print the graph (adjacency list)
    void printGraph()
    {
        for (int i = 0; i < cityCount; i++)
        {
            adjList[i].printList();
        }
    }

    //visualize grapg function for viewing the whole graph, the totla number of flights mentioned in the 'Flights.txt'
    void visualizeGraphForViewing(graph& flightGraph, int windowWidth, int windowHeight)
    {
        const double PI = 3.14159265358979323846;
        RenderWindow window(VideoMode(windowWidth, windowHeight), "Flight Path Visualizer - SkyNav AI");
        window.setFramerateLimit(60);

        // Load background image with smoothing disabled
        Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("background.jpg")) 
        {
            cout << "Error loading background image" << endl;
        }

        // Disable texture smoothing for sharper scaling
        backgroundTexture.setSmooth(true);

        // Create sprite for the background
        Sprite backgroundSprite;
        backgroundSprite.setTexture(backgroundTexture);

        // Scale the background to fit the window
        backgroundSprite.setScale(
            static_cast<float>(windowWidth) / backgroundTexture.getSize().x,
            static_cast<float>(windowHeight) / backgroundTexture.getSize().y);

        const int maxCities = 11; // Assuming max 11 cities as per the graph initialization
        CircleShape cityNodes[maxCities];
        Vector2f cityPositions[maxCities];
        string cityNames[maxCities];
        int cityCount = 0;

        // Define circular layout for cities
        float centerX = static_cast<float>(windowWidth) / 2, centerY = static_cast<float>(windowHeight) / 2;
        float radius = 300.0f; // Original radius for the circular layout

        // Assign positions to cities in a circular layout, with adjustments for specific cities
        for (int i = 0; i < flightGraph.cityCount; i++) 
        {
            // Calculate the base angle for each city
            float angle = static_cast<float>((2 * PI / flightGraph.cityCount) * i);
            cityPositions[i] = Vector2f(centerX + radius * cos(angle), centerY + radius * sin(angle));
            cityNames[i] = flightGraph.cities[i];

            // Setup the appearance of the city nodes
            cityNodes[i].setRadius(20);  // Node size remains fixed
            cityNodes[i].setFillColor(Color::Green);
            cityNodes[i].setOrigin(15, 15); // Center the circles
            cityNodes[i].setPosition(cityPositions[i]);
        }

        // Create SFML text with default font (no external font required)
        Font font;
        if (!font.loadFromFile("Arial.ttf"))
        { 
            // Load Arial font from system
            cout << "Error loading default font." << endl;
        }

        Text cityText, flightText;
        cityText.setFont(font);
        cityText.setCharacterSize(18);
        cityText.setFillColor(Color::White);
        flightText.setFont(font);
        flightText.setCharacterSize(7);
        flightText.setFillColor(Color::White);

        // Load airplane texture (make sure you have an airplane image)
        Texture airplaneTexture;
        if (!airplaneTexture.loadFromFile("airplanered.png"))
        { 
            // Load your airplane icon
            cout << "Error loading airplane icon." << endl;
        }

        Sprite airplaneSprite(airplaneTexture);
        // Resize the airplane sprite to make it smaller (scale it down to 0.04 for example)
        airplaneSprite.setScale(0.03f, 0.03f);  // Further reduce the size

        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
            }

            window.clear(Color::Black);

            // Draw background image
            window.draw(backgroundSprite);

            // Draw edges (flights) between cities
            for (int i = 0; i < flightGraph.cityCount; i++)
            {
                flightNode* node = flightGraph.adjList[i].head;

                while (node != NULL) 
                {
                    if (node->flightInfo != NULL) 
                    {
                        // Find source and destination positions
                        Vector2f srcPos, destPos;
                        for (int j = 0; j < flightGraph.cityCount; j++)
                        {
                            if (flightGraph.cities[j] == node->flightInfo->city1)
                                srcPos = cityPositions[j];
                            if (flightGraph.cities[j] == node->flightInfo->city2)
                                destPos = cityPositions[j];
                        };

                        // Draw line for the edge (flight) between cities
                        sf::Color neonPink(255, 140, 0);
                        Vertex line[] =
                        {
                            Vertex(srcPos, neonPink),
                            Vertex(destPos, neonPink)
                        };
                        window.draw(line, 2, Lines);  // Fixed to draw only two points (src and dest)

                        // Create rotation angle for airplane sprite
                        float deltaX = destPos.x - srcPos.x;
                        float deltaY = destPos.y - srcPos.y;
                        float angle = static_cast<float>(atan2(deltaY, deltaX) * 180 / PI); // Get angle in degrees

                        // Place airplane sprites closer to the source city
                        flightNode* tempNode = node;
                        int flightIndex = 0;  // Keep track of how many airplanes on this path
                        while (tempNode != NULL)
                        {
                            // Horizontal offset: alternate left/right for each airplane
                            float offset = (flightIndex % 2 == 0) ? -20.0f : 20.0f;  // Alternate left (-) or right (+)

                            // Calculate the airplane position closer to the source city (20% of the distance from src to dest)
                            float ratio = 0.2f; // 20% of the distance
                            Vector2f airplanePos = srcPos + ratio * (destPos - srcPos);

                            // Adjust the airplane position based on the offset (left or right)
                            airplanePos.x += offset;

                            // Set airplane position
                            airplaneSprite.setPosition(airplanePos.x - airplaneSprite.getGlobalBounds().width / 2, airplanePos.y - airplaneSprite.getGlobalBounds().height / 2); // Adjust for multiple flights
                            airplaneSprite.setRotation(angle); // Rotate airplane to face the direction
                            window.draw(airplaneSprite);
                            flightIndex++;  // Increment for the next airplane
                            tempNode = tempNode->next;
                        }

                        // Display flight details near the flight path
                        flightText.setString(flightGraph.cities[i] + " -> " + node->flightInfo->city2); // Show the flight path details
                        flightText.setPosition((srcPos.x + destPos.x) / 2 - 100, (srcPos.y + destPos.y) / 2 - 40); // Adjust position to avoid overlap
                        window.draw(flightText);
                    }
                    node = node->next;
                }
            }

            // Draw city nodes
            for (int i = 0; i < flightGraph.cityCount; i++) 
            {
                window.draw(cityNodes[i]);

                // Set text position and display city name
                cityText.setString(flightGraph.cities[i]);
                cityText.setPosition(cityPositions[i].x - 20, cityPositions[i].y - 30);  // Adjust position to avoid overlap
                window.draw(cityText);
            }
            window.display();
        }
    }

    //searching function 
    void visualizeGraphForSearch(graph& flightGraph, int windowWidth, int windowHeight, string source, string destination, string date)
    {
        const double PI = 3.14159265358979323846;
        RenderWindow window(VideoMode(windowWidth, windowHeight), "Flight Path Visualizer - SkyNav AI");
        window.setFramerateLimit(60);

        // Load background image with smoothing disabled
        Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("background.jpg")) 
        {
            cout << "Error loading background image." << endl;
        }

        // Disable texture smoothing for sharper scaling
        backgroundTexture.setSmooth(true);

        // Create sprite for the background
        Sprite backgroundSprite;
        backgroundSprite.setTexture(backgroundTexture);

        // Scale the background to fit the window
        backgroundSprite.setScale(
            static_cast<float>(windowWidth) / backgroundTexture.getSize().x, static_cast<float>(windowHeight) / backgroundTexture.getSize().y);

        const int maxCities = 11; // Assuming max 11 cities as per the graph initialization
        CircleShape cityNodes[maxCities];
        Vector2f cityPositions[maxCities];
        string cityNames[maxCities];
        int cityCount = 0;

        // Define circular layout for cities
        float centerX = static_cast<float>(windowWidth) / 2, centerY = static_cast<float>(windowHeight) / 2;
        float radius = 300.0f; // Original radius for the circular layout

        // Assign positions to cities in a circular layout, with adjustments for specific cities
        for (int i = 0; i < flightGraph.cityCount; i++) 
        {
            // Calculate the base angle for each city
            float angle = static_cast<float>((2 * PI / flightGraph.cityCount) * i);
            cityPositions[i] = Vector2f(centerX + radius * cos(angle), centerY + radius * sin(angle));
            cityNames[i] = flightGraph.cities[i];

            // Setup the appearance of the city nodes
            cityNodes[i].setRadius(20);  // Node size remains fixed
            cityNodes[i].setFillColor(Color::Green);
            cityNodes[i].setOrigin(15, 15); // Center the circles
            cityNodes[i].setPosition(cityPositions[i]);
        }

        // Create SFML text with default font (no external font required)
        Font font;
        if (!font.loadFromFile("Arial.ttf"))
        { 
            // Load Arial font from system
            cout << "Error loading default font." << endl;
        }

        Text cityText, flightText;
        cityText.setFont(font);
        cityText.setCharacterSize(18);
        cityText.setFillColor(Color::White);
        flightText.setFont(font);
        flightText.setCharacterSize(7);
        flightText.setFillColor(Color::White);

        // Load airplane texture (make sure you have an airplane image)
        Texture airplaneTexture;
        if (!airplaneTexture.loadFromFile("airplanered.png"))
        { 
            // Load your airplane icon
            cout << "Error loading airplane icon." << endl;
        }

        Sprite airplaneSprite(airplaneTexture);
        // Resize the airplane sprite to make it smaller (scale it down to 0.04 for example)
        airplaneSprite.setScale(0.03f, 0.03f);  // Further reduce the size

        while (window.isOpen()) 
        {
            Event event;
            while (window.pollEvent(event)) 
            {
                if (event.type == Event::Closed)
                    window.close();
            }

            window.clear(Color::Black);

            // Draw background image
            window.draw(backgroundSprite);

            // Draw edges (flights) between cities
            for (int i = 0; i < flightGraph.cityCount; i++)
            {
                flightNode* node = flightGraph.adjList[i].head;

                while (node != NULL) 
                {
                    if (node->flightInfo != NULL)
                    {
                        // Match direct flights for the specified source, destination, and date
                        if (node->flightInfo->city1 == source && node->flightInfo->city2 == destination && node->flightInfo->depdate == date) 
                        {
                            // Find source and destination positions
                            Vector2f srcPos, destPos;
                            for (int j = 0; j < flightGraph.cityCount; j++) 
                            {
                                if (flightGraph.cities[j] == node->flightInfo->city1)
                                    srcPos = cityPositions[j];
                                if (flightGraph.cities[j] == node->flightInfo->city2)
                                    destPos = cityPositions[j];
                            }

                            // Draw line for the edge (flight) between cities
                            sf::Color neonPink(255, 20, 147);
                            Vertex line[] =
                            {
                                Vertex(srcPos, neonPink),
                                Vertex(destPos, neonPink)
                            };
                            window.draw(line, 2, Lines);  // Fixed to draw only two points (src and dest)

                            // Create rotation angle for airplane sprite
                            float deltaX = destPos.x - srcPos.x;
                            float deltaY = destPos.y - srcPos.y;
                            float angle = static_cast<float>(atan2(deltaY, deltaX) * 180 / PI); // Get angle in degrees

                            // Place airplane sprites closer to the source city
                            flightNode* tempNode = node;
                            int flightIndex = 0;  // Keep track of how many airplanes on this path
                            while (tempNode != NULL)
                            {
                                // Horizontal offset: alternate left/right for each airplane
                                float offset = (flightIndex % 2 == 0) ? -20.0f : 20.0f;  // Alternate left (-) or right (+)

                                // Calculate the airplane position closer to the source city (20% of the distance from src to dest)
                                float ratio = 0.2f; // 20% of the distance
                                Vector2f airplanePos = srcPos + ratio * (destPos - srcPos);

                                // Adjust the airplane position based on the offset (left or right)
                                airplanePos.x += offset;

                                // Set airplane position
                                airplaneSprite.setPosition(airplanePos.x - airplaneSprite.getGlobalBounds().width / 2, airplanePos.y - airplaneSprite.getGlobalBounds().height / 2); // Adjust for multiple flights 
                                airplaneSprite.setRotation(angle); // Rotate airplane to face the direction
                                window.draw(airplaneSprite);
                                flightIndex++;  // Increment for the next airplane
                                tempNode = tempNode->next;
                            }

                            // Display flight details near the flight path
                            flightText.setString(flightGraph.cities[i] + " -> " + node->flightInfo->city2); // Show the flight path details
                            flightText.setPosition((srcPos.x + destPos.x) / 2 - 100, (srcPos.y + destPos.y) / 2 - 40); // Adjust position to avoid overlap
                            window.draw(flightText);
                        }

                        // Check for connecting flights
                        if (node->flightInfo->city1 == source && node->flightInfo->depdate == date)  
                        {
                            string middleCity = node->flightInfo->city2;  // Intermediate city
                            for (int j = 0; j < flightGraph.cityCount; j++)
                            {
                                if (flightGraph.cities[j] == middleCity) 
                                {
                                    flightNode* intermediateNode = flightGraph.adjList[j].head;
                                    while (intermediateNode != NULL)
                                    {
                                        if (intermediateNode->flightInfo != NULL && intermediateNode->flightInfo->city1 == middleCity && intermediateNode->flightInfo->city2 == destination && intermediateNode->flightInfo->depdate == date)
                                        {
                                            if (node->flightInfo->arrivalTime == "1:00" || node->flightInfo->arrivalTime == "2:00" || node->flightInfo->arrivalTime == "3:00" || node->flightInfo->arrivalTime == "4:00" ||
                                                node->flightInfo->arrivalTime == "5:00" || node->flightInfo->arrivalTime == "6:00" || node->flightInfo->arrivalTime == "7:00" || node->flightInfo->arrivalTime == "8:00" ||
                                                node->flightInfo->arrivalTime == "9:00" || node->flightInfo->arrivalTime == "10:00" || node->flightInfo->arrivalTime == "11:00" || node->flightInfo->arrivalTime == "12:00" && 
                                                node->flightInfo->depdate == intermediateNode->flightInfo->depdate)
                                            {
                                                break;
                                            }

                                            // Find positions for source, intermediate, and destination cities
                                            Vector2f srcPos, interPos, destPos;
                                            for (int k = 0; k < flightGraph.cityCount; k++)
                                            {
                                                if (flightGraph.cities[k] == node->flightInfo->city1)
                                                    srcPos = cityPositions[k];
                                                if (flightGraph.cities[k] == middleCity)
                                                    interPos = cityPositions[k];
                                                if (flightGraph.cities[k] == destination)
                                                    destPos = cityPositions[k];
                                            }

                                            // Draw the first leg of the connecting flight
                                            sf::Color neonOrange(255, 140, 0);
                                            Vertex line1[] = {
                                                Vertex(srcPos, neonOrange),
                                                Vertex(interPos, neonOrange)
                                            };
                                            window.draw(line1, 2, Lines);

                                            // Draw the second leg of the connecting flight
                                            sf::Color neonBlue(0, 0, 255);
                                            Vertex line2[] = {
                                                Vertex(interPos, neonBlue),
                                                Vertex(destPos, neonBlue)
                                            };
                                            window.draw(line2, 2, Lines);

                                            // Add airplane sprites for both legs of the connecting flight
                                            float ratio = 0.2f; // Adjust the ratio to place airplanes closer to the starting point of each leg
                                            // For the first leg (source -> intermediate)
                                            Vector2f airplanePos1 = srcPos + ratio * (interPos - srcPos); 
                                            airplaneSprite.setPosition( 
                                                airplanePos1.x - airplaneSprite.getGlobalBounds().width / 2, 
                                                airplanePos1.y - airplaneSprite.getGlobalBounds().height / 2 
                                            );
                                            float angle1 = static_cast<float>(atan2(interPos.y - srcPos.y, interPos.x - srcPos.x) * 180 / PI); 
                                            airplaneSprite.setRotation(angle1); 
                                            window.draw(airplaneSprite); 

                                            // For the second leg (intermediate -> destination)
                                            Vector2f airplanePos2 = interPos + ratio * (destPos - interPos);
                                            airplaneSprite.setPosition( 
                                                airplanePos2.x - airplaneSprite.getGlobalBounds().width / 2,
                                                airplanePos2.y - airplaneSprite.getGlobalBounds().height / 2
                                            );
                                            float angle2 = static_cast<float>(atan2(destPos.y - interPos.y, destPos.x - interPos.x) * 180 / PI);
                                            airplaneSprite.setRotation(angle2); 
                                            window.draw(airplaneSprite); 
                                            window.draw(airplaneSprite);
                                        }
                                        intermediateNode = intermediateNode->next;
                                    }
                                }
                            }
                        }
                    }
                    node = node->next;
                }
            }

            // Draw city nodes
            for (int i = 0; i < flightGraph.cityCount; i++)
            {
                window.draw(cityNodes[i]);

                // Set text position and display city name
                cityText.setString(flightGraph.cities[i]);
                cityText.setPosition(cityPositions[i].x - 20, cityPositions[i].y - 30);  // Adjust position to avoid overlap
                window.draw(cityText);
            }
            window.display();
        }
    }

    //booking function visualziation
    void visualizeGraphForBooking(graph& flightGraph, int windowWidth, int windowHeight, string source, string destination, string date, int selectedFlightNumber)
    {
        const double PI = 3.14159265358979323846;
        RenderWindow window(VideoMode(windowWidth, windowHeight), "Flight Path Visualizer - SkyNav AI");
        window.setFramerateLimit(60);

        // Load background image
        Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("background.jpg"))
        {
            cout << "Error loading background image." << endl;
        }
        backgroundTexture.setSmooth(true);

        Sprite backgroundSprite;
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(
            static_cast<float>(windowWidth) / backgroundTexture.getSize().x,
            static_cast<float>(windowHeight) / backgroundTexture.getSize().y);

        const int maxCities = 11; // Maximum cities
        CircleShape cityNodes[maxCities];
        Vector2f cityPositions[maxCities];
        string cityNames[maxCities];
        int cityCount = 0;

        // Define circular layout
        float centerX = static_cast<float>(windowWidth) / 2, centerY = static_cast<float>(windowHeight) / 2;
        float radius = 300.0f;

        for (int i = 0; i < flightGraph.cityCount; i++)
        {
            float angle = static_cast<float>((2 * PI / flightGraph.cityCount) * i);
            cityPositions[i] = Vector2f(centerX + radius * cos(angle), centerY + radius * sin(angle));
            cityNames[i] = flightGraph.cities[i];

            cityNodes[i].setRadius(20);
            cityNodes[i].setFillColor(Color::Green);
            cityNodes[i].setOrigin(15, 15);
            cityNodes[i].setPosition(cityPositions[i]);
        }

        Font font;
        if (!font.loadFromFile("Arial.ttf"))
        {
            cout << "Error loading font." << endl;
        }

        Text cityText, flightText;
        cityText.setFont(font);
        cityText.setCharacterSize(18);
        cityText.setFillColor(Color::White);
        flightText.setFont(font);
        flightText.setCharacterSize(14);
        flightText.setFillColor(Color::White);

        Texture airplaneTexture;
        if (!airplaneTexture.loadFromFile("airplanered.png"))
        {
            cout << "Error loading airplane icon." << endl;
        }
        Sprite airplaneSprite(airplaneTexture);
        airplaneSprite.setScale(0.03f, 0.03f);

        // Manual storage for flight information
        string flightCity1[2], flightCity2[2];
        int flightCount = 0;

        // Search and store the selected flight path
        for (int i = 0; i < flightGraph.cityCount && flightCount < 2; i++)
        {
            flightNode* node = flightGraph.adjList[i].head;
            while (node != NULL && flightCount < 2)
            {
                if (node->flightInfo != NULL && node->flightInfo->depdate == date && (selectedFlightNumber == 1 || selectedFlightNumber == 2))
                {
                    if (node->flightInfo->city1 == source || node->flightInfo->city2 == destination)
                    {
                        flightCity1[flightCount] = node->flightInfo->city1;
                        flightCity2[flightCount] = node->flightInfo->city2;
                        flightCount++;
                    }
                }
                node = node->next;
            }
        }

        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
            }

            window.clear(Color::Black);
            window.draw(backgroundSprite);

            // Draw flight paths for selected flight
            for (int i = 0; i < flightCount; i++)
            {
                Vector2f srcPos, destPos;
                for (int j = 0; j < flightGraph.cityCount; j++)
                {
                    if (flightGraph.cities[j] == flightCity1[i])
                        srcPos = cityPositions[j];
                    if (flightGraph.cities[j] == flightCity2[i])
                        destPos = cityPositions[j];
                }

                // Draw line
                sf::Color neonPink(255, 20, 147);
                Vertex line[] =
                {
                    Vertex(srcPos, neonPink),
                    Vertex(destPos, neonPink)
                };
                window.draw(line, 2, Lines);

                // Draw airplane icon
                float deltaX = destPos.x - srcPos.x;
                float deltaY = destPos.y - srcPos.y;
                float angle = static_cast<float>(atan2(deltaY, deltaX) * 180 / PI);

                float ratio = 0.2f; // 20% of the way
                Vector2f airplanePos = srcPos + ratio * (destPos - srcPos);
                airplaneSprite.setPosition(
                    airplanePos.x - airplaneSprite.getGlobalBounds().width / 2,
                    airplanePos.y - airplaneSprite.getGlobalBounds().height / 2);
                airplaneSprite.setRotation(angle);
                window.draw(airplaneSprite);
            }

            // Draw city nodes and names
            for (int i = 0; i < flightGraph.cityCount; i++)
            {
                window.draw(cityNodes[i]);
                cityText.setString(cityNames[i]);
                cityText.setPosition(cityPositions[i].x - 20, cityPositions[i].y - 30);
                window.draw(cityText);
            }

            window.display();
        }
    }

    // visualize graph for expensive, cheap and moderate flights
    void visualizeGraph(graph& flightGraph, int windowWidth, int windowHeight)
    {
        const double PI = 3.14159265358979323846;
        RenderWindow window(VideoMode(windowWidth, windowHeight), "Flight Path Visualizer - SkyNav AI");
        window.setFramerateLimit(60);

        // Load background image with smoothing disabled
        Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("background.jpg")) 
        {
            cout << "Error loading background image." << endl;
        }

        // Disable texture smoothing for sharper scaling
        backgroundTexture.setSmooth(true);

        // Create sprite for the background
        Sprite backgroundSprite;
        backgroundSprite.setTexture(backgroundTexture);

        // Scale the background to fit the window
        backgroundSprite.setScale(static_cast<float>(windowWidth) / backgroundTexture.getSize().x, static_cast<float>(windowHeight) / backgroundTexture.getSize().y);

        const int maxCities = 11;
        CircleShape cityNodes[maxCities];
        Vector2f cityPositions[maxCities];
        string cityNames[maxCities];
        int cityCount = 0;

        float centerX = static_cast<float>(windowWidth) / 2, centerY = static_cast<float>(windowHeight) / 2;
        float radius = 300.0f;

        for (int i = 0; i < flightGraph.cityCount; i++)
        {
            float angle = static_cast<float>((2 * PI / flightGraph.cityCount) * i);
            cityPositions[i] = Vector2f(centerX + radius * cos(angle), centerY + radius * sin(angle));
            cityNames[i] = flightGraph.cities[i];

            cityNodes[i].setRadius(20);
            cityNodes[i].setFillColor(Color::Green);
            cityNodes[i].setOrigin(20, 20);
            cityNodes[i].setPosition(cityPositions[i]);
        }

        // Create SFML text with default font (no external font required)
        Font font;
        if (!font.loadFromFile("Arial.ttf"))
        {
            cout << "Error loading default font." << endl;
        }

        Text cityText, flightText;
        cityText.setFont(font);
        cityText.setCharacterSize(18);
        cityText.setFillColor(Color::White);
        flightText.setFont(font);
        flightText.setCharacterSize(9);
        flightText.setFillColor(Color::White);

        // Load airplane texture (make sure you have an airplane image)
        Texture airplaneTexture;
        if (!airplaneTexture.loadFromFile("airplanered.png"))
        {
            cout << "Error loading airplane icon." << endl;
        }

        Sprite airplaneSprite(airplaneTexture);
        airplaneSprite.setScale(0.03f, 0.03f);

        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
            }

            window.clear(Color::Black);

            // Draw background image
            window.draw(backgroundSprite);

            // Draw edges (flights) between cities
            for (int i = 0; i < flightGraph.cityCount; i++)
            {
                flightNode* node = flightGraph.adjList[i].head;

                while (node != nullptr) 
                {
                    if (node->flightInfo != nullptr) 
                    {
                        int flightCost = stringToInt(node->flightInfo->cost);  // Convert the cost to int

                        Vector2f srcPos, destPos;
                        for (int j = 0; j < flightGraph.cityCount; j++) 
                        {
                            if (flightGraph.cities[j] == node->flightInfo->city1)
                                srcPos = cityPositions[j];
                            if (flightGraph.cities[j] == node->flightInfo->city2)
                                destPos = cityPositions[j];
                        }

                        // Determine the line color based on the cost
                        Color lineColor;
                        if (flightCost >= 110000 && flightCost <= 300000)
                        {
                            lineColor = Color::Magenta;  // Expensive flight
                        }

                        else if (flightCost > 25000 && flightCost <= 1100000)
                        {
                            lineColor = Color::Yellow;  // Moderate flight
                        }
                        else
                        {
                            lineColor = Color::Red;  // Cheap flight
                        }

                        Vertex line[] = {
                            Vertex(srcPos, lineColor),
                            Vertex(destPos, lineColor)
                        };
                        window.draw(line, 2, Lines);

                        float deltaX = destPos.x - srcPos.x;
                        float deltaY = destPos.y - srcPos.y;
                        float angle = static_cast<float>(atan2(deltaY, deltaX) * 180 / PI);

                        flightNode* tempNode = node;
                        int flightIndex = 0;
                        while (tempNode != nullptr) 
                        {
                            float offset = (flightIndex % 2 == 0) ? -40.0f : 30.0f;
                            float ratio = 0.3f;
                            Vector2f airplanePos = srcPos + ratio * (destPos - srcPos);
                            airplanePos.x += offset;

                            airplaneSprite.setPosition(airplanePos.x - airplaneSprite.getGlobalBounds().width / 2,
                                airplanePos.y - airplaneSprite.getGlobalBounds().height / 2);
                            airplaneSprite.setRotation(angle);
                            window.draw(airplaneSprite);
                            flightIndex++;
                            tempNode = tempNode->next;
                        }
                    }
                    node = node->next;
                }
            }

            for (int i = 0; i < flightGraph.cityCount; i++) 
            {
                window.draw(cityNodes[i]);
                cityText.setString(flightGraph.cities[i]);
                cityText.setPosition(cityPositions[i].x - 20, cityPositions[i].y - 50);
                window.draw(cityText);
            }

            window.display();
        }
    }

    //function for visualize graph with path
    void visualizeGraphWithPath(graph& flightGraph, int windowWidth, int windowHeight, const string& source, const string& destination) 
    {
        const double PI = 3.14159265358979323846;
        RenderWindow window(VideoMode(windowWidth, windowHeight), "Flight Path Visualizer - SkyNav AI");
        window.setFramerateLimit(60);

        // Load background image with smoothing disabled
        Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("background.jpg"))
        {
            cout << "Error loading background image." << endl;
        }

        // Disable texture smoothing for sharper scaling
        backgroundTexture.setSmooth(true);

        // Create sprite for the background
        Sprite backgroundSprite;
        backgroundSprite.setTexture(backgroundTexture);

        // Scale the background to fit the window
        backgroundSprite.setScale(static_cast<float>(windowWidth) / backgroundTexture.getSize().x, static_cast<float>(windowHeight) / backgroundTexture.getSize().y);

        const int maxCities = 11;
        CircleShape cityNodes[maxCities];
        Vector2f cityPositions[maxCities];
        string cityNames[maxCities];
        int cityCount = 0;

        float centerX = static_cast<float>(windowWidth) / 2, centerY = static_cast<float>(windowHeight) / 2;
        float radius = 300.0f;

        for (int i = 0; i < flightGraph.cityCount; i++) 
        {
            float angle = static_cast<float>((2 * PI / flightGraph.cityCount) * i);
            cityPositions[i] = Vector2f(centerX + radius * cos(angle), centerY + radius * sin(angle));
            cityNames[i] = flightGraph.cities[i];

            cityNodes[i].setRadius(20);
            cityNodes[i].setFillColor(Color::Green);
            cityNodes[i].setOrigin(20, 20);
            cityNodes[i].setPosition(cityPositions[i]);
        }

        sf::Font font;
        if (!font.loadFromFile("Arial.ttf")) 
        {
            cout << "Error loading default font." << endl;
        }

        sf::Text cityText, flightText;
        cityText.setFont(font);
        cityText.setCharacterSize(18);
        cityText.setFillColor(Color::White);

        flightText.setFont(font);
        flightText.setCharacterSize(9);
        flightText.setFillColor(Color::White);

        Texture airplaneTexture;
        if (!airplaneTexture.loadFromFile("airplanered.png"))
        {
            cout << "Error loading airplane icon." << endl;
        }

        Sprite airplaneSprite(airplaneTexture);
        airplaneSprite.setScale(0.03f, 0.03f);

        // Dijkstra's algorithm with conditions
        int  dist[maxCities] = { 0 };
        string prev[maxCities];
        bool visited[maxCities] = { false };
        string earliestDate[maxCities];
        int earliestTime[maxCities] = { 0 };
        int flightEvaluationNumber[maxCities] = { 0 };

        for (int i = 0; i < flightGraph.cityCount; i++)
        {
            dist[i] = INT_MAX;
            prev[i] = "";
            visited[i] = false;
            earliestDate[i] = "";
            earliestTime[i] = 1440; // End of the day
        }

        int srcIndex = -1;
        for (int i = 0; i < flightGraph.cityCount; i++)
        {
            if (flightGraph.cities[i] == source) 
            {
                srcIndex = i;
                break;
            }
        }

        if (srcIndex == -1)
        {
            cout << "\nSource city not found!" << endl;
            return;
        }

        dist[srcIndex] = 0;
        earliestDate[srcIndex] = "1/12/2019"; // Start date
        earliestTime[srcIndex] = 0;          // Start time

        //Dijkstra’s algorithm
        while (true) 
        {
            int u = -1;
            int minDist = INT_MAX;

            for (int i = 0; i < flightGraph.cityCount; i++) 
            {
                if (!visited[i] && dist[i] < minDist) 
                {
                    minDist = dist[i];
                    u = i;
                }
            }

            if (u == -1)
                break;

            visited[u] = true;

            flightNode* neighbor = flightGraph.adjList[u].head;
            while (neighbor != nullptr)
            {
                int v = -1;
                for (int i = 0; i < flightGraph.cityCount; i++) 
                {
                    if (flightGraph.cities[i] == neighbor->city)
                    {
                        v = i;
                        break;
                    }
                }

                // Direct flight check from source to destination
                if (v != -1 && !visited[v])
                {
                    // Validate flight details
                    string depDate = neighbor->flightInfo->depdate;
                    string depTime = neighbor->flightInfo->departureTime;
                    string arrDate = earliestDate[u];
                    int arrTime = earliestTime[u];

                    if (depDate == arrDate && timeToMinutesfordijkstra(depTime) > arrTime)
                    {
                        int edgeCost = stringToInt(neighbor->flightInfo->cost);
                        if (dist[u] + edgeCost < dist[v])
                        {
                            dist[v] = dist[u] + edgeCost;
                            prev[v] = flightGraph.cities[u];
                            earliestDate[v] = depDate;
                            earliestTime[v] = timeToMinutesfordijkstra(neighbor->flightInfo->arrivalTime);
                        }
                    }
                }
                neighbor = neighbor->next;
            }
        }

        // Rebuild the path and calculate total cost
        int destIndex = -1;
        for (int i = 0; i < flightGraph.cityCount; i++)
        {
            if (flightGraph.cities[i] == destination)
            {
                destIndex = i;
                break;
            }
        }

        if (destIndex == -1 || dist[destIndex] == INT_MAX)
        {
            cout << "\nNo valid path found from " << source << " to " << destination << endl;
            return;
        }

        string path = destination;
        int totalCost = dist[destIndex];
        string shortestPathEdges[maxCities][2];
        int pathIndex = 0;

        while (prev[destIndex] != "") 
        {
            shortestPathEdges[pathIndex][0] = prev[destIndex];
            shortestPathEdges[pathIndex][1] = flightGraph.cities[destIndex];
            pathIndex++;

            for (int i = 0; i < flightGraph.cityCount; i++)
            {
                if (flightGraph.cities[i] == prev[destIndex])
                {
                    destIndex = i;
                    break;
                }
            }
        }

        while (window.isOpen()) 
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
            }

            window.clear(Color::Black);

            // Draw background image
            window.draw(backgroundSprite);

            for (int i = 0; i < flightGraph.cityCount; i++)
            {
                flightNode* node = flightGraph.adjList[i].head;
                while (node != nullptr) 
                {
                    if (node->flightInfo != nullptr)
                    {
                        Vector2f srcPos, destPos;
                        for (int j = 0; j < flightGraph.cityCount; j++) 
                        {
                            if (flightGraph.cities[j] == node->flightInfo->city1)
                                srcPos = cityPositions[j];
                            if (flightGraph.cities[j] == node->flightInfo->city2)
                                destPos = cityPositions[j];
                        }

                        Color lineColor = Color::Green;
                        bool isInPath = false;

                        for (int k = 0; k < pathIndex; k++)
                        {
                            if ((shortestPathEdges[k][0] == flightGraph.cities[i] && shortestPathEdges[k][1] == node->flightInfo->city2) || (shortestPathEdges[k][1] == flightGraph.cities[i] && shortestPathEdges[k][0] == node->flightInfo->city2))
                            {
                                isInPath = true;
                                break;
                            }
                        }

                        if (isInPath) 
                        {
                            lineColor = Color::Blue;
                        }

                        Vertex line[] = {
                            Vertex(srcPos, lineColor),
                            Vertex(destPos, lineColor)
                        };
                        window.draw(line, 2, Lines);
                    }
                    node = node->next;
                }
            }

            for (int i = 0; i < flightGraph.cityCount; i++)
            {
                window.draw(cityNodes[i]);
                cityText.setString(flightGraph.cities[i]);
                cityText.setPosition(cityPositions[i].x - 20, cityPositions[i].y - 50);
                window.draw(cityText);
            }

            window.display();
        }
    }

    // Function to convert string cost to int
    int stringToInt(const string& costStr)
    {
        int cost = 0;
        bool isNegative = (costStr[0] == '-');
        size_t startIndex = isNegative ? 1 : 0;

        // Convert string to integer
        for (size_t i = startIndex; i < costStr.length(); ++i)
        {
            cost = cost * 10 + (costStr[i] - '0');
        }

        return isNegative ? -cost : cost;
    }

    int timeToMinutesfordijkstra(const string& time)
    {
        int hours = stoi(time.substr(0, time.find(":")));
        int minutes = stoi(time.substr(time.find(":") + 1));

        return hours * 60 + minutes;

    }

    //dijkstra algo for finding cheapest/shortest path
    void findCheapestPath(string source, string destination)
    {
        const int MAX_COST = 10000000; // Representing infinity
        int* dist = new int[cityCount]; // Minimum cost to reach each city
        string* prev = new string[cityCount]; // Previous city for path reconstruction
        string* earliestDate = new string[cityCount]; // Earliest date to reach each city
        int* earliestTime = new int[cityCount]; // Earliest time to reach each city (in minutes)
        bool* processed = new bool[cityCount]; // Processed status for each city
        bool validPathFound = false; // Track if at least one valid path is found

        // Initialize distances, previous city, and time tracking
        for (int i = 0; i < cityCount; i++) 
        {
            dist[i] = MAX_COST;
            prev[i] = "";
            earliestDate[i] = "";
            earliestTime[i] = 1440; // Set to the end of the day (24 * 60 minutes)
            processed[i] = false;
        }

        // Find the source node in the adjacency list
        int srcIndex = -1;
        for (int i = 0; i < cityCount; i++)
        {
            if (cities[i] == source)
            {
                srcIndex = i;
                break;
            }
        }

        if (srcIndex == -1)
        {
            cout << "\n\t\t\t\033[1;31m Source city not found in the graph!\033[0m" << endl;
            delete[] dist;
            delete[] prev;
            delete[] earliestDate;
            delete[] earliestTime;
            delete[] processed;
            return;
        }

        dist[srcIndex] = 0; // Start from the source city
        earliestDate[srcIndex] = "1/12/2019"; // Starting date (assumed known)
        earliestTime[srcIndex] = 0; // Start from the beginning of the day

        bool updated;
        do 
        {
            updated = false;

            // Process each city
            for (int u = 0; u < cityCount; u++) 
            {
                if (processed[u]) continue; // Skip already fully processed cities

                // Traverse the adjacency list for city u
                flightNode* neighbor = adjList[u].head;
                while (neighbor != NULL)
                {
                    if (neighbor->flightInfo == NULL)
                    {
                        neighbor = neighbor->next;
                        continue;
                    }

                    int v = -1;
                    for (int i = 0; i < cityCount; i++)
                    {
                        if (cities[i] == neighbor->city)
                        {
                            v = i;
                            break;
                        }
                    }

                    if (v != -1)
                    {
                        // Ensure valid neighbor index
                        string depDate = neighbor->flightInfo->depdate;
                        string depTime = neighbor->flightInfo->departureTime;
                        string arrDate = earliestDate[u];
                        int arrTime = earliestTime[u];

                        int depTimeInMinutes = timeToMinutesfordijkstra(depTime);
                        int arrTimeInMinutes = timeToMinutesfordijkstra(neighbor->flightInfo->arrivalTime);

                        // Check if the departure is valid (date and time criteria)
                        if ((depDate > arrDate) || (depDate == arrDate && depTimeInMinutes > arrTime))
                        {
                            int edgeCost = stringToInt(neighbor->flightInfo->cost);

                            // Only update if this flight is better or is the first valid flight
                            if (dist[u] + edgeCost < dist[v])
                            {
                                dist[v] = dist[u] + edgeCost;
                                prev[v] = cities[u];
                                earliestDate[v] = depDate;
                                earliestTime[v] = arrTimeInMinutes;

                                validPathFound = true; // Mark that at least one valid path exists
                                updated = true; // Mark that an update occurred
                            }
                        }
                    }

                    neighbor = neighbor->next; // Move to the next flight
                }

                processed[u] = true; // Mark this city as processed
            }
        }
        while (updated); // Continue until no more updates

        // Reconstruct the path from destination to source
        int destIndex = -1;
        for (int i = 0; i < cityCount; i++)
        {
            if (cities[i] == destination) 
            {
                destIndex = i;
                break;
            }
        }

        if (!validPathFound || destIndex == -1 || dist[destIndex] == MAX_COST)
        {
            cout << "\n\t\t\t\033[1;31m No valid path found from " << source << " to " << destination << " \033[0m" << endl;
            delete[] dist;
            delete[] prev;
            delete[] earliestDate;
            delete[] earliestTime;
            delete[] processed;
            return;
        }

        // Output the path and total cost
        cout << "\n\n\t\t\t\t\t\033[1;36m Cheapest path from " << source << " to " << destination << ": \033[0m";
        string path = destination;
        int totalCost = dist[destIndex];

        while (prev[destIndex] != "")
        {
            path = prev[destIndex] + " -> " + path;
            for (int i = 0; i < cityCount; i++)
            {
                if (cities[i] == prev[destIndex]) 
                {
                    destIndex = i;
                    break;
                }
            }
        }

        cout << path << endl;
        cout << "\n\n\t\t\t\t\t\033[1;36m Total cost of the cheapest path:  \033[0m" << totalCost << endl;

        // Cleanup dynamically allocated memory
        delete[] dist;
        delete[] prev;
        delete[] earliestDate;
        delete[] earliestTime;
        delete[] processed;
    }

    // Function to visualize graph with the path for the specified airline
    void visualizeGraphForAirline(graph& flightGraph, int windowWidth, int windowHeight, const string& selectedAirline)
    {
        const double PI = 3.14159265358979323846;
        RenderWindow window(VideoMode(windowWidth, windowHeight), "Flight Path Visualizer - SkyNav AI");
        window.setFramerateLimit(60);

        // Load background image with smoothing disabled
        Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("background.jpg"))
        {
            cout << "Error loading background image." << endl;
        }

        // Disable texture smoothing for sharper scaling
        backgroundTexture.setSmooth(true);

        // Create sprite for the background
        Sprite backgroundSprite;
        backgroundSprite.setTexture(backgroundTexture);

        // Scale the background to fit the window
        backgroundSprite.setScale(
            static_cast<float>(windowWidth) / backgroundTexture.getSize().x,
            static_cast<float>(windowHeight) / backgroundTexture.getSize().y);

        const int maxCities = 11; // Assuming max 11 cities as per the graph initialization
        CircleShape cityNodes[maxCities];
        Vector2f cityPositions[maxCities];
        string cityNames[maxCities];
        int cityCount = 0;

        // Define circular layout for cities
        float centerX = static_cast<float>(windowWidth) / 2, centerY = static_cast<float>(windowHeight) / 2;
        float radius = 300.0f; // Original radius for the circular layout

        // Assign positions to cities in a circular layout
        for (int i = 0; i < flightGraph.cityCount; i++) 
        {
            // Calculate the base angle for each city
            float angle = static_cast<float>((2 * PI / flightGraph.cityCount) * i);
            cityPositions[i] = Vector2f(centerX + radius * cos(angle), centerY + radius * sin(angle));
            cityNames[i] = flightGraph.cities[i];

            // Setup the appearance of the city nodes
            cityNodes[i].setRadius(20);  // Node size remains fixed
            cityNodes[i].setFillColor(Color::Green);
            cityNodes[i].setOrigin(15, 15); // Center the circles
            cityNodes[i].setPosition(cityPositions[i]);
        }

        // Create SFML text with default font (no external font required)
        Font font;
        if (!font.loadFromFile("Arial.ttf"))
        { // Load Arial font from system
            cout << "Error loading default font." << endl;
        }

        Text cityText, flightText;
        cityText.setFont(font);
        cityText.setCharacterSize(18);
        cityText.setFillColor(Color::White);
        flightText.setFont(font);
        flightText.setCharacterSize(7);
        flightText.setFillColor(Color::White);

        // Load airplane texture (make sure you have an airplane image)
        Texture airplaneTexture;
        if (!airplaneTexture.loadFromFile("airplanered.png"))
        { // Load your airplane icon
            cout << "Error loading airplane icon." << endl;
        }

        Sprite airplaneSprite(airplaneTexture);
        // Resize the airplane sprite to make it smaller (scale it down to 0.04 for example)
        airplaneSprite.setScale(0.03f, 0.03f);  // Further reduce the size

        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
            }

            window.clear(Color::Black);

            // Draw background image
            window.draw(backgroundSprite);

            // Draw edges (flights) between cities for the selected airline
            for (int i = 0; i < flightGraph.cityCount; i++)
            {
                flightNode* node = flightGraph.adjList[i].head;

                while (node != NULL) 
                {
                    if (node->flightInfo != NULL) 
                    {
                        // Check if the flight belongs to the selected airline
                        if (node->flightInfo->airline == selectedAirline)
                        {
                            // Find source and destination positions
                            Vector2f srcPos, destPos;
                            for (int j = 0; j < flightGraph.cityCount; j++)
                            {
                                if (flightGraph.cities[j] == node->flightInfo->city1)
                                    srcPos = cityPositions[j];
                                if (flightGraph.cities[j] == node->flightInfo->city2)
                                    destPos = cityPositions[j];
                            }

                            // Draw line for the edge (flight) between cities
                            sf::Color neonPink(255, 215, 0);
                            Vertex line[] =
                            {
                                Vertex(srcPos, neonPink),
                                Vertex(destPos, neonPink)
                            };
                            window.draw(line, 2, Lines);  // Fixed to draw only two points (src and dest)

                            // Create rotation angle for airplane sprite
                            float deltaX = destPos.x - srcPos.x;
                            float deltaY = destPos.y - srcPos.y;
                            float angle = static_cast<float>(atan2(deltaY, deltaX) * 180 / PI); // Get angle in degrees

                            // Place airplane sprites closer to the source city
                            flightNode* tempNode = node;
                            int flightIndex = 0;  // Keep track of how many airplanes on this path
                            while (tempNode != NULL) 
                            {
                                // Horizontal offset: alternate left/right for each airplane
                                float offset = (flightIndex % 2 == 0) ? -20.0f : 20.0f;  // Alternate left (-) or right (+)

                                // Calculate the airplane position closer to the source city (20% of the distance from src to dest)
                                float ratio = 0.2f; // 20% of the distance
                                Vector2f airplanePos = srcPos + ratio * (destPos - srcPos);

                                // Adjust the airplane position based on the offset (left or right)
                                airplanePos.x += offset;

                                // Set airplane position
                                airplaneSprite.setPosition(airplanePos.x - airplaneSprite.getGlobalBounds().width / 2,
                                    airplanePos.y - airplaneSprite.getGlobalBounds().height / 2); // Adjust for multiple flights
                                airplaneSprite.setRotation(angle); // Rotate airplane to face the direction
                                window.draw(airplaneSprite);
                                flightIndex++;  // Increment for the next airplane
                                tempNode = tempNode->next;
                            }

                            // Display flight details near the flight path
                            flightText.setString(flightGraph.cities[i] + " -> " + node->flightInfo->city2); // Show the flight path details
                            flightText.setPosition((srcPos.x + destPos.x) / 2 - 100, (srcPos.y + destPos.y) / 2 - 40); // Adjust position to avoid overlap
                            window.draw(flightText);
                        }
                    }
                    node = node->next;
                }
            }

            // Draw city nodes
            for (int i = 0; i < flightGraph.cityCount; i++)
            {
                window.draw(cityNodes[i]);

                // Set text position and display city name
                cityText.setString(flightGraph.cities[i]);
                cityText.setPosition(cityPositions[i].x - 20, cityPositions[i].y - 30);  // Adjust position to avoid overlap
                window.draw(cityText);
            }
            window.display();
        }
    }

    // Helper function to convert date (dd/mm/yyyy) to a comparable integer format (yyyymmdd)
    int convertDateToInt(const string& date) 
    {
        int day, month, year;
        char slash;
        stringstream ss(date);
        ss >> day >> slash >> month >> slash >> year;
        return year * 10000 + month * 100 + day;
    }

    int calculateLayoverTime2(string arrival, string departure, string arrivalDate, string departureDate) 
    {
        int arrivalHours = stoi(arrival.substr(0, 2));
        int arrivalMinutes = stoi(arrival.substr(3, 2));
        int departureHours = stoi(departure.substr(0, 2));
        int departureMinutes = stoi(departure.substr(3, 2));

        // Convert the date strings to integer format
        int arrivalDateInt = convertDateToInt(arrivalDate);
        int departureDateInt = convertDateToInt(departureDate);

        // If the departure date is later than the arrival date, the layover happens the next day
        if (departureDateInt > arrivalDateInt) 
        {
            return (24 * 60 - (arrivalHours * 60 + arrivalMinutes)) + (departureHours * 60 + departureMinutes);
        }

        // Otherwise, calculate the layover within the same day
        int totalArrivalMinutes = arrivalHours * 60 + arrivalMinutes;
        int totalDepartureMinutes = departureHours * 60 + departureMinutes;

        if (totalDepartureMinutes >= totalArrivalMinutes) 
        {
            return totalDepartureMinutes - totalArrivalMinutes;
        }
        else
        {
            // Handle the case where the second flight departs before the first one arrives on the same date
            return (24 * 60 - totalArrivalMinutes) + totalDepartureMinutes;
        }
    }

    //gettor 
    string getNextDay(string currentDate) 
    {
        int day, month, year;
        char slash;
        stringstream ss(currentDate);
        ss >> day >> slash >> month >> slash >> year;

        // Handle date increment logic (simple for now, more robust logic can be added for month/year transitions)
        day += 1;
        if (day > 31) 
        {
            day = 1;
            month += 1;
            if (month > 12)
            {
                month = 1;
                year += 1;
            }
        }

        stringstream newDate;
        newDate << (day < 10 ? "0" : "") << day << "/" << (month < 10 ? "0" : "") << month << "/" << year;
        return newDate.str();
    }

    //next flight finding function
    bool findFlightOnNextDay(string middleCity, string destination, string nextDay) 
    {
        for (int i = 0; i < cityCount; i++)
        {
            if (cities[i] == middleCity) 
            {
                flightNode* sourceTemp = adjList[i].head;

                while (sourceTemp != nullptr)
                {
                    if (sourceTemp->flightInfo != nullptr && sourceTemp->flightInfo->destination == destination && sourceTemp->flightInfo->depdate == nextDay) 
                    {
                        return true;
                    }
                    sourceTemp = sourceTemp->next;
                }
            }
        }
        return false;
    }

    //compare time function
    bool compareTime(const string& time1, const string& time2)
    {
        int hour1, minute1, hour2, minute2;
        char colon;

        stringstream ss1(time1);
        ss1 >> hour1 >> colon >> minute1;

        stringstream ss2(time2);
        ss2 >> hour2 >> colon >> minute2;

        // Convert to minutes for easier comparison
        int timeInMinutes1 = hour1 * 60 + minute1;
        int timeInMinutes2 = hour2 * 60 + minute2;

        return timeInMinutes2 > timeInMinutes1; // Return true if time2 is later than time1
    }

    //manage layover function
    void manageLayovers(string source, string destination, string date, Queue& layoverQueue)
    {
       // Queue layoverQueue;
        bool connectingFlight = false;

        for (int i = 0; i < cityCount; i++)
        {
            if (cities[i] == source)
            {
                flightNode* sourceTemp = adjList[i].head;

                while (sourceTemp != nullptr)
                {
                    if (sourceTemp->flightInfo != nullptr && sourceTemp->flightInfo->depdate == date)
                    {
                        string middleCity = sourceTemp->flightInfo->destination;

                        for (int j = 0; j < cityCount; j++)
                        {
                            if (cities[j] == middleCity)
                            {
                                flightNode* destTemp = adjList[j].head;

                                while (destTemp != nullptr)
                                {
                                    if (destTemp->flightInfo != nullptr && destTemp->flightInfo->destination == destination && destTemp->flightInfo->depdate == date)
                                    {
                                        string layover1 = sourceTemp->flightInfo->arrivalTime;
                                        string layover2 = destTemp->flightInfo->departureTime;
                                        string arrivalDate = sourceTemp->flightInfo->depdate;
                                        string departureDate = destTemp->flightInfo->depdate;
                                        int layoverMinutes = 0;
                                        string nextDayDate = departureDate;
                                        bool checkForNextDate = false;

                                        // First compare the dates
                                        if (arrivalDate == departureDate)
                                        {
                                            // Compare the arrival time with the departure time of the second flight on the same day
                                            layoverMinutes = calculateLayoverTime2(layover1, layover2, arrivalDate, departureDate);

                                            if (layoverMinutes >= 60 && compareTime(layover1, layover2))
                                            {
                                                // If the layover is sufficient and departure time is after arrival time on the same day
                                                cout << "\n\033[1;32mLayover on the same day is valid. \nConnecting flight found:\033[0m" << endl;
                                            }
                                            else
                                            {
                                                cout << "\n\033[1;31mInvalid layover on the same day. Checking for flights on the next day: ";

                                                // Check for a flight on the next day
                                                nextDayDate = getNextDay(departureDate);  // Get the next day's date

                                                cout << nextDayDate << "\033[0m" << endl;
                                                if (findFlightOnNextDay(middleCity, destination, nextDayDate))
                                                {
                                                    // Update layover time to next day
                                                    layoverMinutes = calculateLayoverTime2(layover1, layover2, arrivalDate, nextDayDate);
                                                    cout << "\n\033[1;35mConnecting flight found on the next day: " << nextDayDate << "\033[0m" << endl;
                                                }
                                            }
                                        }
                                        else
                                        {
                                            // If the dates are different, check the next day's flight
                                            nextDayDate = getNextDay(arrivalDate);
                                            if (findFlightOnNextDay(middleCity, destination, nextDayDate))
                                            {
                                                layoverMinutes = calculateLayoverTime2(layover1, layover2, arrivalDate, nextDayDate);
                                                cout << "\n\033[1;35mConnecting flight found on the next day: " << nextDayDate << "\033[0m" << endl;
                                            }
                                        }

                                        if (layoverMinutes >= 60)
                                        {
                                            // Assuming minimum layover time is 60 minutes
                                            layoverQueue.enqueue(middleCity, sourceTemp->flightInfo, destTemp->flightInfo, layoverMinutes);

                                            //cout << "\n\t\033[1;32m Connecting Flight Found: \033[0m" << endl;
                                            sourceTemp->flightInfo->print();  // Print the first flight details

                                            // Check if the departure date of the second flight has changed (i.e., it is now the next day)
                                            if (nextDayDate != destTemp->flightInfo->depdate)
                                            {
                                                // Manually print the second flight details with the updated next day date
                                                cout << "Source: " << destTemp->flightInfo->city1
                                                    << " --> Destination: " << destTemp->flightInfo->city2
                                                    << ", Airline: " << destTemp->flightInfo->airline
                                                    << ", Cost: " << destTemp->flightInfo->cost
                                                    << ", Departure Time: " << destTemp->flightInfo->departureTime
                                                    << ", Arrival Time: " << destTemp->flightInfo->arrivalTime
                                                    << ", Date: " << nextDayDate << endl;
                                            }
                                            else
                                            {
                                                destTemp->flightInfo->print();  // Print the second flight details if the date is the same
                                            }

                                            cout << "\033[1;34mLayover at " << middleCity << ": " << layoverMinutes / 60 << " hours and " << layoverMinutes % 60 << " minutes.\033[0m" << endl;

                                            connectingFlight = true;
                                        }
                                    }
                                    destTemp = destTemp->next;
                                }
                            }
                        }
                    }
                    sourceTemp = sourceTemp->next;
                }
            }
        }

        if (!connectingFlight)
        {
            cout << "\n\033[1;31mNo connecting flights available with sufficient layover time!\033[0m" << endl;
        }
        else
        {
            // Display all layovers
            layoverQueue.display();
        }
    }

    //helper for drawing dashed lines for layover management
    void drawDashedLine(RenderWindow& window, Vector2f start, Vector2f end, Color color)
    {
        float dashLength = 10.0f;  // Length of each dash
        float gapLength = 5.0f;    // Length of the gap between dashes
        float totalLength = sqrt(pow(end.x - start.x, 2) + pow(end.y - start.y, 2));  // Total length of the line
        float angle = atan2(end.y - start.y, end.x - start.x);  // Calculate angle of the line

        // Determine the number of dashes
        int numDashes = static_cast<int>(totalLength / (dashLength + gapLength));

        for (int i = 0; i < numDashes; ++i)
        {
            float x1 = start.x + (i * (dashLength + gapLength)) * cos(angle);
            float y1 = start.y + (i * (dashLength + gapLength)) * sin(angle);
            float x2 = x1 + dashLength * cos(angle);
            float y2 = y1 + dashLength * sin(angle);

            Vertex line[] = {
                Vertex(Vector2f(x1, y1), color),
                Vertex(Vector2f(x2, y2), color)
            };

            window.draw(line, 2, Lines);
        }
    }

    //visualize graph function for Layover management with dashed lines
    void visualizeLayoversWithDashedPaths(graph& flightGraph, int windowWidth, int windowHeight, Queue& layoverQueue)
    {
        const double PI = 3.14159265358979323846;
        RenderWindow window(VideoMode(windowWidth, windowHeight), "Flight Layover Path Visualizer");
        window.setFramerateLimit(60);

        // Load background image with smoothing disabled
        Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("background.jpg"))
        {
            cout << "Error loading background image." << endl;
        }

        backgroundTexture.setSmooth(true);
        Sprite backgroundSprite;
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(static_cast<float>(windowWidth) / backgroundTexture.getSize().x, static_cast<float>(windowHeight) / backgroundTexture.getSize().y);

        // Setup for cities and layout
        const int maxCities = 11;
        CircleShape cityNodes[maxCities];
        Vector2f cityPositions[maxCities];
        string cityNames[maxCities];
        int cityCount = 0;

        float centerX = static_cast<float>(windowWidth) / 2, centerY = static_cast<float>(windowHeight) / 2;
        float radius = 300.0f;

        for (int i = 0; i < flightGraph.cityCount; i++)
        {
            float angle = static_cast<float>((2 * PI / flightGraph.cityCount) * i);
            cityPositions[i] = Vector2f(centerX + radius * cos(angle), centerY + radius * sin(angle));
            cityNames[i] = flightGraph.cities[i];

            cityNodes[i].setRadius(20);
            cityNodes[i].setFillColor(Color::Green);
            cityNodes[i].setOrigin(15, 15);
            cityNodes[i].setPosition(cityPositions[i]);
        }

        Font font;
        if (!font.loadFromFile("Arial.ttf"))
        {
            cout << "Error loading font." << endl;
        }

        Text cityText;
        cityText.setFont(font);
        cityText.setCharacterSize(18);
        cityText.setFillColor(Color::White);

        // Load airplane icon (optional)
        Texture airplaneTexture;
        if (!airplaneTexture.loadFromFile("airplanered.png"))
        {
            cout << "Error loading airplane icon." << endl;
        }

        Sprite airplaneSprite(airplaneTexture);
        airplaneSprite.setScale(0.03f, 0.03f);

        // Visualize the layovers and their paths in the queue
        while (window.isOpen())
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
            }

            window.clear(Color::Black);
            window.draw(backgroundSprite);

            // Iterate through the layover queue and draw dashed paths
            Queue::LayoverNode* layoverNode = layoverQueue.getFront();
            while (layoverNode != nullptr)
            {
                string sourceCity = layoverNode->firstFlight->city1;
                string intermediateCity = layoverNode->intermediateCity;
                string destinationCity = layoverNode->secondFlight->city2;

                // Get positions for cities
                Vector2f srcPos, intermediatePos, destPos;
                for (int i = 0; i < flightGraph.cityCount; i++)
                {
                    if (flightGraph.cities[i] == sourceCity)
                        srcPos = cityPositions[i];
                    if (flightGraph.cities[i] == intermediateCity)
                        intermediatePos = cityPositions[i];
                    if (flightGraph.cities[i] == destinationCity)
                        destPos = cityPositions[i];
                }

                // Draw dashed lines for layovers
                drawDashedLine(window, srcPos, intermediatePos, Color::Yellow);
                drawDashedLine(window, intermediatePos, destPos, Color::Magenta);

                // Airplane icon movement from source -> intermediate -> destination
                Vector2f airplanePos;

                // Airplane position 20% along the path from source to intermediate
                airplanePos = srcPos + 0.2f * (intermediatePos - srcPos);
                // Calculate the angle of rotation for the airplane
                float angle1 = atan2(intermediatePos.y - srcPos.y, intermediatePos.x - srcPos.x);
                airplaneSprite.setRotation(angle1 * 180 / PI); // Convert radians to degrees
                airplaneSprite.setPosition(airplanePos.x - airplaneSprite.getGlobalBounds().width / 2, airplanePos.y - airplaneSprite.getGlobalBounds().height / 2);
                window.draw(airplaneSprite);

                // Move airplane from intermediate to destination (20% along the path)
                airplanePos = intermediatePos + 0.2f * (destPos - intermediatePos);
                // Calculate the angle of rotation for the airplane
                float angle2 = atan2(destPos.y - intermediatePos.y, destPos.x - intermediatePos.x);
                airplaneSprite.setRotation(angle2 * 180 / PI); // Convert radians to degrees
                airplaneSprite.setPosition(airplanePos.x - airplaneSprite.getGlobalBounds().width / 2,
                    airplanePos.y - airplaneSprite.getGlobalBounds().height / 2);
                window.draw(airplaneSprite);

                layoverNode = layoverNode->next;
            }

            // Draw city nodes and names
            for (int i = 0; i < flightGraph.cityCount; i++)
            {
                window.draw(cityNodes[i]);

                cityText.setString(flightGraph.cities[i]);
                cityText.setPosition(cityPositions[i].x - 20, cityPositions[i].y - 30);
                window.draw(cityText);
            }

            window.display();
        }
    }

    // Advanced Route Generation function
    void advancedRouteGeneration(graph& flightGraph, Queue& layoverQueue, LinkedList& journeyList, int cityCount, string source, string destination, string date)
    {
        char userChoice;
        string layoverCity;

        while (true) 
        {
            cout << "\nDo you want to add a layover city? (y/n): ";
            cin >> userChoice;
            if (userChoice == 'y' || userChoice == 'Y') 
            {
                cout << "\033[1;33mEnter Layover City: \033[0m";
                cin >> layoverCity;

                // Calculate layover time and find connecting flights
                flight* firstFlight = nullptr;
                flight* secondFlight = nullptr;
                int layoverDuration = 0;

                // Iterate through layover queue to find flights for the selected layover city
                layoverQueue.resetTraversal(); // Assuming a resetTraversal method for queue to iterate from the start
                while (layoverQueue.hasNext()) 
                {
                    Queue::LayoverNode* layover = layoverQueue.next();

                    if (layover->intermediateCity == layoverCity) 
                    {
                        firstFlight = layover->firstFlight;
                        secondFlight = layover->secondFlight;
                        layoverDuration = layover->layoverMinutes; // Calculated automatically in manageLayovers
                        break;
                    }
                }

                if (firstFlight != nullptr && secondFlight != nullptr) 
                {
                    // Add the segment to the journey list
                    journeyList.addSegment(source, destination, layoverCity, firstFlight, secondFlight, layoverDuration);
                    cout << "\n\033[1;32mLayover at " << layoverCity << " with a duration of " << layoverDuration / 60 << " hours and " << layoverDuration % 60 << " minutes added successfully!\n\033[0m";
                }
                else 
                {
                    cout << "\n\033[1;31mNo valid connecting flights found for the selected layover city.\n\033[0m";
                }
            }

            cout << "\nDo you want to remove any layover city? (y/n): ";
            cin >> userChoice;
            if (userChoice == 'y' || userChoice == 'Y')
            {
                cout << "\n\033[1;33mEnter Layover City to Remove: \033[0m";
                cin >> layoverCity;
                journeyList.removeSegment(source, destination);
            }

            cout << "\nDo you want to confirm the current route? (y/n): ";
            cin >> userChoice;
            if (userChoice == 'y' || userChoice == 'Y')
            {
                cout << "\nDo you want to stay in a Hotel? (y/n): ";
                cin >> userChoice;
                if (userChoice == 'y' || userChoice == 'Y')
                {
                    // Finalize the journey
                    int totalCost = journeyList.calculateTotalCost(layoverCity);
                    cout << "\nTotal Cost of Journey with Hotel Charges: $" << totalCost << endl;
                    cout << "\033[1;32mRoute confirmed. Booking the flight...\033[0m\n";
                }
                else 
                {
                    // Finalize the journey
                    int totalCost = journeyList.calculateTotalCost("");
                    cout << "\nTotal Cost of Journey without Hotel Charges: $" << totalCost << endl;
                    cout << "\033[1;32mRoute confirmed. Booking the flight...\033[0m\n";
                }
                break;
            }

            // Modify source for the next segment if the user wants to extend the route
            source = destination;
            cout << "\nEnter New Destination City: ";
            cin >> destination;
        }
    }

    void drawDashedLineWithArrow(RenderWindow& window, Vector2f start, Vector2f end, Color color)
    {
        float dashLength = 10.0f;  // Length of each dash
        float gapLength = 5.0f;    // Length of the gap between dashes
        float totalLength = sqrt(pow(end.x - start.x, 2) + pow(end.y - start.y, 2));  // Total length of the line
        float angle = atan2(end.y - start.y, end.x - start.x);  // Calculate angle of the line

        // Determine the number of dashes
        int numDashes = static_cast<int>(totalLength / (dashLength + gapLength));

        // Draw the dashes
        for (int i = 0; i < numDashes; ++i)
        {
            float x1 = start.x + (i * (dashLength + gapLength)) * cos(angle); 
            float y1 = start.y + (i * (dashLength + gapLength)) * sin(angle); 
            float x2 = x1 + dashLength * cos(angle); 
            float y2 = y1 + dashLength * sin(angle); 

            Vertex dash[] = { 
                Vertex(Vector2f(x1, y1), color), 
                Vertex(Vector2f(x2, y2), color) 
            };

            window.draw(dash, 2, Lines); 
        }

        // Draw the triangle (arrow) at the end of the line
        float arrowSize = 10.0f;  // Size of the triangle
        Vector2f arrowPoint1 = end - Vector2f(arrowSize * cos(angle - 0.5f), arrowSize * sin(angle - 0.5f)); 
        Vector2f arrowPoint2 = end - Vector2f(arrowSize * cos(angle + 0.5f), arrowSize * sin(angle + 0.5f)); 

        // Create a triangle to represent the arrowhead
        Vertex arrow[] = { 
            Vertex(end, color),            // Tip of the arrow
            Vertex(arrowPoint1, color),   // Left side of the triangle
            Vertex(arrowPoint2, color)    // Right side of the triangle
        };

        window.draw(arrow, 3, Triangles);   // Draw the arrow as a triangle
    }

    // New function to visualize the journey stored in the LinkedList
    void visualizeJourneyFromLinkedList(LinkedList& journeyList, graph& flightGraph, int windowWidth, int windowHeight)
    {
        const double PI = 3.14159265358979323846;
        RenderWindow window(VideoMode(windowWidth, windowHeight), "Journey Visualization");
        window.setFramerateLimit(60);

        // Load background image with smoothing disabled
        Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("background.jpg"))
        {
            cout << "Error loading background image." << endl;
        }
        backgroundTexture.setSmooth(true);
        Sprite backgroundSprite;
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(static_cast<float>(windowWidth) / backgroundTexture.getSize().x, static_cast<float>(windowHeight) / backgroundTexture.getSize().y);

        // Setup for cities and layout
        const int maxCities = 11;
        CircleShape cityNodes[maxCities];
        Vector2f cityPositions[maxCities];
        string cityNames[maxCities];
        int cityCount = 0;

        float centerX = static_cast<float>(windowWidth) / 2, centerY = static_cast<float>(windowHeight) / 2;
        float radius = 300.0f;

        for (int i = 0; i < flightGraph.cityCount; i++)
        {
            float angle = static_cast<float>((2 * PI / flightGraph.cityCount) * i);
            cityPositions[i] = Vector2f(centerX + radius * cos(angle), centerY + radius * sin(angle));
            cityNames[i] = flightGraph.cities[i];

            cityNodes[i].setRadius(20);
            cityNodes[i].setFillColor(Color::Green);
            cityNodes[i].setOrigin(15, 15);
            cityNodes[i].setPosition(cityPositions[i]);
        }

        Font font;
        if (!font.loadFromFile("Arial.ttf")) 
        {
            cout << "Error loading font." << endl;
        }
        Text cityText;
        cityText.setFont(font);
        cityText.setCharacterSize(18);
        cityText.setFillColor(Color::White);

        // Load airplane icon
        Texture airplaneTexture;
        if (!airplaneTexture.loadFromFile("airplanered.png"))
        {
            cout << "Error loading airplane icon." << endl;
        }
        Sprite airplaneSprite(airplaneTexture);
        airplaneSprite.setScale(0.03f, 0.03f);

        // Visualize the journey and its flight segments
        while (window.isOpen()) 
        {
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
            }

            window.clear(Color::Black);
            window.draw(backgroundSprite);

            // Iterate through the journey list and draw arrows for each segment
            JourneyNode* journeyNode = journeyList.head;
            while (journeyNode != nullptr)
            {
                string sourceCity = journeyNode->sourceCity;
                string destinationCity = journeyNode->destinationCity;
                string layoverCity = journeyNode->layoverCity;

                // Get positions for cities
                Vector2f srcPos, destPos, layoverPos;
                for (int i = 0; i < flightGraph.cityCount; i++)
                {
                    if (flightGraph.cities[i] == sourceCity)
                        srcPos = cityPositions[i];
                    if (flightGraph.cities[i] == destinationCity)
                        destPos = cityPositions[i];
                    if (flightGraph.cities[i] == layoverCity)
                        layoverPos = cityPositions[i];
                }

                // Draw dashed lines between cities
                if (!layoverCity.empty()) 
                {
                    drawDashedLineWithArrow(window, srcPos, layoverPos, Color::Red); // Source to layover
                    drawDashedLineWithArrow(window, layoverPos, destPos, Color::White); // Layover to destination
                }
                else
                {
                    drawDashedLineWithArrow(window, srcPos, destPos, Color::Cyan); // Direct flight
                }

                // Draw the airplane icon at various positions
                Vector2f airplanePos;

                // Airplane 20% along the path from source to layover
                if (!layoverCity.empty())
                {
                    airplanePos = srcPos + 0.2f * (layoverPos - srcPos);
                    float angle1 = atan2(layoverPos.y - srcPos.y, layoverPos.x - srcPos.x);
                    airplaneSprite.setRotation(angle1 * 180 / PI);
                    airplaneSprite.setPosition(airplanePos.x - airplaneSprite.getGlobalBounds().width / 2, airplanePos.y - airplaneSprite.getGlobalBounds().height / 2);
                    window.draw(airplaneSprite);

                    // Airplane 20% along the path from layover to destination
                    airplanePos = layoverPos + 0.2f * (destPos - layoverPos);
                    float angle2 = atan2(destPos.y - layoverPos.y, destPos.x - layoverPos.x);
                    airplaneSprite.setRotation(angle2 * 180 / PI);
                    airplaneSprite.setPosition(airplanePos.x - airplaneSprite.getGlobalBounds().width / 2, airplanePos.y - airplaneSprite.getGlobalBounds().height / 2);
                    window.draw(airplaneSprite);
                }
                else 
                {
                    // Airplane 20% along the path from source to destination
                    airplanePos = srcPos + 0.2f * (destPos - srcPos);
                    float angle1 = atan2(destPos.y - srcPos.y, destPos.x - srcPos.x);
                    airplaneSprite.setRotation(angle1 * 180 / PI);
                    airplaneSprite.setPosition(airplanePos.x - airplaneSprite.getGlobalBounds().width / 2,
                        airplanePos.y - airplaneSprite.getGlobalBounds().height / 2);
                    window.draw(airplaneSprite);
                }

                journeyNode = journeyNode->next;
            }

            // Draw city nodes and names
            for (int i = 0; i < flightGraph.cityCount; i++)
            {
                window.draw(cityNodes[i]);
                cityText.setString(flightGraph.cities[i]);
                cityText.setPosition(cityPositions[i].x - 20, cityPositions[i].y - 30);
                window.draw(cityText);
            }

            window.display();
        }
    }

};


/////// MAIN FUNCTION ///////
int main()
{
    int choice;
    graph flightGraph(11);  // Initialize graph with a fixed number of cities
    string line;
    ifstream file("Flights.txt");
    string select;
    cout << "\n\n\t\t\t\t\t\t\033[35m*************************************************************************************\033[0m\n"

        << "            \t\t\t\t\t\033[35m**                                                                                 **\033[0m\n"

        << "            \t\t\t\t\t\033[35m** \033[0m" << "\033[1;36m                            ---DATA STRUCTURES---                            \033[0m   " << "\033[35m**\033[0m\n"

        << "            \t\t\t\t\t\033[35m** \033[0m" << "\033[1;36m                               SEMESTER PROJECT                              \033[0m   " << "\033[35m**\033[0m\n"

        << "            \t\t\t\t\t\033[35m** \033[0m" << "\033[1;36m                       LAIBA NASIR , SYEDA SHANZAY SHAH                      \033[0m   " << "\033[35m**\033[0m\n"

        << "            \t\t\t\t\t\033[35m** \033[0m" << "\033[1;36m                        ROLL NO . 23I-2079  , 23I-2016                       \033[0m   " << "\033[35m**\033[0m\n"

        << "            \t\t\t\t\t\033[35m** \033[0m" << "\033[1;36m                                    CYS-A                                    \033[0m   " << "\033[35m**\033[0m\n"

        << "            \t\t\t\t\t\033[35m** \033[0m" << "\033[1;36m                       FLIGHT PATH VISUALIZER - SkyNav AI                    \033[0m   " << "\033[35m**\033[0m\n"

        << "            \t\t\t\t\t\033[35m**                                                                                 **\033[0m\n"

        << "            \t\t\t\t\t\033[35m*************************************************************************************\033[0m\n";
    if (!file)
    {
        cout << "Error opening the file!" << endl;
        return 1;
    }


    while (getline(file, line))
    {
        string city1, city2, depDate, depTime, arrTime, costStr, airline;

        stringstream ss(line);
        ss >> city1 >> city2 >> depDate >> depTime >> arrTime >> costStr >> airline;

        // Insert cities into the graph
        flightGraph.insertCity(city1);
        flightGraph.insertCity(city2);

        // Insert edge (flight) between city1 and city2
        flightGraph.insertEdge(city1, city2, depTime, costStr, airline, depDate, arrTime);
    }

    file.close();

    do
    {
        cout << "\n\n\t\t\t\t\t\t\t\t\033[1;34m************* FEATURES OF SkyNav AI ****************" << endl
            << "1- See All Flights" << endl
            << "2- See Flights of Specific Airlines "<< endl
            << "3- View Paths Price-wise " << endl                ///bonus
            << "4- Search A Flight" << endl
            << "5- Book A Flight " << endl
            << "6- See The Cheaper And Shorter Route" << endl
            << "7- Custom Flight Paths and Preferences" << endl
            << "8- Layover Management And Advanced Route Generation"<< endl 
            << "9- Exit\033[0m\n" << endl << endl;

        cout << "Enter Your Choice :  ";
        cin >> choice;

        Queue layoverQueue;
        LinkedList journeyList;

        switch (choice)
        {
            case 1:
            {
                // Print the final graph
                flightGraph.printGraph();
                flightGraph.visualizeGraphForViewing(flightGraph, 1000, 800);
                break;

            }
            case 2:
            {
                string airlines;
                cout << "\033[1;33m Enter Airline: \033[0m";
                cin >> airlines;

                flightGraph.visualizeGraphForAirline(flightGraph, 1000, 800, airlines);
                break;
            }

            case 3:
            {
                cout << "\n\t\t\t\033[1;37m >> Expensive Flights ::\033[0m \033[1;35m Pink \033[0m"
                    << " \n\t\t\t\033[1;37m >> Moderate Flights  ::\033[0m \033[1;33m Yellow \033[0m"
                    << " \n\t\t\t\033[1;37m >> Cheap Flights     ::\033[0m \033[1;31m Red \033[0m";
                
                flightGraph.visualizeGraph(flightGraph, 1000, 800);
                break;
            }

            case 4:
            {
                cout << "\n\n\n\t\t\t\t\t\t\t\t\t\033[36m******** GRAPH ***********\033[0m\n\n";

                // Print the final graph
                flightGraph.printGraph();

                string source, destination, date;
                cout << "\n\033[1;33m Enter Source City: \033[0m";
                cin >> source;
                cout << "\033[1;33m Enter Destination City: \033[0m";
                cin >> destination;
                cout << "\033[1;33m Enter Date (D/MM/YYYY): \033[0m";
                cin >> date;

                flightGraph.searchFlight(source, destination, date);
                flightGraph.visualizeGraphForSearch(flightGraph, 1000, 800, source, destination, date);
                break;
            }

            case 5:
            {
                // Print the final graph
                flightGraph.printGraph();

                string source, destination, date;
                cout << "\n\033[1;33m Enter Source City: \033[0m";
                cin >> source;
                cout << "\033[1;33m Enter Destination City: \033[0m";
                cin >> destination;
                cout << "\033[1;33m Enter Date (D/MM/YYYY): \033[0m";
                cin >> date;

                int choice = flightGraph.booking(flightGraph, source, destination, date);
                //flightGraph.visualizeGraphForBooking(flightGraph, 1000, 800, source, destination, date, choice);

                cout << "\n\n\t\t\t\t\t\t\t\t\t\033[1;32m THANKYOU FOR FLYING WITH SKY-NAV\n\n\033[0m";

                break;
            }

            case 6:
            {
               string source, destination, date;
                cout << "\n\033[1;33m Enter Source City: \033[0m";
                cin >> source;
                cout << "\033[1;33m Enter Destination City: \033[0m";
                cin >> destination;
                flightGraph.findCheapestPath(source, destination);
                flightGraph.visualizeGraphWithPath(flightGraph, 1000, 800, source, destination); 
                break;
            }
            case 7:
            {
                cout << "\n\n\n\t\t\t\t\t\t\t\t\t\033[36m******** GRAPH ***********\033[0m\n\n";

                string airline;
                string layovercITY;

                // Print the final graph
                flightGraph.printGraph();

                cout << "\n\033[1;33m Enter the Airline : \033[0m";
                cin >> airline;
                cout << "\033[1;33m Enter the Layover City : \033[0m";
                cin >> layovercITY;

                flightGraph.CustomUserPreference(airline, layovercITY);
                flightGraph.visualizeCustomFlights(flightGraph, airline, layovercITY, 1000, 800);

                break;
            }

            case 8:
            {
                string source, destination, date;
                cout << "\n\033[1;33mEnter Source City: \033[0m";
                cin >> source;
                cout << "\033[1;33mEnter Destination City: \033[0m";
                cin >> destination;
                cout << "\033[1;33mEnter Date (D/MM/YYYY): \033[0m";
                cin >> date;

                // Call the function to manage layovers and generate the route
                flightGraph.manageLayovers(source, destination, date, layoverQueue);
                flightGraph.visualizeLayoversWithDashedPaths(flightGraph, 1000, 800, layoverQueue);
                flightGraph.advancedRouteGeneration(flightGraph, layoverQueue, journeyList, 11, source, destination, date);
                flightGraph.visualizeJourneyFromLinkedList(journeyList, flightGraph, 1000, 800);
                break;
            }

            case 9:
            {
                cout << "\n\n\t\t\t\t\t\t\t\t\t\033[1;32m THANKYOU FOR FLYING WITH SKY-NAV\n\t\t\t\t\t\t\t\t\t    GOODBYE :) SEE YOU SOON !!!\n\n\n\n\033[0m";
                exit(0);
            }
        }
    } 
    while (choice != 9);
    return 0;
}