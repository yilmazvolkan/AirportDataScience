# AirportDataScience

## :tophat: Introduction

It is an airport simulation to observe passengers,their waiting time, and then increase the efficiency by reducing waiting time and number of missing flights. As a data scientist, we need to optimize our cost of spending time in the airport to increase the efficiency in the end. The passengers are really annoyed by the long waiting times for luggage hand-in and security checks. These are routine controls in the airports. Some of the passengers are even missing their flights because of the long waiting times at the queues. We come up with a few ideas to reduce these waiting times, but before proposing these ideas we actually decide to test our design 'in silico' if the new proposals will work or not on the data it is given.

In the current system there is a first come first serve queue before the L luggage hand-in counters. Whenever there is a free counter, first person in the queue goes to the free counter and gives their luggage. After handing in luggage, they go into a second queue for a security check. This is also a first come first serve queue and there are S security counters. Similarly, whenever a security counter is free, first person in the queue goes to that counter for the security check.

<p align="center">
<a href = "https://github.com/yilmazvolkan/AirportDataScience"><img 
<img src="https://github.com/yilmazvolkan/AirportDataScience/blob/master/airport.png" width="700" height="250"></a>
</p>

We have 3 different ideas to make these queues more effcient:

* **First-to-Fly, First Serve**
Instead of making the queues first come, first serve we can make them first-to-fly, first serve. The person whose flight is earliest will move to the first place of the line. This applies to both luggage hand-in queue
and security queue. We are hoping that this would reduce the number of people who are late to their flights.


* **Our Airlines has a special VIP Passenger program**
Each passenger is either in this program or not. As a firm we are trusting these customers and therefore we decide that we can make these VIP passengers skip the security queue entirely. We are hoping that this would have an impact on other people's waiting time as well since VIP passengers won't keep the security counters busy.


* **Online ticketing system**
As some people do not have a luggage to hand-in, we can implement an online ticketing system. As a result the people who does not carry a luggage can skip the first queue entirely and go in line for the security check immediately. We are hoping that this would have an impact on other people's waiting time as well, since passengers without a luggage won't keep the luggage hand-in counters busy.

In summary we have 8 different scenarios:

<p align="center">
<a href = "https://github.com/yilmazvolkan/AirportDataScience"><img 
<img src="https://github.com/yilmazvolkan/AirportDataScience/blob/master/summary.png" width="450" height="400"></a>
</p>

## :flashlight: How to compile

In a terminal, call commands:
```
>cmake CMakeLists.txt

>make

OR

>cmake CMakeLists.txt && make

```
Make sure the executable is produced.

Then you can test the project with the command:
```
>./AirportDataScience inputFile.txt outputFile.txt
```

## LICENCE
All Solutions licensed under [The MIT Licence](https://github.com/yilmazvolkan/AirportDataScience/blob/master/LICENSE).

## COPYRIGHT
Copyright (c) 2018 [yilmazvolkan](https://github.com/yilmazvolkan).
