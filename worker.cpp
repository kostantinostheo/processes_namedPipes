#include <bits/c++config.h>

#include "executive_worker_header.h"
#include "structures_header.h"


using namespace std;
bool kill_flag = true;
bool interupt_flag = true;
Backbone structure;
Country_list * list = nullptr;
Date_list dlist;
//Listnode countrylist = nullptr;
int TOTAL;
int SUCCESS;
int FAIL;

void countLogfileStats(int help)
{
    TOTAL++;
    if(help < 0)
        FAIL++;
    else
        SUCCESS++;
}
int dateCompare(string date1 , string date2)//returns 1 if date1>date2.....else 0
{

    if(atoi(date1.c_str()+6) > atoi(date2.c_str()+6))
        return 1;
    else if(atoi(date1.c_str()+6) < atoi(date2.c_str()+6))
        return 0;

    if(atoi(date1.c_str()+3) > atoi(date2.c_str()+3))
        return 1;
    else if(atoi(date1.c_str()+3) < atoi(date2.c_str()+3))
        return 0;
    
    if(atoi(date1.c_str()) > atoi(date2.c_str()))
        return 1;
    else
        return 0;

}

bool dateBounds(string date_bot , string date_top , string search_date)
{
    if(dateCompare(search_date , date_bot) == 1 && dateCompare(search_date , date_top) == 0)
        return true;
    
    return false;
}



void sig_handler(int signo)
{
    //cout << "MINIMAKTAKI PANW PANW <3" <<endl;
    if (signo == SIGKILL)
    {
        kill_flag = false;
        //cout << kill_flag << endl;
    }
    else if(signo == SIGUSR1)
    {
        //nea arxeia bikan ara parto apo tin arxi.
    }
    else if(signo == SIGINT || signo == SIGQUIT)
    {
        interupt_flag = false;
        kill_flag =false;
        //cout << "signal: " << signo << endl;
                
    }
}

void write_text_to_log_file()
{   
    int N_countries = list->countCountries();
    stringstream ss;
    Country_list * tmp = list;
    
    ss << "log_file." << getpid();
    
    ofstream log_file(ss.str() , ios_base::out);
    for (int i = 0; i < N_countries; i++)
    {
        while(tmp != nullptr)
        {
            log_file << tmp->country << endl;
            tmp = tmp->next;
        }
        
    }
    log_file << "TOTAL " << TOTAL << endl;
    log_file << "SUCCESS " << SUCCESS << endl;
    log_file << "FAIL " << FAIL;

    log_file.close();
}




void load_file(string file_name , string directory_name)
{
    ifstream file (directory_name + "/" + file_name, ios_base::in);
    
    string recordID_temp; 
    int age_temp;
    string treatment_temp; //ENTER or EXIT
    string fname_temp;
    string lname_temp;
    string disease_temp;
    string country_tmp;

    country_tmp = directory_name.substr(directory_name.rfind('/')+1);
    //cout << country_tmp;

    dlist.insert(file_name);
    
    while(!file.eof())
    {
        file >> recordID_temp >> treatment_temp >> fname_temp >> lname_temp >> disease_temp >> age_temp;
        //cout << recordID_temp << "  " << disease_temp << endl;

        //Data structure insertion.
        structure.insert(recordID_temp, treatment_temp, fname_temp, lname_temp, file_name, disease_temp ,age_temp, country_tmp);
        
    }
    file.close();
}


void directoryOpen(string directory_name) //Pernei svarna to directory kai vlepei ena ena ta onomata twn arxeiwn h allwn fakelwn.
{
        struct dirent *directoryEntry; //file_name
        DIR *directoryFolder;

        
        directoryFolder = opendir (directory_name.c_str());
        if (directoryFolder == NULL) 
        {
            cerr << "Error: No such  file or directory" << endl;
            return;
        }

        string country_tmp = directory_name.substr(directory_name.rfind('/')+1);
        if(list == nullptr)
            list = new Country_list(country_tmp , nullptr);
        else
            list->insertCountryList(country_tmp, &list);
        
        // Process each entry.
        while ((directoryEntry = readdir(directoryFolder)) != NULL)
        {
            if(string(directoryEntry->d_name) != "." && string(directoryEntry->d_name) != "..")
            {
                //Call function to open file.
                load_file(directoryEntry->d_name , directory_name);
            }

        }

        // Close directory and exit.
        closedir (directoryFolder);
        
}

void worker_reads_path(int fd_r)
{
    char buff[512];
    int msg_size;
    int help;
    
    help = read(fd_r , buff , 4);
    countLogfileStats(help);

    memcpy(&msg_size, buff, sizeof(int));
    help = read(fd_r , buff , msg_size);
    countLogfileStats(help);
    
    directoryOpen(string(buff));
    
    sleep(getpid()%10);         // prepei na fygei !
    
    Country_list *date_temp = dlist.getHead();
    
    // gia kathe imerominia - arxeio
    while ( date_temp != nullptr ) {
        
        Country_list *country_temp = list;
        
        cout << date_temp->date << endl;
        
        // gia kathe xwra
        while (country_temp != nullptr) {
            
            // Gia kathe asthenia
            Backbone_node *desease_temp = structure.root;
        
            cout << country_temp->country << endl;
            
            int r020, r2140, r4160, r61;
            
            while(desease_temp != nullptr)
            {
                structure.statistics(date_temp->date, country_temp->country, desease_temp->disease, r020, r2140, r4160, r61);
                
                
                // Send statistics
                cout << endl << desease_temp->disease << endl;
                cout << "Age range 0-20 years: " << r020 << " cases" << endl;
                cout << "Age range 21-40 years: " << r2140 << " cases" << endl;
                cout << "Age range 41-60 years: " << r4160 << " cases" << endl;
                cout << "Age range 61+ years: " << r61 << " cases" << endl;
                
                desease_temp = desease_temp->next;
            }

            cout << endl;
            country_temp = country_temp->next;
        }

        date_temp = date_temp->next;
    }

}


void worker_reads_l(int fd_w)
{
    char buff[512];
    int msg_size; // string length + "string" !!
    int N; // how many countries??
    int help;

    N = (list == nullptr ? 0 : list->countCountries());

    help = write(fd_w , &N , sizeof(int));
    countLogfileStats(help);
    
    Country_list * tmp=list;
    for (int i = 0; i < N; i++)
    {
        stringstream ss;
        ss << tmp->country << " " << getpid();
        string tmp_s = ss.str();
        msg_size = tmp_s.length()+1;

        help = write(fd_w , &msg_size , sizeof(int));
        countLogfileStats(help);
        
        help = write(fd_w, tmp_s.c_str() , msg_size);
        countLogfileStats(help);
        
        tmp=tmp->next;
    }
    
}


void worker_reads_a(int fd_w, string buff , int argus)
{
    
    int N; // how many countries??
    stringstream ss (buff);
    string d, d1, d2, c;
    int h;
    
    if(argus == 3)
    {
        N = (list == nullptr ? 0 : list->countCountries());
        h = write(fd_w , &N , sizeof(int));
        countLogfileStats(h);
        
        ss >> d >> d1 >> d2;
        structure.Admissions(fd_w, list, d, d1, d2);
    }
    else
    {
        ss >> d >> d1 >> d2 >> c;
        N = (list == nullptr ? 0 : list->search(c));
        h = write(fd_w , &N , sizeof(int));
        countLogfileStats(h);
        if(N!=0)
            structure.AdmissionsSpecific(fd_w, d, d1, d2, c);
    }
}

void worker_reads_d(int fd_w, string buff , int argus)
{
    
    int N; // how many countries??
    stringstream ss (buff);
    string d, d1, d2, c;
    int h;
    
    if(argus == 3)
    {
        N = (list == nullptr ? 0 : list->countCountries());
        h = write(fd_w , &N , sizeof(int));
        countLogfileStats(h);
        ss >> d >> d1 >> d2;
        structure.Discharges(fd_w, list, d, d1, d2);
    }
    else
    {
        ss >> d >> d1 >> d2 >> c;
        N = (list == nullptr ? 0 : list->search(c));
        h = write(fd_w , &N , sizeof(int));
        countLogfileStats(h);
        if(N!=0)
            structure.DischargesSpecific(fd_w, d, d1, d2, c);
    }
}

void worker_reads_f(int fd_w, string buff , int argus)
{
    
    int N;
    stringstream ss (buff);
    string d, d1, d2, c;
    int h;
    
    if(argus == 3)
    {
        N = (list == nullptr ? 0 : list->countCountries());
        h = write(fd_w , &N , sizeof(int));
        countLogfileStats(h);
        ss >> d >> d1 >> d2;
        structure.Frequency(fd_w, list, d, d1, d2);
    }
    else
    {
        ss >> d >> d1 >> d2 >> c;
        N = (list == nullptr ? 0 : list->search(c));
        h = write(fd_w , &N , sizeof(int));
        countLogfileStats(h);
        if(N!=0)
            structure.FrequencySpecific(fd_w, d, d1, d2, c);
    }
    
}
void worker_reads_k(int fd_w, string buff , int k)
{
    
    int N;
    stringstream ss (buff);
    string d, d1, d2, c;
    int h;
    
        ss >> c >> d >> d1 >> d2;
        N = (list == nullptr ? 0 : list->search(c));
        h = write(fd_w , &N , sizeof(int));
        countLogfileStats(h);
        if(N!=0)
            structure.topkAgeRanges(fd_w, k, c, d, d1, d2);

    
}

void worker_working(int wId)
{

    int fd_w , fd_r;
    char BUFF[20];
    signal(SIGKILL, sig_handler);
    signal(SIGINT , sig_handler);
    //signal(SIGUSR1 , sig_handler);
    std::stringstream pipe_name;
    pipe_name << "pipes/executive_W_worker_R" << wId;
    fd_r = open(pipe_name.str().c_str() , O_RDONLY);
    

    pipe_name = stringstream();
    pipe_name << "pipes/worker_W_executive_R" << wId;
    fd_w = open(pipe_name.str().c_str() , O_WRONLY);

    

    while (kill_flag)
    {
        int help;
        BUFF[0] = '\0';
        
        help = read(fd_r , BUFF , 1);
        countLogfileStats(help);
        
        

        if(BUFF[0] == 'p')
        {
            worker_reads_path(fd_r);
        }
        else if(BUFF[0] == 'l')
        {
            worker_reads_l(fd_w);
        }
        else if(BUFF[0] == 's')
        {
            char buff[512];
            int msg_size;
            int h;

            h = read(fd_r , &msg_size , sizeof(int));
            countLogfileStats(h);
            h = read(fd_r , buff, msg_size);
            countLogfileStats(h);

            structure.searchPatientRecord(buff, fd_w);
        }
        else if(BUFF[0] == 'a')
        {
            char buff[512];
            int msg_size; // string length + "string" !!
            int args_;
            int h;

            h = read(fd_r, &args_, sizeof(int));
            countLogfileStats(h);
            h = read(fd_r, &msg_size, sizeof(int));
            countLogfileStats(h);
            h = read(fd_r, buff, msg_size);
            countLogfileStats(h);
            
            worker_reads_a(fd_w, buff, args_);
        }
        else if(BUFF[0] == 'd')
        {
            char buff[512];
            int msg_size; // string length + "string" !!
            int args_;
            int h;

            h = read(fd_r, &args_, sizeof(int));
            countLogfileStats(h);
            h = read(fd_r, &msg_size, sizeof(int));
            countLogfileStats(h);
            h = read(fd_r, buff, msg_size);
            countLogfileStats(h);
            
            worker_reads_d(fd_w, buff, args_);
        }
        else if(BUFF[0] == 'f')
        {
            char buff[512];
            int msg_size; // string length + "string" !!
            int args_;
            int h;

            h = read(fd_r, &args_, sizeof(int));
            countLogfileStats(h);
            h = read(fd_r, &msg_size, sizeof(int));
            countLogfileStats(h);
            h = read(fd_r, buff, msg_size);
            countLogfileStats(h);

            worker_reads_f(fd_w, buff, args_);
            
        }
        else if(BUFF[0] == 'k')
        {
            char buff[512];
            int msg_size; //string apo disease kai pera.
            int k_; //to k mas
            int h;

            h = read(fd_r, &k_, sizeof(int));
            countLogfileStats(h);
            h = read(fd_r, &msg_size, sizeof(int));
            countLogfileStats(h);
            h = read(fd_r, buff, msg_size);
            countLogfileStats(h);

            worker_reads_k(fd_w, buff, k_);
            
        }
        
    }
    
    delete list;
    delete structure.root;
    //cout << "EDW" << endl;
    write_text_to_log_file();

    
}
