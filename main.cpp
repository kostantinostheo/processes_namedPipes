#include <iostream>
#include "executive_worker_header.h"
#include "pipes_header.h"
#include "structures_header.h"


using namespace std;



int numWorkers;
int bufferSize;
string input_dir;

bool break_flag = true;

int countArguments(string input)
{
  int counter = 0;
  string word;
  stringstream ss_tmp(input);

  while(ss_tmp >> word)
    counter++;

  return counter;

}


void sig_h(int signo)
{

    cout << " ctrl+c" << endl;
    if(signo == SIGINT)
      cout << signo << endl;
       // break_flag = false;
}

int main(int argc, char *argv[]) 
{
    

    for(int i=0; i<argc; i++)
    {
        if(argc>2 && string(argv[i]) == "-w")
          numWorkers = atoi(argv[i+1]);
        
        else if(argc>2 && string(argv[i]) == "-i")
          input_dir = argv[i+1];

        else if(argc>2 && string(argv[i]) == "-b")
          bufferSize = atoi(argv[i+1]);

        //input error checking.
        if (argc < 7)
        {
            perror("Error: Too few arguments");
            exit(1);
        }
        if (argc > 7)
        {
            perror("Error: Too many arguments");
            exit(1);
        }
        
    }
    if(numWorkers == 1)
    {
      cout << "Are you sure you wanna do that? Only one worker..?" << endl;
      sleep(1);
    }
      
  
  
  pipes_creation(numWorkers);
  //perror("ERROR");
  
  pid_t * proc_id;
  proc_id = worker_creation(numWorkers); //forks
  
  int * writersArr = pipes_open_writers(numWorkers);
  int * readerArr = pipes_open_readers(numWorkers);
  
  
  load_dir("dir", numWorkers, readerArr, writersArr);

  


  stringstream cmd; 
  string command;
  while (break_flag)
  {
    //signal(SIGINT , sig_h);
    if(command == "/exit")
      break;
    else if(command == "/numPatientAdmissions")
    {
      getline(cin, command);
      cmd = stringstream(command);
      int n = countArguments(cmd.str());
      
      //count kena. ara kai lekseis sto stream.
      string disease;
      string date_bot;
      string date_top;
      string country;
      if(n == 3)
      {
        cmd >> disease >> date_bot >> date_top;
        a_admissions(readerArr, writersArr, numWorkers, disease, date_bot, date_top);
      }
      else if(n == 4)
      {
        cmd >> disease >> date_bot >> date_top >> country;
        a_admissions(readerArr, writersArr, numWorkers, disease, date_bot, date_top, country);
      }
        
      else
        cout << "error" << endl;
    }
    else if(command == "/numPatientDischarges")
    {
      getline(cin, command);
      cmd = stringstream(command);
      int n = countArguments(cmd.str());
      
      //count kena. ara kai lekseis sto stream.
      string disease;
      string date_bot;
      string date_top;
      string country;
      if(n == 3)
      {
        cmd >> disease >> date_bot >> date_top;
        d_discharges(readerArr, writersArr, numWorkers, disease, date_bot, date_top);
      }
      else if(n == 4)
      {
        cmd >> disease >> date_bot >> date_top >> country;
        d_discharges(readerArr, writersArr, numWorkers, disease, date_bot, date_top, country);
      }
        
      else
        cout << "error" << endl;
    }

    else if(command == "/diseaseFrequency")
    {
      getline(cin, command);
      cmd = stringstream(command);
      int n = countArguments(cmd.str());
      
      //count kena. ara kai lekseis sto stream.
      string disease;
      string date_bot;
      string date_top;
      string country;
      if(n == 3)
      {
        cmd >> disease >> date_bot >> date_top;
        f_frequency(readerArr, writersArr, numWorkers, disease, date_bot, date_top);
      }
      else if(n == 4)
      {
        cmd >> disease >> date_bot >> date_top >> country;
        f_frequency(readerArr, writersArr, numWorkers, disease, date_bot, date_top, country);
      }
        
      else
        cout << "error" << endl;
    }

    else if(command == "/searchPatientRecord")
    {
      getline(cin, command);
      cmd = stringstream(command);
      int n = countArguments(cmd.str());

      string recordID_tmp;

      if(n == 1)
      {
        cmd >> recordID_tmp;
        s_patient_record(readerArr , writersArr , numWorkers, recordID_tmp);
      }
        
      else if(n > 1)
        cout << "Too many arguments" << endl;
      else
        cout << "Too few arguments" << endl;  

    }
    else if(command == "/listCountries")
    {
      getline(cin, command);
      cmd = stringstream(command);
      int n = countArguments(cmd.str());

      int recordID_tmp;

      if(n == 0)
        listCountries(readerArr, writersArr, numWorkers);
      else if(n > 0)
        cout << "Too many arguments" << endl;
      else
        cout << "Too few arguments" << endl;
    }
    else if(command == "/topk-AgeRanges")
    {
      getline(cin, command);
      cmd = stringstream(command);
      int n = countArguments(cmd.str());
      
      if(n<5)
        cout << "error" << endl;
      else if(n > 5)
        cout << "error" << endl;

      //count kena. ara kai lekseis sto stream.
      int k;
      string country;
      string disease;
      string date_bot;
      string date_top;
      
      
        cmd >> k >> country >> disease >> date_bot >> date_top;
        k_top(readerArr , writersArr, numWorkers, k, country, disease, date_bot, date_top);
     

    }
    
      
    sleep(1);
    cout << "command: ";
    cin >> command;
    cmd.clear();
  }


  cout << "terminating...please wait" << endl;
  shutting_down(proc_id, numWorkers, readerArr , writersArr);
  
  
  

  
  return EXIT_SUCCESS;
}
