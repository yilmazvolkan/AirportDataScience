#include <iostream>
#include <queue>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

/*This is the LuggageCounter class
 *People leave their buggage here.
 *while they leaving it takes some time to process their luggages.
 */
class LuggageCounter{
public:
    bool is_busy=false;

public: LuggageCounter(){
        this->is_busy=false;
    }
};

/*This is the SecurityCounter class
 *A person passses through the security check
 *They need some time to take out liquids and their metal belongings.
 */
class SecurityCounter{
public:
    bool is_busy=false;

public: SecurityCounter(){
        this->is_busy=false;
    }
};

/*This is the Passenger class
 *A person who has a flight and therefore comes to airport.
 */
struct Passenger{

	//Where is the customer: 0:arrived,1:LuggageCounter,2:SecurityCounter
    int currentSit = 0; 
    //When the customer comes to the airport
    double arrival_time;
    //When his/her flight departures
    double flight_time;
    //How much time will be spend in luggage process
    double luggage_time;
    //How much time will be spend in security process
    double security_time;
    //VIP privilege
    int isVIP=0; //V=1 N=0
    //Has a Luggage or not
    int isLuggageFree=0; //N=1 L=0
    //Total time spend on the system
    double waiting_time; 
    //Current time is the ending time of the process the customer is in
    double current_time;
    //Where is for determining in which luggage or security the customer is
    int where = -1;
    Passenger(double arrivalTime, double flight_time, double luggage_time, double security_time, int isVIP, int isLuggageFree){
        this->arrival_time = arrivalTime;
        this->flight_time = flight_time;
        this->luggage_time = luggage_time;
        this->security_time = security_time;
        this->isVIP = isVIP;
        this->isLuggageFree = isLuggageFree;
        this->current_time = arrivalTime;
        this->currentSit = 0;
    }
    Passenger(){

    }
};
//This method returns the first LuggageCounter who's empty if no one is empty, returns -1
static int whichLCEmpty(LuggageCounter lc[],int N){
    for(int i=0; i<N; i++){
        if(lc[i].is_busy== false)
            return i;
    }
    return -1;
}
//This method returns the first SecurityCounter who's empty if no one is empty, returns -1
static int whichSCEmpty(SecurityCounter sc[],int N){
    for(int i=0;i<N;i++){
        if(sc[i].is_busy==false){
            return i;
        }
    }
    return -1;
}
/*This for the event time priority queue
 *If current times are equal the priority is current sits and secuirty leaves early.
 *If they are all equal, we look which passenger comes first to the airport.
 */
struct CompareToTime{
    bool operator()(Passenger& pass1, Passenger& pass2){
        if(pass1.current_time == pass2.current_time){
            if(pass1.currentSit == pass2.currentSit){
                return pass1.arrival_time > pass2.arrival_time;
            }
            return pass1.currentSit < pass2.currentSit;
        }
        return pass1.current_time > pass2.current_time;
    }
};

//This for the flight time priority queue
struct CompareToFlight{
    bool operator()(Passenger& pass1, Passenger& pass2){
        if(pass1.flight_time == pass2.flight_time){
            return pass1.arrival_time > pass2.arrival_time;
        }
        return pass1.flight_time > pass2.flight_time;
    }
};

//Process the input
template <class Container>
void split1(const string& str, Container& cont)
{
    istringstream iss(str);
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(cont));
}
int main(int argc, char* argv[]) {
    // Below reads the input file
    if (argc != 3) {
        cout << "Run the code with the following command: ./project2 [input_file] [output_file]" << endl;
        return 1;
    }
    cout << "input file: " << argv[1] << endl;
    cout << "output file: " << argv[2] << endl;

    int L = 0; //Number of LUGGAGE
    int S = 0; //Number of SECURTIY
    int P = 0; //Number of Passengers (M)
    ifstream infile(argv[1]);
    string line;
    vector <string> input;
    // Process the first line and get the relevant numbers
    getline(infile, line);
    cout << line << endl;
    vector <string> words;
    split1(line, words);
    cout << words.size() << endl;
    P = atof(words[0].c_str());
    L = atof(words[1].c_str());
    S = atof(words[2].c_str());

    // FIRST CASE

    //Customers array
    Passenger passengersFirst[P];
    //Luggage counters array
    LuggageCounter lcs[L];
    //Security counters array
    SecurityCounter scs[S];

    //Time priority queue for events
    priority_queue <Passenger, vector<Passenger>, CompareToTime> timeLine;
    //Time priority queue for luggage
    priority_queue <Passenger, vector<Passenger>, CompareToTime> luggageq;
    //Time priority queue for security
    priority_queue <Passenger, vector<Passenger>, CompareToTime> securityq;

    //This for loop is adding the customers to the array
    for (int i = 0; i < P; i++) {
        getline(infile, line);
        vector <string> words;
        split1(line, words);
        int isVIP = 0; //V=1 N=0
        int isLuggageFree = 0; //L=1 N=0
        if (words[4] == "V") {
            isVIP = 1;
        }
        if (words[5] == "N") {
            isLuggageFree = 1;
        }
        //According to given input it is all set
        Passenger c(atof(words[0].c_str()), atof(words[1].c_str()), atof(words[2].c_str()), atof(words[3].c_str()),
                    isVIP, isLuggageFree);
        passengersFirst[i] = c;
        timeLine.push(c);
    }

    //Time is the total time
    double time = 0;
    //Number of passengers with missing flight
    int w = 0;
    //Max length of the security queue
    int maxSecq = 20;
    //Max length of the luggage queue
    int maxLugq = 20;
    //Initialize the waiting time
    double avg_waiting = 0.0;

    //This is the event simulation
    while (!timeLine.empty()) {
        Passenger current = timeLine.top();
        timeLine.pop();

        //Incrementing the time
        time = current.current_time;
        //Passenger is arrived
        if (current.currentSit == 0) {
            //There is no available luggage, pushing to luggage queue
            if (whichLCEmpty(lcs, L) == -1) {
                luggageq.push(current);
                //Updating the length
                if (luggageq.size() > maxLugq)  //max length 20 yi aşmamalı
                    maxLugq = luggageq.size();
            } else {
                //Finding the first empty luggage
                int x = whichLCEmpty(lcs, L);
                lcs[x].is_busy = true;
                current.where = x;
                //Processing the order
                current.current_time = current.current_time + current.luggage_time;
                current.currentSit = 1;
                //Adding to time que to process security que
                timeLine.push(current);
            }
        }
		//Passenger is done at the luggage
        else if (current.currentSit == 1) {
            //There's a line for luggages, popping from the line and sending it to luggage who is current leaving
            if (!luggageq.empty()) {
                int x = current.where;
                Passenger fromLine = luggageq.top();
                luggageq.pop();
                fromLine.where = x;
                fromLine.currentSit = 1;
                fromLine.current_time = time + fromLine.luggage_time;
                timeLine.push(fromLine);
            }
            //freeing a luggage
            else {
                int x = current.where;
                lcs[x].is_busy = false;
            }
            //if there's no empty security, pushing it to security queue
            if (whichSCEmpty(scs, S) == -1) {
                current.where = -1;
                securityq.push(current);
                //updating length
                if (securityq.size() > maxSecq)
                    maxSecq = securityq.size();
            } else {
                //finding the first empty security
                int x = whichSCEmpty(scs, S);
                scs[x].is_busy = true;
                current.currentSit = 2;
                current.where = x;
                //updating time and sending it to time queue
                current.current_time += current.security_time;
                timeLine.push(current);
            }
        }
		//customer is leaving security
        else if (current.currentSit == 2) {
            //there's a line for security popping from the line and sending it to current's security
            if (!securityq.empty()) {
                int x = current.where;
                Passenger fromLine = securityq.top();
                securityq.pop();
                fromLine.where = x;
                fromLine.currentSit = 2;
                fromLine.current_time = time + fromLine.security_time;
                timeLine.push(fromLine);
            }
            //freeing the security if there's no line
            else {
                int x = current.where;
                scs[x].is_busy = false;
            }
            current.where = -1;
            current.waiting_time = time - current.arrival_time;
            avg_waiting = avg_waiting + current.waiting_time;
            //Calculations for missed flight
            if (current.flight_time < (current.arrival_time + current.waiting_time)) {
                w++;
            }
        }
    }
    //Opens output file and determines precision then writes the statistics
    ofstream myFile;
    myFile.open(argv[2]);
    myFile << fixed << setprecision(4);
    avg_waiting = avg_waiting / P;
    myFile << avg_waiting << " " << w << endl;

    // SECOND CASE

    //Luggage counters array
    LuggageCounter lcs2[L];
    //Security counters array
    SecurityCounter scs2[S];
    //Time priority queue for events
    priority_queue <Passenger, vector<Passenger>, CompareToTime> timeLine2;
    //Flight priority queue for luggage
    priority_queue <Passenger, vector<Passenger>, CompareToFlight> luggageq2;
    //Flight priority queue for security
    priority_queue <Passenger, vector<Passenger>, CompareToFlight> securityq2;

    //Adding the first customers to the time que again
    for (int i = 0; i < P; i++) {
        timeLine2.push(passengersFirst[i]);
        passengersFirst[i].current_time = passengersFirst[i].arrival_time;
        passengersFirst[i].waiting_time = 0;
        passengersFirst[i].currentSit = 0;
    }
    //Initializing
    double time2 = 0;
    w = 0;
    avg_waiting = 0;
    maxLugq = 20;
    maxSecq = 20;

    //This is the event simulation 2
    while (!timeLine2.empty()) {
        Passenger current = timeLine2.top();
        timeLine2.pop();
        //incrementing time
        time2 = current.current_time;
        //passenger is arrived
        if (current.currentSit == 0) {
            //there is no available luggage, pushing to luggage queue
            if (whichLCEmpty(lcs2, L) == -1) {
                luggageq2.push(current);
                //updating the length
                if (luggageq2.size() > maxLugq) 
                    maxLugq = luggageq2.size();
            } else {
                //finding the first empty luggage
                int x = whichLCEmpty(lcs2, L);
                lcs2[x].is_busy = true;
                current.where = x;
                //processing the order
                current.current_time = current.current_time + current.luggage_time;
                current.currentSit = 1;
                //adding to time que
                timeLine2.push(current);
            }
        }
		//passenger is finished at the luggage
        else if (current.currentSit == 1) {
            //there's a line for luggages, popping from the line and sending it to luggage who is current leaving
            if (!luggageq2.empty()) {
                int x = current.where;
                Passenger fromLine = luggageq2.top();
                luggageq2.pop();
                fromLine.where = x;
                fromLine.currentSit = 1;
                fromLine.current_time = time2 + fromLine.luggage_time;
                timeLine2.push(fromLine);
            }
            //freeing the luggage
            else {
                int x = current.where;
                lcs2[x].is_busy = false;
            }
            //if there's no empty security, pushing it to security queue
            if (whichSCEmpty(scs2, S) == -1) {
                current.where = -1;
                securityq2.push(current);
                //updating length
                if (securityq2.size() > maxSecq)
                    maxSecq = securityq2.size();
            } else {
                //finding the first empty security
                int x = whichSCEmpty(scs2, S);
                scs2[x].is_busy = true;
                current.currentSit = 2;
                current.where = x;
                //updating time and sending it to time queue
                current.current_time += current.security_time;
                timeLine2.push(current);
            }
        }
		//customer is leaving security
        else if (current.currentSit == 2) {
            //there's a line for security popping from the line and sending it to current's security
            if (!securityq2.empty()) {
                int x = current.where;
                Passenger fromLine = securityq2.top();
                securityq2.pop();
                fromLine.where = x;
                fromLine.currentSit = 2;
                fromLine.current_time = time2 + fromLine.security_time;
                timeLine2.push(fromLine);
            }
            //freeing the security if there's no line
            else {
                int x = current.where;
                scs2[x].is_busy = false;
            }
            current.where = -1;
            current.waiting_time = time2 - current.arrival_time;
            avg_waiting = avg_waiting + current.waiting_time;
            //Calculations for missed flight
            if (current.flight_time < (current.arrival_time + current.waiting_time)) {
                w++;
            }
        }
    }
    avg_waiting = avg_waiting / P;
    myFile << avg_waiting << " " << w << endl;


	// THIRD CASE
	//Luggage array
    LuggageCounter lcs3[L];
	//Security array
    SecurityCounter scs3[S];
	//que for events
    priority_queue <Passenger, vector<Passenger>, CompareToTime> timeLine3;
	//time priority que for luggage
    priority_queue <Passenger, vector<Passenger>, CompareToTime> luggageq3;
	//time priority que for security
    priority_queue <Passenger, vector<Passenger>, CompareToTime> securityq3;

	//adding the first customers to the time que again
    for (int i = 0; i < P; i++) {
        timeLine3.push(passengersFirst[i]);
        passengersFirst[i].current_time = passengersFirst[i].arrival_time;
        passengersFirst[i].waiting_time = 0;
        passengersFirst[i].currentSit = 0;
    }

    double time3 = 0;
    w = 0;
    avg_waiting = 0;
    maxLugq = 20;
    maxSecq = 20;

	//this is the event simulation 3
    while (!timeLine3.empty()) {
        Passenger current = timeLine3.top();
        timeLine3.pop();
        //incrementing time
        time3 = current.current_time;
        //passenger is arrived
        if (current.currentSit == 0) {
            //there is no available luggage, pushing to luggage queue
            if (whichLCEmpty(lcs3, L) == -1) {
                luggageq3.push(current);
                //updating the length
                if (luggageq3.size() > maxLugq)
                    maxLugq = luggageq3.size();
            } else {
                //finding the first empty luggage
                int x = whichLCEmpty(lcs3, L);
                lcs3[x].is_busy = true;
                current.where = x;
                //processing the order
                current.current_time = current.current_time + current.luggage_time;
                current.currentSit = 1;
                //adding to time que
                timeLine3.push(current);
            }
        }
		//passenger is finished at the luggage
        else if (current.currentSit == 1) {
            //there's a line for luggages, popping from the line and sending it to luggage who is current leaving
            if (!luggageq3.empty()) {
                int x = current.where;
                Passenger fromLine = luggageq3.top();
                luggageq3.pop();
                fromLine.where = x;
                fromLine.currentSit = 1;
                fromLine.current_time = time3 + fromLine.luggage_time;
                timeLine3.push(fromLine);
            }
            //freeing the luggage
            else {
                int x = current.where;
                lcs3[x].is_busy = false;
            }
            //if there's no empty security, pushing it to security queue
            if (whichSCEmpty(scs3, S) == -1 && current.isVIP != 1) {
                current.where = -1;
                securityq3.push(current);
                //updating length
                if (securityq3.size() > maxSecq)
                    maxSecq = securityq3.size();
            } else if (current.isVIP != 1) {
                //finding the first empty security
                int x = whichSCEmpty(scs3, S);
                scs3[x].is_busy = true;
                current.currentSit = 2;
                current.where = x;
                //updating time and sending it to time queue
                current.current_time += current.security_time;
                timeLine3.push(current);
            } else {
                current.currentSit = 3; //Exit
                current.waiting_time = time3 - current.arrival_time;
                avg_waiting = avg_waiting + current.waiting_time;
                if (current.flight_time < (current.arrival_time + current.waiting_time)) {
                    w++;
                }
            }
        }
		//customer is leaving security
        else if (current.currentSit == 2) {
            //there's a line for security popping from the line and sending it to current's security
            if (!securityq3.empty()) {
                int x = current.where;
                Passenger fromLine = securityq3.top();
                securityq3.pop();
                fromLine.where = x;
                fromLine.currentSit = 2;
                fromLine.current_time = time3 + fromLine.security_time;
                timeLine3.push(fromLine);
            }
            //freeing the security if there's no line
            else{
                int x = current.where;
                scs3[x].is_busy = false;
            }
            current.where = -1;
            current.waiting_time = time3 - current.arrival_time;
            avg_waiting = avg_waiting + current.waiting_time;
            if (current.flight_time < (current.arrival_time + current.waiting_time)) {
                w++;
            }
        }
    }
    avg_waiting = avg_waiting / P;
    myFile << avg_waiting << " " << w << endl;

    // FOURTH CASE

	//Luggage array
    LuggageCounter lcs4[L];
	//Security array
    SecurityCounter scs4[S];
	//que for events
    priority_queue <Passenger, vector<Passenger>, CompareToTime> timeLine4;
	//flight priority que for luggage
    priority_queue <Passenger, vector<Passenger>, CompareToFlight> luggageq4;
	//flight priority que for security
    priority_queue <Passenger, vector<Passenger>, CompareToFlight> securityq4;
	//adding the first customers to the time que again
    for (int i = 0; i < P; i++) {
        timeLine4.push(passengersFirst[i]);
        passengersFirst[i].current_time = passengersFirst[i].arrival_time;
        passengersFirst[i].waiting_time = 0;
        passengersFirst[i].currentSit = 0;
    }

    double time4 = 0;
    w = 0;
    avg_waiting = 0;
    maxLugq = 20;
    maxSecq = 20;

	//this is the event simulation 4
    while (!timeLine4.empty()) {
        Passenger current = timeLine4.top();
        timeLine4.pop();
        //incrementing time
        time4 = current.current_time;
        //passenger is arrived
        if (current.currentSit == 0) {
            //there is no available luggage, pushing to luggage queue
            if (whichLCEmpty(lcs4, L) == -1) {
                luggageq4.push(current);
                //updating the length
                if (luggageq4.size() > maxLugq)
                    maxLugq = luggageq4.size();
            } else {
                //finding the first empty luggage
                int x = whichLCEmpty(lcs4, L);
                lcs4[x].is_busy = true;
                current.where = x;
                //processing the order
                current.current_time = current.current_time + current.luggage_time;
                current.currentSit = 1;
                //adding to time que
                timeLine4.push(current);
            }
        }
		//passenger is finished at the luggage
        else if (current.currentSit == 1) {
            //there's a line for luggages, popping from the line and sending it to luggage who is current leaving
            if (!luggageq4.empty()) {
                int x = current.where;
                Passenger fromLine = luggageq4.top();
                luggageq4.pop();
                fromLine.where = x;
                fromLine.currentSit = 1;
                fromLine.current_time = time4 + fromLine.luggage_time;
                timeLine4.push(fromLine);
            }
            //freeing the luggage
            else {
                int x = current.where;
                lcs4[x].is_busy = false;
            }
            //if there's no empty security, pushing it to security queue
            if (whichSCEmpty(scs4, S) == -1 && current.isVIP != 1) {
                current.where = -1;
                securityq4.push(current);
                //updating length
                if (securityq4.size() > maxSecq)
                    maxSecq = securityq4.size();
            } else if (current.isVIP != 1) {
                //finding the first empty security
                int x = whichSCEmpty(scs4, S);
                scs4[x].is_busy = true;
                current.currentSit = 2;
                current.where = x;
                //updating time and sending it to time queue
                current.current_time += current.security_time;
                timeLine4.push(current);
            } else {
                current.currentSit = 3; //Exit
                current.waiting_time = time4 - current.arrival_time;
                avg_waiting = avg_waiting + current.waiting_time;
                if (current.flight_time < (current.arrival_time + current.waiting_time)) {
                    w++;
                }
            }
        }
		//customer is leaving security
        else if (current.currentSit == 2) {
            //there's a line for security popping from the line and sending it to current's security
            if (!securityq4.empty()) {
                int x = current.where;
                Passenger fromLine = securityq4.top();
                securityq4.pop();
                fromLine.where = x;
                fromLine.currentSit = 2;
                fromLine.current_time = time4 + fromLine.security_time;
                timeLine4.push(fromLine);
            }
            //freeing the security if there's no line
            else{
                int x = current.where;
                scs4[x].is_busy = false;
            }
            //adding customer to the array of finished customers
            current.where = -1;
            current.waiting_time = time4 - current.arrival_time;
            avg_waiting = avg_waiting + current.waiting_time;
            if (current.flight_time < (current.arrival_time + current.waiting_time)) {
                w++;
            }
        }
    }
    avg_waiting = avg_waiting / P;
    myFile << avg_waiting << " " << w << endl;

	// FIFTH CASE

	//Luggage array
    LuggageCounter lcs5[L];
	//Security array
    SecurityCounter scs5[S];
	//que for events
    priority_queue<Passenger,vector<Passenger>,CompareToTime> timeLine5;
	//time priority que for luggage
    priority_queue<Passenger,vector<Passenger>,CompareToTime> luggageq5;	
	//time priority que for security
    priority_queue<Passenger,vector<Passenger>,CompareToTime> securityq5;

	//adding the first customers to the time que again
    for(int i=0;i<P;i++){
        timeLine5.push(passengersFirst[i]);
        passengersFirst[i].current_time = passengersFirst[i].arrival_time;
        passengersFirst[i].waiting_time = 0;
        passengersFirst[i].currentSit = 0;
    }

    double time5=0;
    w=0;
    avg_waiting = 0;
    maxLugq = 20;
    maxSecq = 20;

	//this is the event simulation 5
    while(!timeLine5.empty()){
        Passenger current = timeLine5.top();
        timeLine5.pop();
        //incrementing time
        time5 = current.current_time;
        //passenger is arrived
        if(current.isLuggageFree == 1 && current.currentSit == 0){
            current.currentSit = 1;
        }
        if(current.currentSit==0 && current.isLuggageFree != 1){
            //there is no available luggage, pushing to luggage queue
            if(whichLCEmpty(lcs5,L)==-1){
                luggageq5.push(current);
                //updating the length
                if(luggageq5.size() > maxLugq)
                    maxLugq = luggageq5.size();
            }
            else{
                //finding the first empty luggage
                int x = whichLCEmpty(lcs5,L);
                lcs5[x].is_busy = true;
                current.where = x;
                //processing the order
                current.current_time = current.current_time + current.luggage_time;
                current.currentSit = 1;
                //adding to time que
                timeLine5.push(current);
            }
        }
        //passenger is finished at the luggage
        else if(current.currentSit == 1 ){
            //there's a line for luggages, popping from the line and sending it to luggage who is current leaving
            if(!luggageq5.empty() && current.isLuggageFree != 1){
                int x=current.where;
                Passenger fromLine = luggageq5.top();
                luggageq5.pop();
                fromLine.where = x;
                fromLine.currentSit = 1;
                fromLine.current_time = time5 + fromLine.luggage_time;
                timeLine5.push(fromLine);
            }
           //freeing the luggage
            else if (current.isLuggageFree != 1){
                int x = current.where;
                lcs5[x].is_busy = false;
            }
            //if there's no empty security, pushing it to security queue
            if(whichSCEmpty(scs5,S) == -1){
                current.where = -1;
                securityq5.push(current);
                //updating length
                if(securityq5.size() > maxSecq)
                    maxSecq=securityq5.size();
            }
            else{
                //finding the first empty security
                int x=whichSCEmpty(scs5,S);
                scs5[x].is_busy=true;
                current.currentSit = 2; 
                current.where=x;
                //updating time and sending it to time queue
                current.current_time += current.security_time;
                timeLine5.push(current);
            }
        }
        //customer is leaving security
        else if(current.currentSit == 2){
            //there's a line for security popping from the line and sending it to current's security
            if(!securityq5.empty()) {
                int x = current.where;
                Passenger fromLine = securityq5.top();
                securityq5.pop();
                fromLine.where = x;
                fromLine.currentSit = 2;
                fromLine.current_time = time5 + fromLine.security_time;
                timeLine5.push(fromLine);
            }
            //freeing the security if there's no line
            else{
                int x = current.where;
                scs5[x].is_busy=false;
                current.currentSit = 2;
            }
            //adding customer to the array of finished customers
            current.where = -1;
            current.waiting_time = time5-current.arrival_time;
            avg_waiting = avg_waiting + current.waiting_time;
            if(current.flight_time < (current.arrival_time + current.waiting_time)){
                w++;
            }
        }
    }
    avg_waiting = avg_waiting / P;
    myFile << avg_waiting << " " << w << endl;

    // SIXTH CASE

	//Luggage array
    LuggageCounter lcs6[L];
	//Security array
    SecurityCounter scs6[S];
	//que for events
    priority_queue<Passenger,vector<Passenger>,CompareToTime> timeLine6;
	//flight priority que for luggage
    priority_queue<Passenger,vector<Passenger>,CompareToFlight> luggageq6;
	//flight priority que for security
    priority_queue<Passenger,vector<Passenger>,CompareToFlight> securityq6;

	//adding the first customers to the time que again
    for(int i=0;i<P;i++){
        timeLine6.push(passengersFirst[i]);
        passengersFirst[i].current_time = passengersFirst[i].arrival_time;
        passengersFirst[i].waiting_time = 0;
        passengersFirst[i].currentSit = 0;
    }
    double time6=0;
    w=0;
    avg_waiting = 0;
    maxLugq = 20;
    maxSecq = 20;

	//this is the event simulation 6
    while(!timeLine6.empty()){
        Passenger current = timeLine6.top();
        timeLine6.pop();
        //incrementing time
        time6 = current.current_time;
        //passenger is arrived
        if(current.isLuggageFree == 1 && current.currentSit == 0){
            current.currentSit = 1;
        }
        if(current.currentSit==0 && current.isLuggageFree != 1){
            //there is no available luggage, pushing to luggage queue
            if(whichLCEmpty(lcs6,L)==-1){
                luggageq6.push(current);
                //updating the length
                if(luggageq6.size() > maxLugq)
                    maxLugq = luggageq6.size();
            }
            else{
                //finding the first empty luggage
                int x = whichLCEmpty(lcs6,L);
                lcs6[x].is_busy = true;
                current.where = x;
                //processing the order
                current.current_time = current.current_time + current.luggage_time;
                current.currentSit = 1;
                //adding to time que
                timeLine6.push(current);
            }
        }
        //passenger is finished at the luggage
        else if(current.currentSit == 1 ){
            //there's a line for luggages, popping from the line and sending it to luggage who is current leaving
            if(!luggageq6.empty() && current.isLuggageFree != 1){
                int x=current.where;
                Passenger fromLine = luggageq6.top();
                luggageq6.pop();
                fromLine.where = x;
                fromLine.currentSit = 1;
                fromLine.current_time = time6 + fromLine.luggage_time;
                timeLine6.push(fromLine);
            }
            //freeing the luggage
            else if (current.isLuggageFree != 1){
                int x = current.where;
                lcs6[x].is_busy = false;
            }
            //if there's no empty security, pushing it to security queue
                current.where = -1;
                securityq6.push(current);
                //updating length
                if(securityq6.size() > maxSecq)
                    maxSecq=securityq6.size();
            }
            else{
                //finding the first empty security
                int x=whichSCEmpty(scs6,S);
                scs6[x].is_busy=true;
                current.currentSit = 2;
                current.where=x;
                //updating time and sending it to time queue
                current.current_time += current.security_time;
                timeLine6.push(current);
            }
        }
        //customer is leaving security
        else if(current.currentSit == 2){
            //there's a line for security popping from the line and sending it to current's security
            if(!securityq6.empty()) {
                int x = current.where;
                Passenger fromLine = securityq6.top();
                securityq6.pop();
                fromLine.where = x;
                fromLine.currentSit = 2;
                fromLine.current_time = time6 + fromLine.security_time;
                timeLine6.push(fromLine);
            }
                //freeing the security if there's no line
            else{
                int x = current.where;
                scs6[x].is_busy=false;
                current.currentSit = 2;
            }
            //adding customer to the array of finished customers
            current.where = -1;
            current.waiting_time = time6-current.arrival_time;
            avg_waiting = avg_waiting + current.waiting_time;
            if(current.flight_time < (current.arrival_time + current.waiting_time)){
                w++;
            }
        }
    }
    avg_waiting = avg_waiting / P;
    myFile << avg_waiting << " " << w << endl;

    // SEVENTH CASE

	//Luggage array
    LuggageCounter lcs7[L];
	//Security array
    SecurityCounter scs7[S];
	//que for events
    priority_queue <Passenger, vector<Passenger>, CompareToTime> timeLine7;
	//time priority que for luggage
    priority_queue <Passenger, vector<Passenger>, CompareToTime> luggageq7;
	//time priority que for security
    priority_queue <Passenger, vector<Passenger>, CompareToTime> securityq7;
	//adding the first customers to the time que again
    for (int i = 0; i < P; i++) {
        timeLine7.push(passengersFirst[i]);
        passengersFirst[i].current_time = passengersFirst[i].arrival_time;
        passengersFirst[i].waiting_time = 0;
        passengersFirst[i].currentSit = 0;
    }
    double time7 = 0;
    w = 0;
    avg_waiting = 0;
    maxLugq = 20;
    maxSecq = 20;

	//this is the event simulation 7
    while (!timeLine7.empty()) {
        Passenger current = timeLine7.top();
        timeLine7.pop();
        //incrementing time
        time7 = current.current_time;
        if(current.isLuggageFree == 1 && current.currentSit == 0){
            current.currentSit = 1;
        }
        //passenger is arrived
        if (current.currentSit == 0 && current.isLuggageFree != 1) {
            //there is no available luggage, pushing to luggage queue
            if (whichLCEmpty(lcs7, L) == -1) {
                luggageq7.push(current);
                //updating the length
                if (luggageq7.size() > maxLugq)
                    maxLugq = luggageq7.size();
            } else {
                //finding the first empty luggage
                int x = whichLCEmpty(lcs7, L);
                lcs7[x].is_busy = true;
                current.where = x;
                //processing the order
                current.current_time = current.current_time + current.luggage_time;
                current.currentSit = 1;
                //adding to time que
                timeLine7.push(current);
            }
        }
        //passenger is finished at the luggage
        else if (current.currentSit == 1) {
            //there's a line for luggages, popping from the line and sending it to luggage who is current leaving
            if (!luggageq7.empty()  && current.isLuggageFree != 1) {
                int x = current.where;
                Passenger fromLine = luggageq7.top();
                luggageq7.pop();
                fromLine.where = x;
                fromLine.currentSit = 1;
                fromLine.current_time = time7 + fromLine.luggage_time;
                timeLine7.push(fromLine);
            }
            //freeing the luggage
            else if(current.isLuggageFree != 1){
                int x = current.where;
                lcs7[x].is_busy = false;
            }
            //if there's no empty security, pushing it to security queue
            if (whichSCEmpty(scs7, S) == -1 && current.isVIP != 1) {
                current.where = -1;
                securityq7.push(current);
                //updating length
                if (securityq7.size() > maxSecq)
                    maxSecq = securityq7.size();
            } else if (current.isVIP != 1) {
                //finding the first empty security
                int x = whichSCEmpty(scs7, S);
                scs7[x].is_busy = true;
                current.currentSit = 2;
                current.where = x;
                //updating time and sending it to time queue
                current.current_time += current.security_time;
                timeLine7.push(current);
            } else {
                current.currentSit = 3; //Exit
                current.waiting_time = time7 - current.arrival_time;
                avg_waiting = avg_waiting + current.waiting_time;
                if (current.flight_time < (current.arrival_time + current.waiting_time)) {
                    w++;
                }
            }
        }
        //customer is leaving security
        else if (current.currentSit == 2) {
            //there's a line for security popping from the line and sending it to current's security

            if (!securityq7.empty()) {
                int x = current.where;
                Passenger fromLine = securityq7.top();
                securityq7.pop();
                fromLine.where = x;
                fromLine.currentSit = 2;
                fromLine.current_time = time7 + fromLine.security_time;
                timeLine7.push(fromLine);
            }
            //freeing the security if there's no line
            else{
                int x = current.where;
                scs7[x].is_busy = false;
            }
            //adding customer to the array of finished customers
            current.where = -1;
            current.waiting_time = time7 - current.arrival_time;
            avg_waiting = avg_waiting + current.waiting_time;
            if (current.flight_time < (current.arrival_time + current.waiting_time)) {
                w++;
            }
        }
    }
    avg_waiting = avg_waiting / P;
    myFile << avg_waiting << " " << w << endl;

    // EIGHT CASE

	//Luggage array
    LuggageCounter lcs8[L];
	//Security array
    SecurityCounter scs8[S];
    //que for events
    priority_queue <Passenger, vector<Passenger>, CompareToTime> timeLine8;
    //flight priority que for luggage
    priority_queue <Passenger, vector<Passenger>, CompareToFlight> luggageq8;
	//flight priority que for security
    priority_queue <Passenger, vector<Passenger>, CompareToFlight> securityq8;

    //adding the first customers to the time que again
    for (int i = 0; i < P; i++) {
        timeLine8.push(passengersFirst[i]);
        passengersFirst[i].current_time = passengersFirst[i].arrival_time;
        passengersFirst[i].waiting_time = 0;
        passengersFirst[i].currentSit = 0;
    }

    double time8 = 0;
    w = 0;
    avg_waiting = 0;
    maxLugq = 20;
    maxSecq = 20;

    //this is the event simulation 8
    while (!timeLine8.empty()) {
        Passenger current = timeLine8.top();
        timeLine8.pop();
        //incrementing time
        time8 = current.current_time;
        if(current.isLuggageFree == 1 && current.currentSit == 0){
            current.currentSit = 1;
        }
        //passenger is arrived
        if (current.currentSit == 0 && current.isLuggageFree != 1) {
            //there is no available luggage, pushing to luggage queue
            if (whichLCEmpty(lcs8, L) == -1) {
                luggageq8.push(current);
                //updating the length
                if (luggageq8.size() > maxLugq)
                    maxLugq = luggageq8.size();
            } else {
                //finding the first empty luggage
                int x = whichLCEmpty(lcs8, L);
                lcs8[x].is_busy = true;
                current.where = x;
                //processing the order
                current.current_time = current.current_time + current.luggage_time;
                current.currentSit = 1;
                //adding to time que
                timeLine8.push(current);
            }
        }
        //passenger is finished at the luggage
        else if (current.currentSit == 1) {
            //there's a line for luggages, popping from the line and sending it to luggage who is current leaving
            if (!luggageq8.empty()  && current.isLuggageFree != 1) {
                int x = current.where;
                Passenger fromLine = luggageq8.top();
                luggageq8.pop();
                fromLine.where = x;
                fromLine.currentSit = 1;
                fromLine.current_time = time8 + fromLine.luggage_time;
                timeLine8.push(fromLine);
            }
            //freeing the luggage
            else if(current.isLuggageFree != 1){
                int x = current.where;
                lcs8[x].is_busy = false;
            }
            //if there's no empty security, pushing it to security queue
            if (whichSCEmpty(scs8, S) == -1 && current.isVIP != 1) {
                current.where = -1;
                securityq8.push(current);
                //updating length
                if (securityq8.size() > maxSecq)
                    maxSecq = securityq8.size();
            } else if (current.isVIP != 1) {
                //finding the first empty security
                int x = whichSCEmpty(scs8, S);
                scs8[x].is_busy = true;
                current.currentSit = 2;
                current.where = x;
                //updating time and sending it to time queue
                current.current_time += current.security_time;
                timeLine8.push(current);
            } else {
                current.currentSit = 3; //Exit
                current.waiting_time = time8 - current.arrival_time;
                avg_waiting = avg_waiting + current.waiting_time;
                if (current.flight_time < (current.arrival_time + current.waiting_time)) {
                    w++;
                }
            }
        }
        //customer is leaving security
        else if (current.currentSit == 2) {
            //there's a line for security popping from the line and sending it to current's security
            if (!securityq8.empty()) {
                int x = current.where;
                Passenger fromLine = securityq8.top();
                securityq8.pop();
                fromLine.where = x;
                fromLine.currentSit = 2;
                fromLine.current_time = time8 + fromLine.security_time;
                timeLine8.push(fromLine);
            }
            //freeing the security if there's no line
            else{
                int x = current.where;
                scs8[x].is_busy = false;
            }
            //adding customer to the array of finished customers
            current.where = -1;
            current.waiting_time = time8 - current.arrival_time;
            avg_waiting = avg_waiting + current.waiting_time;
            if (current.flight_time < (current.arrival_time + current.waiting_time)) {
                w++;
            }
        }
    }
    avg_waiting = avg_waiting / P;
    myFile << avg_waiting << " " << w << endl;
    myFile.close();
}