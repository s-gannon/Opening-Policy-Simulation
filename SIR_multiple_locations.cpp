#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>

using std::cout;
using std::cin;
using std::ifstream;
using std::ofstream;
using std::endl;

// Ideas for improvement: the transfer rates between locations should be dynamic and adjusted if there is a time interval with
// no openings (i.e. more individuals will go to the store at other times when the stores are open

//rate of moving back home from locations 1,2,...,L

float a(int n)
{
	if(n % 24 < 8)
		return 1;
	else if(n % 24 < 16)
		return 0.2;
	else if(n % 24 < 20)
		return 0.3;
	else
		return 0.4;
}

float a(int n, int other_vals[4])
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

// rate of  moving rates from home to the locations 1,2...,L
float b(int n)
{
	if(n % 24 < 8)
		return 0.01;
	else if(n % 24 < 16)
		return 0.08;
	else if(n % 24 < 20)	//S: original line: else if(n%24>=16 and n%24<20)
		return 0.05;
	else
		return 0.02;
}

float b(int n, int other_vals[4])
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
int f(int n)
{
	return !(n < 0);
}

//this function returns  viral load fraction of saturated level N or 1 if  viral load exceeds  N
float g(float load, float N)
{
	return (load > N ? 1 : (load/N));
}

//returns zero if arguments differ and 1 if they are equal in order to capture matching between two locations
int h(int a, int b)
{
	return (a == b);
}

int main()
{   //declaration of variables to be taken from the input data file
    int P,L; //number of locations L and preferences P
	float N[10]; //N[i] is saturation viral load at location i
	float S[10][10][1000],I[10][10][1000];// S[i][j][n], I[i][j][n] is number of  susceptible/infected of preference i, at location j at time n
    int c[10][10]; // c[i,j] choice ranking of locations. Each row represents one preference. The matrix has P rows and L columns
    int T; // Number of time ranges for the opening of locations policies
    int t[10]; // threshold times of opening policies: [0,t1], [t1,t2],...[t_{T-1},t_T]
    int o[10][10]; //opening policies: each row corresponds to one policy and each entry is either 0 or 1 indicating the location open or closed
                   // o[i][j]=1 will mean: in the time range indicated by t[i], location j is open. Thus the "o" matrix has T rows and L columns

	float lambda, r, mu; //maximum infection rate, removal rate and disease mortality

	//variables outside of input data file
	float R[1000]; //R[n] removed individuals at time n
	float Ncrt[10]; // Ncrt[j] Current viral  load at location j
	int n,i,j,k,l,nothing_open;
	float Scounter,Icounter;//  Counts the susceptible and infected at the current location
    float aux;
    float Saux,Iaux; // Counts the susceptible and infected at time n that will return home from locations 1,2,..,L


//  reading data from the input file and displaying to console for verification. Console display can be removed later
	ifstream infile;
    infile.open("Input_Data_6.txt");

	infile>>P>>L;
	cout<<P<<" "<<L<<endl;

    for(j=0;j<=L;j++)	//out of bounds?
	{
		infile>>N[j];
		cout<<N[j]<<" ";
	}
	cout<<endl;

	for(i=1;i<=P;i++)	//also out of bounds?
	{
		infile>>S[i][0][0]>>I[i][0][0];
		cout<<S[i][0][0]<<" "<<I[i][0][0]<<endl;
	}

	for(i=1;i<=P;i++)
	{
		for(j=1;j<=L;j++)
		{
			infile>>c[i][j];
			cout<<c[i][j]<<" ";
		}
		cout<<endl;
  	}

	infile>>T;
	cout<<T<<endl;

	for(i=1;i<=T;i++)
	{
		infile>>t[i];
		cout<<t[i]<<" ";
	}
	cout<<endl;

	for(i=1;i<=T;i++)
	{
		for(j=1;j<=L;j++)
		{
		    infile>>o[i][j];
		  	cout<<o[i][j]<<" ";
		}
		cout<<endl;
	}

	infile>>lambda>>r>>mu;
	cout<<"lambda: "<<lambda<<" r: "<<r<<" mu: "<<mu<<endl;

	infile.close();

    // initialization with zero of the populations at locations 1,2,...,L

	for(j=1;j<=L;j++) //this should be to P
	{
		for(i=1;i<=P;i++) //this should be to L
		{
			S[i][j][0]=0;	//because this is S[Pref][Loc][Time]
			I[i][j][0]=0;
		}
	}

	// opening Output data file for writing
  	ofstream outfile;
  	outfile.open("/output_files/Output_Data_6.txt");

	n=0;//initialization time counter
    R[0]=0;//initialization recovered individuals
	while(n<=900)
	{
		//calculation of the viral load at location j and the recovery step
		for(j=0;j<=L;j++)
     	{
			Ncrt[j]=0;	// LEFT OFF HERE
			for(i=1;i<=P;i++)
			{
				aux=r*I[i][j][n];
				I[i][j][n] -= aux;
				R[n] += aux;
				Ncrt[j] += I[i][j][n]+f(n-1)*I[i][j][n-1]+f(n-2)*I[i][j][n-2];
			}
		}
		//infection step
		for(j=0;j<=L;j++)
		{
			for(i=1;i<=P;i++)
			{
				aux=lambda*g(Ncrt[j],N[j])*S[i][j][n];
				S[i][j][n] -= aux;
				I[i][j][n] += aux;
			}
		}
	//writting data in the outputfile: S and I by location
		for(j=0;j<=L;j++)
		{
			Scounter=0;
			Icounter=0;
			char fmt_str[30] = "";
			for(i=1;i<=P;i++)
			{
				Scounter += S[i][j][n];
				Icounter += I[i][j][n];
			}
			sprintf(fmt_str, "%3.0f , %3.0f||", roundf(Scounter), roundf(Icounter));
			std::string temp = fmt_str;
            outfile << temp;
			//outfile<<round(Scounter)<<" , "<<round(Icounter)<<"||";
		}

		outfile<<endl;

		//moving between locations step
		for(i=1;i<=P;i++)
		{
			//identifying the time interval for the opening policy where current time n belongs to
			k=1;
			while(!(t[k-1]<=n%24 && n%24<=t[k]))
				k++;
			//identifying the first location open in the order of preference
			l=0;
			o[k][c[i][l]]=0;
			while(o[k][c[i][l]]==0 && l<=L)
				l++;

			if(l>L)
				nothing_open=0;
			else
				nothing_open=1;

			//Calculating the total susceptible and infected at locations 1,2,...,L from which a fraction moves home
			for(j=1,Saux=0,Iaux=0;j<=L;j++)
			{
				Saux += S[i][j][n];
				Iaux += I[i][j][n];
			}

			 // updating the home susceptible and infected
			S[i][0][n+1]=S[i][0][n]-nothing_open*b(n)*S[i][0][n]+a(n)*Saux;
			I[i][0][n+1]=I[i][0][n]-nothing_open*b(n)*I[i][0][n]+a(n)*Iaux;

			// updating the susceptible and infected at locations 1,2,...,L
			for(j=1;j<=L;j++)
			{
				S[i][j][n+1]=S[i][j][n]-a(n)*S[i][j][n]+nothing_open*h(j,c[i][l])*b(n)*S[i][0][n];
				I[i][j][n+1]=I[i][j][n]-a(n)*I[i][j][n]+nothing_open*h(j,c[i][l])*b(n)*I[i][0][n];
			}

		}

	//  death from disease
		R[n+1]=R[n]-mu*R[n];
		n++;
	}

	outfile.close();
}
