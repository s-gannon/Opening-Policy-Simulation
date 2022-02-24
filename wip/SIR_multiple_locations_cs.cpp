#include <cmath>
#include <fstream>
#include <iostream>

#define MAX_TIME 900
#define INPUT_FILE_PATH "input/"
#define OUTPUT_FILE_PATH "output/"

using std::cout;
using std::cin;
using std::ifstream;
using std::ofstream;
using std::endl;
using std::string;

//rate of moving back home from the locations 1, 2, ..., L
float rate_of_moving_back(int n)
{
	if(n % 24 < 8)			//if this passes, number is less than 7
		return 1;
	else if(n % 24 < 16)	//test to see if number is <= 8 not necessary; tested in the if before this
		return 0.2;
	else if(n % 24 < 20)
		return 0.3;
	else
		return 0.4;
}

float rate_of_moving_back(int n, int other_vals[4])
{
	if(n % 24 < 8)			//if this passes, number is less than 7
		return other_vals[0];
	else if(n % 24 < 16)	//test to see if number is <= 8 not necessary; tested in the if before this
		return other_vals[1];
	else if(n % 24 < 20)
		return other_vals[2];
	else
		return other_vals[3];
}

// rate of moving from home to the locations 1, 2, ..., L
float rate_of_moving_to(int n)
{
	if(n % 24 < 8) 			//could do (n % 24)/8 and use that as an index for an array
		return 0.01;
	else if(n % 24 < 16)
		return 0.08;
	else if(n % 24 < 20)	//S: original line: else if(n%24>=16 and n%24<20)
		return 0.05;
	else
		return 0.02;
}

float rate_of_moving_to(int n, int other_vals[4])
{
	if(n % 24 < 8)			//if this passes, number is less than 7
		return other_vals[0];
	else if(n % 24 < 16)	//test to see if number is <= 8 not necessary; tested in the if before this
		return other_vals[1];
	else if(n % 24 < 20)
		return other_vals[2];
	else
		return other_vals[3];
}

//this function returns 0 if the time becomes negative and 1 if the time is positive and it is used to prevent negative times
int is_negative(int n)
{
	return !(n < 0);
}

//this function returns viral load fraction of saturated level N or 1 if viral load exceeds N
float get_viral_load(float load, float N)
{
	return (load > N ? 1 : (load/N));
}

//returns zero if arguments differ and 1 if they are equal in order to capture matching between two locations
int is_equal(int a, int b)
{
	return (a == b);
}

int main(int argc, char* argv[])
{
    int prefs, locs, times, curr_time;					//preferences, locations, and times
    float infect_rate, remov_rate, mort_rate;	//max. infection rate, removal rate, mortality rate

	string input;			//input from the user
    ifstream infile;		//input data; will eventually be optional after random number implementation
	ofstream outfile;		//output data; used to get output

	if(argc == 1)
	{
	    cout << "Enter the name of the input data file (no .txt): ";
	    cin >> input;
	    input.append(".txt");
	    try
		{
	        infile.open(INPUT_FILE_PATH + input);
	    }
	    catch(...)
		{
	        cout << "Error. File not found or invalid file name." << endl;
	        cout << "File name given: " << input << endl;
	        exit(0);
	    }
	}
	else
	{
		string cl_input = argv[1];
		cl_input.append(".txt");
		try
		{
			infile.open(OUTPUT_FILE_PATH + cl_input);
		}
		catch(...)
		{
			cout << "Error. File not found or invalid file name." << endl;
	        cout << "File name given: " << input << endl;
	        exit(0);
		}
	}

	// P L T lambda r mu
    infile >> prefs >> locs >> times >> infect_rate >> remov_rate >> mort_rate;

	//locs++;		//incremented in order to account for home (?)

	cout << "Preferences: " << prefs;
	cout << " | Locations: " << locs;
	cout << " | Time Slots: " << times;
	cout << " | Maximum Infection Rate: " << infect_rate;
	cout << " | Removal Rate: " << remov_rate;
	cout << " | Mortality Rate: " << mort_rate << endl;

    float ***susceptible = new float**[prefs];	//susceptible people of a pref. at a location and time
    float ***infected = new float**[prefs];		//infected people of a pref. at a location and time
    float *removed = new float[MAX_TIME];		//removed individuals at a specific time
    float *cur_viral_load = new float[locs + 1];	//current viral load at each location
    float *max_viral_load = new float[locs + 1];	//maximum viral load at each location
    int **choice_rank = new int*[prefs];		//choice ranking of locations
    int **open_policies = new int*[times];	//opening policies for each location
    int *time_segments = new int[times];		//the start and end times for each time slot

	//creating all of the multidimensional arrays
    for(int p = 0; p < prefs; p++)
    {
        susceptible[p] = new float*[locs];
        infected[p] = new float*[locs];
        for(int l = 0; l < locs; l++)
        {
            susceptible[p][l] = new float[MAX_TIME];
            infected[p][l] = new float[MAX_TIME];
        }
    }
    for(int p = 0; p < prefs; p++)
    {
        choice_rank[p] = new int[locs];
    }
    for(int tm = 0; tm < times; tm++)
    {
        open_policies[tm] = new int[locs];
    }

	for(int j = 0; j < locs; j++)
	{
		infile >> max_viral_load[j];
		cout << max_viral_load[j] << " ";
	}
	cout << endl;

	for(int i = 0; i < prefs; i++)
	{
		infile >> susceptible[i][0][0] >> infected[i][0][0];
		cout << susceptible[i][0][0] << " " << infected[i][0][0] << endl;
	}

	for(int i = 0; i < prefs; i++)
	{
		for(int j = 0; j < locs; j++)
		{
			infile >> choice_rank[i][j];
			cout << choice_rank[i][j] << " ";
		}
		cout << endl;
	}

	for(int i = 0; i < times; i++)
	{
		infile >> time_segments[i];
		cout << time_segments[i] << " ";
	}
	cout << endl;

	for(int i = 0; i < times; i++)
	{
		for(int j = 0; j < locs; j++)
		{
			infile >> open_policies[i][j];
			cout << open_policies[i][j] << " ";
		}
		cout << endl;
	}

	for(int i = 0; i < prefs; i++)
	{
		for(int j = 0; j < locs; j++)
		{
			susceptible[i][j][0] = 0;
			infected[i][j][0] = 0;
		}
	}

	infile.close();
	if(argc < 2)
	{
		outfile.open("output_data.txt");
	}
	else
	{
		string out_name = argv[2];
		outfile.open(OUTPUT_FILE_PATH + out_name + ".txt");
	}

	curr_time = 0;
	removed[0] = 0;
	while(curr_time < MAX_TIME)
	{
		for(int j = 0; j < locs; j++)
		{
			cur_viral_load[j] = 0;
		}
		//death from disease
		removed[curr_time + 1] = removed[curr_time] - mort_rate * removed[curr_time];
		curr_time++;
	}

	outfile.close();

    return 0;
}
