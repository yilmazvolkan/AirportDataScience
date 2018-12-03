# AirportDataScience

## :tophat: Introduction

It is an airports simulation to observe passengers situation,their waiting time, and then increase the efficiency by reducing waiting time and number of missing flights. As a data scientist, we need to optimize our cost of spending time in the airport to increase the efficiency in the end. The passengers are really annoyed by the long waiting times for luggage hand-in and security checks.These are routine controls in the airports. Some of the passengers are even missing their fights because of the long waiting times at the queues. We come up with a few ideas to reduce these waiting times, but before proposing these ideas we decide to actually test our design 'in silico' if the new proposals will work or not on the data it is given.

In the current system there is a first come first serve queue before the L luggage hand-in counters. Whenever there is a free counter, first person in the queue goes to the free counter and gives their luggage. After handing in luggage, they go into a second queue for a security check. This is also a first come first serve queue and there are S security counters. Similarly, whenever a security counter is free, first person in the queue goes to that counter for the security check.
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
