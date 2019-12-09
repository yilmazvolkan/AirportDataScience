#include <iostream>
#include <fstream>
#include <queue>
#include <ctime>
#include <iomanip>
using namespace std;
typedef pair<int,int> ii;
#define MAXP 50005
#define MAXL 26
#define MAXS 26
#define ArrivalEvent 0
#define LuggageOverEvent 1
#define SecurityOverEvent 2
struct person{
    int arrival,flight,secT,lugT;
    bool vip,lug;
} passengers[MAXP];
int P, L, S;

class airportQueue{
    public:
    bool ffly;
    queue <ii> q;
    priority_queue <ii,vector<ii>,greater<ii> > pq;
    airportQueue(bool ff)
    {
        ffly=ff;
    }
    void push(ii per)
    {
        if(ffly)
            pq.push(per);
        else
            q.push(per);
    }
    ii front()
    {
        if(ffly)
            return pq.top();
        else
            return q.front();
    }
    void pop()
    {
        if(ffly)
            return pq.pop();
        else
            return q.pop();
    }

    bool empty()
    {
        if(ffly)
            return pq.empty();
        else
            return q.empty();
    }
};

class event{
public:
    int counterID,leaveTime,personID,eventType,arrivalTime; // 0 arrival, 1 luggage over, 2 security over
    event(int counterID, int leaveTime, int personID, int eventType,int arrivalTime) : counterID(counterID), leaveTime(leaveTime),
                                                        personID(personID),eventType(eventType),arrivalTime(arrivalTime) {}
    bool operator<(event other) const
    {
        if(leaveTime == other.leaveTime)
        {
            if(eventType == other.eventType)
                return arrivalTime > other.arrivalTime;
            return eventType < other.eventType;
        }
        return leaveTime > other.leaveTime;
    }


};
int main(int argc, char* argv[]) {
    clock_t tStart = clock();
    if(argc!=3) {
        cout << "This code needs 2 files." << endl;
        return 0;
    }
    ifstream inputFile(argv[1]);
    ofstream outputFile(argv[2]);

    inputFile >> P >> L >> S;
    for(int i=0;i<P;i++)
    {
        inputFile >> passengers[i].arrival >> passengers[i].flight >> passengers[i].lugT >> passengers[i].secT;
        char c;
        inputFile >> c;
        passengers[i].vip = (c=='V');
        inputFile >> c;
        passengers[i].lug = (c=='L');
    }
    for(int onlineskip=0;onlineskip<2;onlineskip++)
        for(int vipskip=0;vipskip<2;vipskip++)
            for(int ffly=0;ffly<2;ffly++)
            {
                cout << "CASE:" << 4*onlineskip+2*vipskip+ffly+1 << endl;
                double totalWaitingTime=0;
                int missedFlightCount=0;
                airportQueue luggageQueue(ffly),securityQueue(ffly);
                priority_queue <event> eventQueue;
                bool luggageCounters[L]={false},securityCounters[S]={false};
                int freeLuggageCounters=L,freeSecurityCounters=S;
                for(int i=0;i<P;i++)
                {
                    event e(-1,passengers[i].arrival,i,ArrivalEvent,passengers[i].arrival);
                    eventQueue.push(e);
                }
                while(!eventQueue.empty())
                {
                    event e = eventQueue.top();
                    eventQueue.pop();
                   // cout << "Passenger:" << e.personID+1 << " Time:" << e.leaveTime << endl;
                    if(e.eventType == ArrivalEvent)
                    {
                        if(onlineskip && !passengers[e.personID].lug)
                        {
                            event e2(-1,e.leaveTime,e.personID,LuggageOverEvent,passengers[e.personID].arrival);
                            eventQueue.push(e2);
                        }
                        else
                        {
                            luggageQueue.push(ii(passengers[e.personID].flight, e.personID));
                        }
                    }
                    if(e.eventType == LuggageOverEvent)
                    {
                        if(e.counterID!=-1)
                        {
                            luggageCounters[e.counterID]= false;
                            freeLuggageCounters++;
                        }
                        if(vipskip && passengers[e.personID].vip)
                        {
                            event e2(-1,e.leaveTime,e.personID,SecurityOverEvent,passengers[e.personID].arrival);
                            eventQueue.push(e2);
                        }
                        else
                        {
                            securityQueue.push(ii(passengers[e.personID].flight, e.personID));
                        }
                    }
                    if (e.eventType == SecurityOverEvent)
                    {
                        if(e.counterID!=-1)
                        {
                            securityCounters[e.counterID] = false;
                            freeSecurityCounters++;
                        }
                        cout << "Person:" << e.personID+1 << " Arriv:" << e.arrivalTime << " Leav:" << e.leaveTime << endl;
                        totalWaitingTime += e.leaveTime-passengers[e.personID].arrival;
                        if(e.leaveTime > passengers[e.personID].flight)
                            missedFlightCount++;
                    }

                    while(!luggageQueue.empty() && freeLuggageCounters>0)
                    {
                        int freeCounter=0;
                        for(;luggageCounters[freeCounter];freeCounter++);
                        luggageCounters[freeCounter] = true;
                        freeLuggageCounters--;
                        int frontPerson = luggageQueue.front().second;
                        luggageQueue.pop();
                        event e2(freeCounter,e.leaveTime+passengers[frontPerson].lugT,frontPerson,LuggageOverEvent,passengers[frontPerson].arrival);
                        eventQueue.push(e2);
                    }
                    while(!securityQueue.empty() && freeSecurityCounters>0)
                    {
                        int freeCounter=0;
                        for(;securityCounters[freeCounter];freeCounter++);
                        securityCounters[freeCounter] = true;
                        freeSecurityCounters--;
                        int frontPerson = securityQueue.front().second;
                        securityQueue.pop();
                        event e2(freeCounter,e.leaveTime+passengers[frontPerson].secT,frontPerson,SecurityOverEvent,passengers[frontPerson].arrival);
                        eventQueue.push(e2);
                    }
                }
                outputFile << std::fixed << std::setprecision(5) << (double)totalWaitingTime/P << " " << missedFlightCount << endl;
            }
    printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
    return 0;
}