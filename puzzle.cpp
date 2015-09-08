/********************************************************************************************************************
 * 	Project Nş.1 																							T7G03
 * 												Develloped by:
 * 												Josef Busta
 * 												Sara Paiva
 * 					
 *	g++ -Wall -pedantic -std=c++0x -lboost_regex -g -o puzzle puzzle.cpp	
 *	Compiled on Fedora release 18 (kernel 3.8.7-201.fc18.x86_64)
 *
 *
 *********************************************************************************************************************/



#include <iostream>
#include <vector>
#include <stdio.h>
#include <algorithm>	
#include <ctime>		
#include <cstdlib>		
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <string.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <regex>
#include <dirent.h>
#include <sstream>
#include <iterator>
#include <exception>


using namespace std;

//Declaration of functions
string itoa(int i);
int compare_with_solution(vector< vector<int> > random_matrix, int row, int column);
vector< vector<int> > randomize_matrix(int row, int column);
void clrscr();
int getch(void);
void matrix_print (vector< vector<int> > random_matrix, int row, int column);
void write_resultes_into_the_file(std::ofstream& myfile, string time);
void create_file(vector< vector<int> > to_save, string time, int row, int column, string line);
bool fexists(const char *filename);
int UpdatePuzzList(string lline);
int rcolumn (string name);
int rrow (string name);
vector< vector<int> > gen_board_from_txt(const char * name);
void update_player_results(const char * name, string all);
int listFile(string line);
int main(int argc, char *argv[]);
int moves ();
int do_you_want_next_game(string to_print);
string play (vector< vector<int> > random_matrix, int row, int column, string line, int mode, int mode2);
boost::smatch validate_input(string re, string output_string);
vector<string> Tokenize(const string& str);

//quit the program if read EOF
struct QuitNow: public std::exception {
	QuitNow() { }
	virtual ~QuitNow() throw() { }
	virtual const char* what() throw()	{
		return "QuitNow: request normal termination of program.\n"
			"(You should not see this message. Please report it if you do.)";
	}
};

//This function compares each element of the matrix with the number that should be present in final solution, returning an 0 if matrix is not solution and 1 if it is.
int compare_with_solution(vector< vector<int> > random_matrix, int row, int column){
	int kk = 1; //increasing this variable to check if the number in the matrix is same as value of this variable is on right position
	int test = 0; //this variable has value which corresponds to number of right position
	int ret = 0; //variable which indicate if the matrix is solved
	if (random_matrix[row-1][column-1] == 0){test++;}
	for (int i=0;i < row;i++) {
		for(int j=0;j < column;j++) {
			if (random_matrix[i][j] == kk){
				test++;
			}
			kk++;
		}
	}
	if (test == (row*column)){
		ret = 1;
	}
	return ret;
}

//Function responsible for creation of a random matrix different from the final solution of the game
vector< vector<int> > randomize_matrix(int row, int column){
	vector< vector<int> > board(row, vector<int>(column,0));
	int ii = 1;
	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < column; j++)
		{
			if ( i == row-1 && j == column-1)
				{board[row-1][column-1] = 0;}
			else
			{
					board[i][j] = ii; 
					ii++;
			}
		}
	}

	int x, y, old_x, old_y, minn, notsolved = 1;	
	x = column-1;
	old_x = column-1;
	y = row-1;
	old_y = row-1;
	srand (time(NULL));
	minn = ((rand() % 10)*(row*column)) + (rand() % 42) + 42;
	int cond = 0;
	while (minn > cond || notsolved == 1){
		cond++;
		int num;
		num = (rand() % 4);
		switch (num)
		{		
		case 0:
			if (x>0){
			x--;
			}
			break;
		case 1:
			if (y>0){
			y--;
			}
			break;
		case 2: 
			if (x<column-1){
			x++;
			}
			break;
		case 3: 
			if (y<row-1){
			y++;
			}
			break;
		}
		int old_item;
		old_item = board[old_y][old_x];
		board[old_y][old_x] = board[y][x];
		board[y][x] = old_item;
		old_x = x;
		old_y = y;
		notsolved = compare_with_solution(board, row, column);
	}
	return board;
}

//Function responsible for cleaning screen
void clrscr(){
	std::cout << "\33[2J\33[1;1H" << std::flush;
} 

//Function that reads a character written by user without needing the ENTER key
int getch(void){
	int ch;
	struct termios oldt;
	struct termios newt;
	tcgetattr(STDIN_FILENO, &oldt); /*store old settings */
	newt = oldt; /* copy old settings to new settings */
	newt.c_lflag &= ~(ICANON | ECHO); /* make one change to old settings in new settings */
	tcsetattr(STDIN_FILENO, TCSANOW, &newt); /*apply the new settings immediately */
	ch = getchar(); /* standard getchar call */
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt); /*reapply the old settings */
	return ch; /*return received char */
}

//Function responsible for printing the game matrix on the screen
void matrix_print (vector< vector<int> > random_matrix, int row, int column){
	clrscr();
	cout << "Press: 'a'--left 'w'--up 'd'--right 's'--down or 'q' to exit game\n";
	printf("\n");
	for (int i=0;i < row;i++){
		for(int j=0;j < column;j++){
			if (random_matrix[i][j] == 0){			
				printf("\t ");
			}else{
				printf("\t%d", random_matrix[i][j]);
			}
		}
		printf("\n\n");
	}
}

//Function that writes the line with name age sex and time taken by the player into the file puzzle_NxNyyy
void write_resultes_into_the_file(std::ofstream& myfile, string time){
	int ii = 0;
	vector<string> vec = Tokenize(time);
	int here = myfile.tellp();
	myfile << "                               ";
	for (std::vector<std::string>::size_type a = 0; a < vec.size(); ++a){
		switch (ii)
		{		
		case 0:
			myfile.seekp ((here + 4) );
			myfile << vec.at(a);
			break;
		case 1:
			myfile.seekp ((here + 25 ));
			myfile << vec.at(a);
			break;
		case 2: 
			myfile.seekp ((here + 29 ));
			myfile << vec.at(a);
			break;
		case 3: 
			myfile.seekp ((here + 31 ));
			myfile << vec.at(a);
			break;
		}
		ii++;
	}
}

//Function that reads puzzle_numbers.txt , finds the number of games finished in a certain size and creates a text file puzzle_nxn with that number by calling write_resultes_into_the_file()
void create_file(vector< vector<int> > to_save, string time, int row, int column, string line){
	string re = "^" + line + " ([0-9]+)$";
	ifstream myfile1 ("./puzzle_numbers.txt");
	boost::regex expression( re );
	boost::smatch match;
	string line2;
	while (getline( myfile1, line2 )){ 
		if(boost::regex_search( line2, match, expression, boost::match_not_dot_newline )== 1 ){
			break;
		}
	}
	string name = "./saved_puzz/puzzle_" + line + "_" + match[1];
	ofstream myfile (name);
	if (myfile.is_open()){
		for (int i=0;i < row;i++){
			for(int j=0;j < column;j++){
				if (to_save[i][j] == 0){
					myfile << "\t ";			
				}else{
					myfile << "\t" << to_save[i][j];
				}
			}
			myfile <<"\n";
		}
		myfile <<"\n";

		write_resultes_into_the_file(myfile, time);

		myfile << "\n";
		myfile.close();
	}
	else cout << "Unable to open file!";
}

//function that checks if a file exists
bool fexists(const char *filename){
	ifstream ifile(filename);
	return ifile;
}

//Function that finds the size and the number of games won in that size and updates it according to the format puzzle_nxn games won
//If there is no game with that size in that file, the function adds a new item to the list
int UpdatePuzzList(string lline){
	string line;
	string re = "^" + lline + " ([0-9]+)$";
	if(fexists("./puzzle_numbers.txt")){
		ifstream myfile1 ("./puzzle_numbers.txt");
		ofstream _myfile ("./_puzzle_numbers.txt");
		int test = 0;
		while (getline( myfile1, line )){ 
			boost::regex expression( re );
			boost::smatch match;
			if ( boost::regex_search( line, match, expression, boost::match_not_dot_newline )== 1 ){
				int game_count = boost::lexical_cast<int>(match[1]);
				game_count++;
				_myfile << lline << " " << game_count << "\n";
				test = 1;
			}else{
				_myfile << line << "\n";
			}
		}
		if (test == 0){_myfile << lline << " 1\n";}
		myfile1.close();
		_myfile.close();
		remove( "./puzzle_numbers.txt" );
		rename( "./_puzzle_numbers.txt" , "./puzzle_numbers.txt" );

	}else{
		ofstream myfile ("./puzzle_numbers.txt");
		myfile << lline << " 1\n";
		myfile.close();
	}
	return 0;
}


//Function that returns the number of columns generated from the name of the file
int rcolumn (string name){
	boost::regex expression( "_[0-9]+x([0-9]+)_" );
	boost::smatch match;
	boost::regex_search( name, match, expression, boost::match_not_dot_newline );
	return boost::lexical_cast<int>(match[1]);
}

//Function that returns the number of rows generated from the name of the file
int rrow (string name){
	boost::regex expression( "_([0-9]+)x[0-9]+_" );
	boost::smatch match;
	boost::regex_search( name, match, expression, boost::match_not_dot_newline );
	return boost::lexical_cast<int>(match[1]);
}

//This function breaks a string into elements separated by tab that are then added to a vector
vector<string> Tokenize(const string& str) {
vector<string> tokens;
const string delimiters = "\t";
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
		 {
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.	Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
		}
return tokens;
}

//Function that generates a board from a text file
vector< vector<int> > gen_board_from_txt(const char * name){
	int row = rrow (name);
	int column = rcolumn (name);
	vector< vector<int> > board(row, vector<int>(column,0));
	ifstream myfile (name);
	for(int i = 0; i < row; i++){
		string lline;
		getline (myfile,lline);
		vector<string> vec = Tokenize(lline);
		vector<int> uloz(column,0);
		int ii = 0;
		for (std::vector<std::string>::size_type a = 0; a < vec.size(); ++a){
			if (vec.at(a) == " "){
				uloz[ii] = 0;
				ii++;
			}else{
				uloz[ii] = boost::lexical_cast<int>(vec.at(a));
				ii++;
			}
		}
		for(int j = 0; j < column; j++){
			board[i][j] = uloz[j];
		}
	}
	return board;
}

//Function that Updates players list according to their times, keeping only a maximum of 10 results
void update_player_results(const char * name, string all){
	int test = 0, test1 = 0, k=0;
	string line, ll;
	ifstream myfile (name);
	ofstream myfile1 ("./saved_puzz/temp.txt");

	boost::regex expression( "\t([0-9]+)$" );
	boost::smatch match;

	boost::regex_search( all, match, expression, boost::match_not_dot_newline );
	int time = boost::lexical_cast<int>(match[1]);

	while ( myfile.good() )	{
		getline (myfile,line);
		if (test == 0){
			myfile1 << line << "\n";
		}else{
			k++;
			if (k==10){
				break;
			}
			ll=line+"\n";
			boost::regex expression1( " ([0-9]+)\n" );
			boost::smatch match1;
			if (boost::regex_search( ll, match1, expression1, boost::match_not_dot_newline )==1){
				int time1 = boost::lexical_cast<int>(match1[1]);
				if (time < time1 && test1 == 0){
					write_resultes_into_the_file(myfile1, all);
					myfile1 << "\n";
					myfile1 << line << "\n";
					test1 = 1;
				}else{
					myfile1 << line << "\n";
				}	
			}
		}
		if (line == ""){
			test = 1;
		}
	}
	if (test1 == 0){
		write_resultes_into_the_file(myfile1, all);
		myfile1 << "\n";
	}
	myfile.close();
	myfile1.close();
	remove( name );
	rename( "./saved_puzz/temp.txt" , name );
}


//Function responsible for showing matrix available to be loaded from a certain size
int listFile(string line){
	int choice_for_play_or_next;
	int k = 0;
	int l = 1;
	string name1 = "./saved_puzz/puzzle_" + line + "_1";
	int row = rrow (name1);
	int column = rcolumn (name1);
	const char * name = name1.c_str();
	if (!fexists(name)){
		cout << "There are no files to load of the size '" << line <<"'!\n";
	}else{
		while (fexists(name) && k==0){
			string lline;
			ifstream myfile (name);
			while ( myfile.good() )
			{
				getline (myfile,lline);
				cout << lline << endl;
			}
			boost::smatch mode_match = validate_input("^([12])$", "Show next [1] or play [2]? 1/2 ");
			choice_for_play_or_next = boost::lexical_cast<int>( mode_match[1]);
			if (choice_for_play_or_next == 2){
				vector< vector<int> > board = gen_board_from_txt(name);
				string all;
				all = play(board, row, column, line, 1, 2);
				if (all != ""){
					update_player_results(name, all);
				}
				k=1;
			}
			myfile.close();
			l++;
			ostringstream convert;
			convert << l;
			string ll = convert.str();
			name1 = "./saved_puzz/puzzle_" + line + "_" + ll;
			name = name1.c_str();
			if (!fexists(name) && k == 0){
				cout << "There are no more files to load of the size '" << line <<"'!\n";
			}
		}
	}
	return 0;
}


//Function responsible for matching a string and deciding if it is valid
boost::smatch validate_input(string re, string output_string){
	cout << output_string;
	boost::regex expression( re );
	boost::smatch match;
	string line="";
	while ( (boost::regex_search( line, match, expression, boost::match_default | boost::match_partial )==0) && !cin.eof() ){
		getline (cin, line);
		if (!cin.eof() && (boost::regex_search( line, match, expression, boost::match_default | boost::match_partial) )==0 && line!=""){
			cout << "Invalid input! Try it again or EOF to exit! ";
		}else if (cin.eof()){
			throw QuitNow();
		}
	}
	return match;
}


//Creates string from a number
string itoa(int i){
	std::string s;
	std::stringstream out;
	out << i;
	s = out.str();
	return s;
}

//
//returns empty string if the game is not solved otherwise returns string with name, age, sex and time in which the game was solved
string play (vector< vector<int> > random_matrix, int row, int column, string line, int mode, int mode2){
	vector< vector<int> > to_save;
	int x, y, old_x, old_y; //positions in the matrix
	to_save = random_matrix;
	string all = ""; //string to return
	int test = 0; //print the matrix just once in the beginning
	for (int i=0;i < row;i++){ //find the position of blank space
		for(int j=0;j < column;j++){
			if (random_matrix[i][j] == 0){x=j; y=i; old_x = j; old_y = i;}
		}
	}
	time_t start, end;
	int counter = 1; //starts measure time after pressing first char
	char ch; //char which is read from stdin
	do{
		if (test == 0){ //it is performed once per game in the beginning
			matrix_print (random_matrix, row, column);
			test = 1;
		}
		ch = getch(); //get one char without entering
		if (counter == 1){ //start to measure time, performed once in the game after reading first char
			start = time(0);
		}
		switch (ch) //change current position in the matrix
		{		
		case 'a':
			if (x>0){
			x--;
			}
			break;
		case 'w':
			if (y>0){
			y--;
			}
			break;
		case 'd': 
			if (x<column-1){
			x++;
			}
			break;
		case 's': 
			if (y<row-1){
			y++;
			}
			break;
		}
		int old_item;
		old_item = random_matrix[old_y][old_x];
		random_matrix[old_y][old_x] = random_matrix[y][x];
		random_matrix[y][x] = old_item;
		old_x = x;
		old_y = y;
		int result = 0;
		matrix_print (random_matrix, row, column);
		result = compare_with_solution(random_matrix, row, column); //check if the matrix is solved
		if (result == 1){
				end = time(0); //stop measure time
				int tt_time = end-start;
				if (tt_time < 1){tt_time = 0;}
				cout << "You solved the game!!! Your time is: " << tt_time <<" sec\n";
				if (mode2 == 2){
					boost::smatch name_match = validate_input("^([A-Za-z0-9 ]{1,20})$", "\nWrite your name: ");	
					string name =  name_match[1];
	
					boost::smatch age_match = validate_input("^([1-9][0-9]{0,2})$", "\nWrite your age: ");	
					string age =  age_match[1];
	
					boost::smatch sex_match = validate_input("^([FMfm])$", "\nWrite your sex: ");	
					string sex =  sex_match[1];

					all = "\t"+name+"\t"+age+"\t"+sex+"\t" + itoa(tt_time);
					if (mode == 0){
						UpdatePuzzList(line);
						create_file(to_save, all, row, column, line);
					}
				}
				ch = 'q';
		}
		counter =2;
	} while (ch != 'q');
	return all;
}

//function which is asking for another game
int do_you_want_next_game(string to_print){
	int ret = 1;
	boost::smatch yn_match = validate_input("^([YynN])$", to_print);	
	string yesno =  yn_match[1];
	if (yesno == "Y" || yesno == "y"){
		ret = 0;
	}
	return ret;
}


//main function, which creates directory for saving puzzles and launch function 'int moves ()'
int main(int argc, char *argv[]){
	struct stat st;
	if(stat("./saved_puzz",&st) == 0){
		moves();	
	}else{
		boost::smatch yn_match = validate_input("^([YynN])$", "\nCreate directory 'saved_puzz' for saving games? Y/N ");	
		string yesno =  yn_match[1];
		if (yesno == "Y" || yesno == "y"){
			int result = mkdir("./saved_puzz",S_IRWXU | S_IRWXG | S_IRWXO);	
			if (result == -1) {
				cout << "Cannot create directory 'saved_puzz'\n";
			}else{
				moves();
			}
		}
	}
	return 0;
}

//function which repeatedly offers a menu and launch appropriate functions
int moves (){
	int next_game = 0;
	while (next_game == 0 && !cin.eof()){
		clrscr();
		try{
			boost::smatch mode_match = validate_input("^([12])$", "Choose the training mode [1] competition mode [2]? 1/2 ");
			int mode = boost::lexical_cast<int>( mode_match[1]);

			int row=0, column=0;
			string line;			
			while (row<3 || row > 12 || row != column){
				boost::smatch matrix_size_match = validate_input("^([0-9]+x[0-9]+)$", "\nEnter Size of puzzle NxN: ");	
				string not_separated =  matrix_size_match[1];

				boost::regex expression( "^([0-9]+)x([0-9]+)$" );
				boost::smatch match;
			
				boost::regex_search( not_separated, match, expression, boost::match_not_dot_newline ); 
				row = boost::lexical_cast<int>(match[1]);
				column = boost::lexical_cast<int>(match[2]);
				line = not_separated;
				if(row<3 || row > 12 || row != column){
					cout << "Invalid input! Size of board can be: 3x3 - 12x12!\n";
				}
			}
			if (mode == 1) {
				int next_game1 = 0;
				while (next_game1 == 0 && !cin.eof()){
					vector< vector<int> > random_matrix;
					random_matrix = randomize_matrix(row, column);
					play(random_matrix, row, column, line, 0, mode);
					next_game1 = do_you_want_next_game("\nPlay again? Y/N ");
				}
				next_game = do_you_want_next_game("\nBack to menu? Y/N ");
			}else {
				int next_game1 = 0;
				while (next_game1 == 0 && !cin.eof()){
					boost::smatch mode_match = validate_input("^([12])$", "Generate a new puzzle [1] or load a previously saved puzzle [2]? 1/2 ");
					int yesno = boost::lexical_cast<int>( mode_match[1]);
					if (yesno == 2){
						listFile(line);
					}else{
						vector< vector<int> > random_matrix;
						random_matrix = randomize_matrix(row, column);
						play(random_matrix, row, column, line, 0, mode);
					}
					next_game1 = do_you_want_next_game("\nPlay again? Y/N ");
				}
				next_game = do_you_want_next_game("\nBack to menu? Y/N ");
			}
			
		}catch(QuitNow){
			cout << "<Ctrl>d\n";
			break;
		}
	}
	return 0;
}
