#include <iostream>		// include input output stream
#include <string>		// include strings 
#include <fstream>		// include file stream
#include <vector>		// include vectors
#include <map> 			// include maps
#include <sstream>		// include string streams
#include <iterator>
#include <algorithm>	// (e.g. searching, sorting, counting, manipulating)
#include <windows.h> 		// standard windows library
#include <iomanip>

//#include "things.hpp"

#define WINDOWS
//#define LINUX

#ifdef WINDOWS
	#ifdef LINUX
		#error You can not compile with LINUX and WINDOWS defines
	#endif
#endif

HANDLE col = GetStdHandle(STD_OUTPUT_HANDLE);

#ifdef WINDOWS
void findTheColor(std::string color) {		// function, that will set output color, by the string
	if(!color.compare("white")) {
		SetConsoleTextAttribute(col, 7);
	} else if(!color.compare("green")) {
		SetConsoleTextAttribute(col, 10);
	} else if(!color.compare("red")) {
		SetConsoleTextAttribute(col, 4);
	} else if(!color.compare("blue")) {
		SetConsoleTextAttribute(col, 1);
	} else if(!color.compare("yellow")) {
		SetConsoleTextAttribute(col, 14);
	} else if(!color.compare("gray")) {
		SetConsoleTextAttribute(col, 8);
	}
}
#endif

#ifdef LINUX
#error Not defined colors for linux YET!
void findTheColor(std::string color) {		// function, that will set output color, by the string
	if(!color.compare("white")) {			// for linux not defined
		SetConsoleTextAttribute(col, 7);
	} else if(!color.compare("green")) {
		SetConsoleTextAttribute(col, 10);
	} else if(!color.compare("red")) {
		SetConsoleTextAttribute(col, 4);
	} else if(!color.compare("blue")) {
		SetConsoleTextAttribute(col, 1);
	} else if(!color.compare("yellow")) {
		SetConsoleTextAttribute(col, 14);
	}
}
#endif

struct thing {		// things you can pickup allong the way
	std::string name;
	int count;	
	std::string ability;
};

struct page {		// defines how a page will look
	std::string text;		// text to display
	std::vector<int> signpost;		// options where to go
	//thing found_thing;
	std::vector<std::string> found_things;
	std::vector<std::string> use_things;
};

struct game {		
	std::vector<page> scenario;		// vector of pages saved
	int choice;
	std::vector<thing> inventory;		// array of things
	std::vector<thing> pile_of_junk;	// array of all things from file
	//int page_loaded;		// max number page loaded - not implemeted

	void load(std::string filename);	// function for loading file
	void loadThings(std::string filename); 	// function for loading file with things
	void play(int next = 0);		// function for plaing loaded files
	void titlePage(std::string filename);
	void showInventory();
};

void game::load(std::string filename) {
	std::ifstream ifs(filename);		// creates file stream

	if(!ifs.good()) {		// error message
		SetConsoleTextAttribute(col, 4);
		std::cout << "ERROR: something went wrong when loading a file (" <<
		filename << ")" << std::endl;
		SetConsoleTextAttribute(col, 7);
		return;
	}

	while(!ifs.eof()) {
		page temp;		// creates temporary page
		int choice_count;	// how many next pages there are
		char c;
		

		std::getline(ifs, temp.text);		// get line of game text
		ifs >> choice_count;		// get one number from file

		if(choice_count > 0) {		// copies next page numbres (choices) to signpost
			std::copy_n(std::istream_iterator<int>(ifs), choice_count, std::back_inserter(temp.signpost));
			// read int using iterator from file and place it into signpost using bac_inserter
		} else if(choice_count < 0) {
			choice_count = choice_count * -1;
			std::string temp_str;
			for(int i = 0; i <= choice_count; i++) {
				std::getline(ifs, temp_str, ' ');
				temp.use_things.push_back(temp_str);
			}
			std::copy_n(std::istream_iterator<int>(ifs), choice_count, std::back_inserter(temp.signpost));
		}

		do {		
            ifs.get(c); 	
        } while(c != '\n');		// read one character untill we get to next line

		std::string temp_str;
		std::getline(ifs, temp_str);
		if(temp_str.compare("")) {
			std::string temp_str2;
			std::stringstream temp_stream(temp_str);
			do {
				std::getline(temp_stream, temp_str2, ' ');
				temp.found_things.push_back(temp_str2);
				//temp_stream.get();
			} while(!temp_stream.eof());
		}
		

		this->scenario.push_back(temp);		// add filled page to end of scenario file
	}
}

void game::loadThings(std::string filename) {		// load things file and save it into pile_of_juk
	std::ifstream ifs(filename);

	if(!ifs.good()) {		// error message
		SetConsoleTextAttribute(col, 4);
		std::cout << "ERROR: something went wrong when loading a file (" <<
		filename << ")" << std::endl;
		SetConsoleTextAttribute(col, 7);
		return;
	}

	while(!ifs.eof()) {
		thing temp;

		std::getline(ifs, temp.name);
		std::getline(ifs, temp.ability);
		temp.count = 0;

		this->pile_of_junk.push_back(temp);
	}
}

void game::showInventory() {
	std::cout << std::endl << "things in inventory:" << std::endl;
	std::cout << std::left << std::setw(20) << "thing "
	<< std::setw(8) << "count " << "ability " << std::endl;

	for(int i = 0; i < inventory.size(); i++) {		// cycle for every single thing in invetory
		std::cout << std::left << std::setw(20) << inventory[i].name
		<< std::setw(8) << inventory[i].count << inventory[i].ability << std::endl;
	}
}

void game::play(int next) {
	const page* pg;
	do {
		pg = &scenario.at(next);		// read actual page
		//std::cout << pg->text << std::endl; 	// output page text

		std::string temp;			// color output block
		std::string color;
		std::stringstream texts(pg->text);
		while(!texts.eof()) {
			std::getline(texts, temp, '$');
			std::cout << temp;
			if(!texts.eof()) {
				std::getline(texts, color, '$');
				#ifdef WINDOWS
				findTheColor(color);
				#endif
			}
		}
		std::cout << std::endl;


		//std::cout << pg->found_things.size() << std::endl;
		
		if(pg->found_things.size() > 0) {		// show list of found things
			SetConsoleTextAttribute(col, 8);
			std::cout << "you have found: ";
			for(auto thing : pg->found_things) {
				std::cout << thing << " ";
				for(auto thing2 : pile_of_junk) {
					if(!thing.compare(thing2.name)) {
						//std::cout << "INVADD ";
						thing2.count++;
						inventory.push_back(thing2);
					}
				}
			}
			std::cout << std::endl;
			SetConsoleTextAttribute(col, 7);
		}
		
		//You can use one of these items:
		if(pg->use_things.size() > 0) {		// show waht you can use and your inventory
			SetConsoleTextAttribute(col, 8);
			std::cout << "You can use one of these items: ";
			for(auto thing : pg->use_things) {
				std::cout << thing << " ";
			}
			std::cout << std::endl;

			showInventory();
			SetConsoleTextAttribute(col, 7);
		}

		if(pg->signpost.size() > 0) {		// if signpost is empty game ends
			std::cout << "> ";		
			std::cin >> this->choice;		// get user choice
			next= pg->signpost.at(choice-1);	// sets next page from signpost
			std::cout << std::endl;
		} else {
			next = -1;	// indicates end of game
		}
	} while(next > 0);

	//inventory.push_back(pile_of_junk[0]);
	//inventory.push_back(pile_of_junk[1]);

	//showInventory();
	
	std::cout << std::endl << "This is the end. The window will now close" << std::endl;
	system("pause");
	return;
}



void game::titlePage(std::string filename) {
	std::ifstream ifs(filename);

	if(!ifs.good()) {		// error message
		SetConsoleTextAttribute(col, 4);
		std::cout << "ERROR: something went wrong when loading a file (" <<
		filename << ")" << std::endl;
		SetConsoleTextAttribute(col, 7);
		return;
	}

	std::string temp;
	std::string color;
	while(!ifs.eof()) {
		std::getline(ifs, temp, '$');
		std::cout << temp;
		if(!ifs.eof()) {
			//std::cout << "[color]";
			std::getline(ifs, color, '$');
			#ifdef WINDOWS
			findTheColor(color);
			#endif
		}
	}
	std::cout << std::endl;
}

int main() {
	game gm;
	gm.load("scenario.txt");
	gm.loadThings("things.txt");
	gm.titlePage("welcome_page.txt");
	gm.play();	
	SetConsoleTextAttribute(col, 7);
	return 0;
}