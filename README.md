# processes_namedPipes
The "diseaseAggregator" application creates child processes and communicates with them using named pipes.

Konstantinos Theofilis
University #ID: sdi1600287

NOTES
-----
1) Do not press CTRL+C because you may create a zombie apocalypse (zombie processes), or maybe not. Sometimes work sometimes not.
2) [BUG] SIGUSR1 signal does not work.
3) [BUG] Application never checks if for an EXIT there is an ENTER record.



diseaseAggregator Application
-----------------------------
Compilation:
1) 'make' (Makefile compilation) or 
2) 'g++ -o diseaseAggregator executive.cpp main.cpp pipes.cpp worker.cpp'

Execution:
./diseaseAggregator –w <numWorkers> -b <bufferSize> -i <input_dir> 
./diseaseAggregator –w 5 -b 50 -i input_dir 

We are getting the arguments at the command line and we check if they are correct. Then we create the pipes.
The executive creates two types o pipes (two for each worker conected to the executive). With mkfifo we create
"pipes/executive_W_worker_R" and "pipes/worker_W_executive_R" pipes. Executive can send (write) data to worker
throught first pipe and the opposite can be done throught the second one.
Then we create our workers. The number of workers is given as an argument at the command line. 

worker_working() --> Basic worker function. First we open the pipes between the execuive and the worker and then 
worker executes commands based on the protocol. I created different protocols, one for each command. 

Protocols
---------

protocol 'p' for "path". Executive sends p through pipe then the size of the path in bytes and then the path. 
            then worker executes directoryOpen() that orginize everything inside structures.
protocol 'l' for "list" and the execution of /ListCoutries command. Sending back to executive "how many countries 
        the worker has". For each country we send first the size of the string (country name) in bytes and then
        the string itself. 
protocol 's' for "search" and execution of /searchPatientRecord command. Worker reads the string size in bytes and
        then the string (command). Executing the searchPatientRecord() function. The result will be a string with 
        patients recordID name disease age and dates. We send to the executive the size of the string and then the 
        string itself.
protocol 'a' for "admissions" and execution of /numPatientAdmissions command. Worker reads the number of arguments 
        user gave. Then the string size (command) in bytes and then the string. Based on the number of argumetns the 
        result is different.
protocol 'd' for "discharges" and execution of /numPatientDischarges command. Same protocol as 'a'.
protocol 'f' for "frequency" and execution of /diseaseFrequency command.  Worker reads the number of arguments 
        user gave. Then the string size (command) in bytes and then the string. Based on the number of argumetns the 
        result is different. If arguments contain "country" then we execute FrequencySpecific() function else Frequency()
        function. We send back to the executive a string (number). First we send the size of the string and then the 
        string itself. If we want for more than one country we executite it as many times as the countries each worker 
        has. 
protocol 'k' for "topk.." it works but it has some bugs and sometime prints wrong result.

To terminate the program we use /exit command. While typing exit the application get a kill signal end terminates.
Shutting_down() function 1) kill all the worker processes, 2) closes all the pipes between executive and workers,
3) deallocate all the memory and 4) remove all the pipes. Before all these things happen the workers create logfiles 
with statistics. 

WHAT MY logfiles SHOW..!
------------------------
Each worker create his own logfile with the extension .XXXX in wich XXXX is the worker/process ID. The lofiles contain 
statistics about how many reads + writes succed (SUCCESS), failed (FAIL), and the total amount (TOTAL).
