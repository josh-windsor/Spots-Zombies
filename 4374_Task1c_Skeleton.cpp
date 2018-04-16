//SKELETON PROGRAM
//---------------------------------
//include libraries
//include standard libraries
#include <iostream >         //for output and input: cin >> and cout <<
#include <iomanip>           //for formatted output in 'cout'
#include <conio.h>           //for getch()
#include <string>            //for string
#include <vector>            //for vectors
#include <fstream>			 //for files
using namespace std;

//include our own libraries
#include "RandomUtils.h"     //for Seed, Random
#include "ConsoleUtils.h"    //for Clrscr, Gotoxy, etc.

//---------------------------------
//define constants
//---------------------------------
//define global constants
//defining the size of the grid
const int SIZEY(12);         //vertical dimension
const int SIZEX(20);         //horizontal dimension
//defining symbols used for display of the grid and content
const char SPOT('@');        //spot
const char TUNNEL(' ');      //open space
const char WALL('#');        //border
const char ZOMBIE('Z');      //zombie
const char PILL(':');      //pill
const char HOLE('O');      //hole
//defining the command letters to move the blob on the maze
const int  UP(72);           //up arrow
const int  DOWN(80);         //down arrow
const int  RIGHT(77);        //right arrow
const int  LEFT(75);         //left arrow
//defining the other command letters
const char QUIT('Q');        //end the game
const char START('P');       //start the game
const char INFO('I');
const char FREEZE('F');      //freeze the zombies
const char EAT('E');         //eat all pills
const char EXTERMINATE('X'); //kill all zombies


const int MAXZOMBIES(4);
const int MAXPILLS(8);
const int MAXHOLES(12);

//data structure to store data for a grid item
struct Item {
	char symbol;	     //symbol on grid
	int x, y;			     //coordinates
};
struct Player :Item
{
	int lives, powerCounter, currentScore;
	bool empowered;
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	void initialiseGame(char grid[][SIZEX], Player& spot, vector<Item>& zombies, vector<Item>& holes, vector<Item>& pills, const int zombieStartingX[], const int zombieStartingY[]);
	bool cheatCheck(int k,int& remainingPills, int& remianingZombies, vector<Item>& zombies, vector<Item>& pills, bool& freezeZombies);
	bool wantToQuit(int k);
	bool wantToPlay(int k);
	bool isArrowKey(int k);
	int  getKeyPress();
	void updateGame(char g[][SIZEX], Player& sp, int k, string& mess, vector<Item>& zombies, vector <Item>& holes, vector <Item>& pills, int& remainingPills, const int zombieStartingX[], const int zombieStartingY[], bool freezeZombies, int& remainingZombies);
	void renderGame(const char gd[][SIZEX], string mess, bool playing, Player spot, int previousScore);
	void endProgram(Player& spot, int remainingPills, int remainingZombies);
	void splashScreen();
	void showMessage(string m, bool playing, Player spot, int previousScore);
	void showInfo(int key);
	void saveLoadScore(int& previousScore, string& playerName, Player spot);


	//local variable declarations 
	int remainingPills;
	int remainingZombies;
	int previousScore;
	bool freezeZombies;
	bool playing;
	freezeZombies = false;
	char grid[SIZEY][SIZEX];                //grid for display
	Player spot;                   //Spot's symbol and position (0, 0) 
	spot.lives = 3;
	spot.symbol = SPOT;
	spot.empowered = false;
	Item zombie = { ZOMBIE };
	Item pill = { PILL };
	Item hole = { HOLE };
	vector<Item> zombies(MAXZOMBIES, zombie);
	vector<Item> pills(MAXPILLS, pill);
	vector<Item> holes(MAXHOLES, hole);
	const int zombieStartingX[8] {1,1,18,18,1,18,18,18}; //Zombie starting co-ords with extras incase more zombies need to added later
	const int zombieStartingY[8] {1, 10, 1, 10, 6, 6, 1, 10};
	string message("LET'S START...      "); //current message to player
	string playerName;

	//action...
	playing = false;
	int key(' ');                         //create key to store keyboard events
	cout << "WELCOME PLAYER! ENTER YOUR NAME!: \n";
	cin >> playerName;
	spot.currentScore = 0;
	saveLoadScore(previousScore, playerName, spot);
	while (!wantToQuit(key)){
		splashScreen();
		playing = false;
		remainingPills = MAXPILLS;
		remainingZombies = MAXZOMBIES;
		spot.empowered = false;
		spot.currentScore = 0;
		initialiseGame(grid, spot, zombies, holes, pills, zombieStartingX, zombieStartingY);           //initialise grid (incl. walls and spot)
		key = getKeyPress();              //read in next keyboard event
		if (wantToPlay(key))
		{
			playing = true;
			Gotoxy(0, 0);
			for (int i(0); i < screenWidth(); ++i)
			{
				for (int ii(0); ii < screenWidth(); ++ii)
					cout << TUNNEL;
			} // clear the screen

			do {
				renderGame(grid, message, playing, spot, previousScore);        //render game state on screen
				message = "                                       "; //reset message
				key = getKeyPress();              //read in next keyboard event
				if (isArrowKey(key))
					updateGame(grid, spot, key, message, zombies, holes, pills, remainingPills, zombieStartingX, zombieStartingY, freezeZombies, remainingZombies);
				else
				{
					if (!cheatCheck(key,remainingPills, remainingZombies, zombies, pills, freezeZombies))
					{
						message = "INVALID KEY!        "; //set 'Invalid key' message
					}
					else
					message = "CHEAT ACCEPTED!        ";
				}
			} while (!wantToQuit(key) && spot.lives >= 1 && remainingPills >= 1 && remainingZombies >= 1);               //while user does not want to quit
			renderGame(grid, message, key, spot, previousScore);        //render game on screen to show final state so player can review how they won or lost
			saveLoadScore(previousScore, playerName, spot);
			endProgram(spot, remainingPills, remainingZombies);

		}
		else
		{
			switch (key)
			{
			default:
			message = "INVALID KEY!        "; //set 'Invalid key' message
			showMessage(message,playing, spot, previousScore);
			key = getKeyPress();              //read in next keyboard event
				break;
			case INFO:
				showInfo(key);
				break;
			case QUIT:
				// does nothing but won't show invalid key message like default case
				// while loop will exit and program will then terminate
				break;
			}
		}
	}
		message = "                                       "; //reset message
	endProgram(spot,remainingPills,remainingZombies);                             //display final message
	Gotoxy(40, 11);
	system("pause");//hold output screen until a keyboard key is hit
	return 0;
} //end main

void updateGame(char grid[][SIZEX], Player& spot, int key, string& message, vector <Item>& zombies, vector <Item>& holes, vector <Item>& pills, int& remainingPills, const int zombieStartingX[], const int zombieStartingY[], bool freezeZombies, int& remainingZombies)
{ //updateGame state
	void updateSpotCoordinates(const char g[][SIZEX], Player& spot, int key, string& mess, vector <Item>& pills, int& remainingPills);
	void updateZombieCoordinates(const char g[][SIZEX], vector <Item>& zombies, int key, string& mess, Player& spot, bool freezeZombies, int& remainingZombies, const int zombieStartingX[], const int zombieStartingY[]);
	void updateGrid(char g[][SIZEX], Player& spot, vector <Item>& zombies, vector <Item>& holes, string& mess, vector <Item>& pills, const int zombieStartingX[], const int zombieStartingY[]);

	updateSpotCoordinates(grid, spot, key, message, pills, remainingPills);    //update spot coordinates
                                                        //according to key
	updateZombieCoordinates(grid, zombies, key, message, spot, freezeZombies, remainingZombies, zombieStartingX, zombieStartingY);

	updateGrid(grid, spot, zombies, holes, message, pills, zombieStartingX, zombieStartingY);                             //update grid information
}

void splashScreen()
{
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(0, 0);
	for (int i(0); i < screenWidth(); ++i)
	{
		for (int ii(0); ii < screenWidth(); ++ii)
			cout << TUNNEL;
	} // clear the screen
	Gotoxy(0, 0);
	string GetTime();
	string GetDate();
	cout << "David, Josh and Rico - 1Y/YY\nProudly Present\nSPOT v.s ZOMBIES\nThe time is " << GetTime() << "\nand the date is " << GetDate();
	Gotoxy(0, 0);
}

//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], Player& spot, vector<Item>& zombies, vector<Item>& holes, vector<Item>&pills, const int zombieStartingX[], const int zombieStartingY[])
{ //initialise grid and place spot in middle
	void setGrid(char[][SIZEX]);
	void setSpotInitialCoordinates(Player& spot);
	void setZombieInitialCoordinates(vector<Item>& zombies, const int zombieStartingX[], const int zombieStartingY[]);
	void setPillInitialCoordinates(vector<Item>& pills, Player&spot, vector<Item>&zombies);
	void setHoleInitialCoordinates(vector<Item>& holes, Player&spot, vector<Item>& pills, vector<Item>&zombies);
	void placeSpot(char gr[][SIZEX], Player spot);
	void placeZombies(char gr[][SIZEX], vector<Item> zombies);
	void placePills(char gr[][SIZEX], vector<Item> pills);
	void placeHoles(char gr[][SIZEX], vector<Item> holes);

	Seed();                            //seed reandom number generator
	spot.lives = 3;
	setZombieInitialCoordinates(zombies, zombieStartingX, zombieStartingY);   //initialise zombie position
	setSpotInitialCoordinates(spot);   //initialise spot position
	setPillInitialCoordinates(pills, spot, zombies);   //initialise pill position
	setHoleInitialCoordinates(holes, spot, pills, zombies);   //initialise hole position
	setGrid(grid);                     //reset empty grid
	placePills(grid, pills);
	placeHoles(grid, holes);
	placeZombies(grid, zombies);
	placeSpot(grid, spot);             //set spot in grid
} //end of initialiseGame

void setSpotInitialCoordinates(Player& spot)
{ //set spot coordinates inside the grid at random at beginning of game
	spot.y = (SIZEY / 2);      //vertical coordinate in range [1..(SIZEY - 2)]
	spot.x = (SIZEX / 2);    //horizontal coordinate in range [1..(SIZEX - 2)]
} //end of setSpotInitialoordinates
void setZombieInitialCoordinates(vector<Item>& zombies, const int zombieStartingX[], const int zombieStartingY[])
{ //set zombie coordinates inside the grid at random at beginning of game
	for (int i(0); i < MAXZOMBIES; ++i)
	{
		zombies[i].y = zombieStartingY[i];      //vertical coordinate
		zombies[i].x = zombieStartingX[i];    //horizontal coordinate
	}
}//end of setZombieInitialoordinates

void setPillInitialCoordinates(vector<Item>& pills, Player&spot, vector<Item>&zombies)
{ //set pill coordinates inside the grid at random at beginning of game
	for (int i(0); i < MAXPILLS; ++i)
	{
		pills[i].y = Random(SIZEY - 2);      //vertical coordinate in range [1..(SIZEY - 2)]
		pills[i].x = Random(SIZEX - 2);    //horizontal coordinate in range [1..(SIZEX - 2)]
		for (int z(0); z < MAXZOMBIES; ++z)
		{
			if (((pills[i].x == spot.x) && (pills[i].y == spot.y)) || ((pills[i].x == zombies[z].x) && (pills[i].y == zombies[z].y)))
			{
				--i;
			}
		}
	}
}//end of setPillInitialoordinates

void setHoleInitialCoordinates(vector<Item>& holes, Player&spot, vector<Item>& pills, vector<Item>&zombies)
{ //set hole coordinates inside the grid at random at beginning of game
	for (int i(0); i < MAXHOLES; ++i)
	{
		holes[i].y = Random(SIZEY - 2);      //vertical coordinate in range [1..(SIZEY - 2)]
		holes[i].x = Random(SIZEX - 2);    //horizontal coordinate in range [1..(SIZEX - 2)]
		for (int p(0); p < MAXPILLS; ++p)
		{
			for (int z(0); z < MAXZOMBIES; ++z)
			{
				if (((holes[i].x == spot.x) && (holes[i].y == spot.y)) || ((holes[i].x == pills[p].x) && (holes[i].y == pills[p].y)) || ((holes[i].x == zombies[z].x) && (holes[i].y == zombies[z].y)))
				{
					--i;
				}
			}
		}
	}
}//end of setHoleInitialoordinates

void setGrid(char grid[][SIZEX])
{ //reset the empty grid configuration
	for (int row(0); row < SIZEY; ++row) //for each column
	{
		for (int col(0); col < SIZEX; ++col) //for each col
		{
			if ((row == 0) || (row == SIZEY - 1))     //top and bottom walls
				grid[row][col] = WALL;                //draw a wall symbol
			else
				if ((col == 0) || (col == SIZEX - 1)) //left and right walls
					grid[row][col] = WALL;            //draw a wall symbol
				else
					grid[row][col] = TUNNEL;          //draw a space
		} //end of row-loop
	} //end of col-loop
} //end of setGrid

void placeSpot(char gr[][SIZEX], Player spot)
{ //place spot at its new position in grid
	gr[spot.y][spot.x] = spot.symbol;
} //end of placeSpot


void placeZombies(char gr[][SIZEX], vector<Item> zombies)
{ //place zombie at its new position in grid
	for (int i(0); i < MAXZOMBIES; ++i)
	{
		if (!(zombies[i].x == 0 && zombies[i].y == 0))
		{
			gr[zombies.at(i).y][zombies.at(i).x] = zombies.at(i).symbol;
		}
	}
} //end of placeZombies

void placePills(char gr[][SIZEX], vector<Item> pills)
{ //place pill at its new position in grid
	SelectTextColour(clRed);
	for (int i(0); i < MAXPILLS; ++i)
	{
		if (!(pills[i].x == 0 && pills[i].y == 0))
		{
			gr[pills.at(i).y][pills.at(i).x] = pills.at(i).symbol;
		}
	}
} //end of placePills

void placeHoles(char gr[][SIZEX], vector<Item> holes)
{ //place hole at its new position in grid
	SelectTextColour(clGrey);
	for (int i(0); i < MAXHOLES; ++i)
		gr[holes.at(i).y][holes.at(i).x] = holes.at(i).symbol;
} //end of placeHoles

//---------------------------------------------------------------------------
//----- update grid state
//---------------------------------------------------------------------------

void updateGrid(char grid[][SIZEX], Player& spot, vector <Item>& zombies, vector <Item>& holes, string& mess, vector <Item>& pills, const int zombieStartingX[], const int zombieStartingY[])
{ //update grid configuration after each move
	void setGrid(char[][SIZEX]);
	void placeSpot(char g[][SIZEX], Player spot);
	void placeZombies(char g[][SIZEX], vector <Item> zombies);
	void checkForZombieBites(vector<Item>& zombies, string& mess, Player& spot, const int zombieStartingX[], const int zombieStartingY[]);

	setGrid(grid);	         //reset empty grid
	placePills(grid, pills);	 //place pills in grid
	placeHoles(grid, holes);	 //place holes in grid
	placeSpot(grid, spot);	 //set spot in grid
	placeZombies(grid, zombies);	 //place zombies in grid
	checkForZombieBites(zombies, mess, spot, zombieStartingX, zombieStartingY);

} //end of updateGrid

//---------------------------------------------------------------------------
//----- move the spot
//---------------------------------------------------------------------------
void updateSpotCoordinates(const char g[][SIZEX], Player& sp, int key, string& mess, vector <Item>& pills, int& remainingPills)
{
	sp.powerCounter--;
	if (sp.powerCounter < 1)
	{
		sp.empowered = false;
	}
	//move spot in required direction
	void setKeyDirection(int k, int& dx, int& dy);

	//calculate direction of movement required by key - if any
	int dx(0), dy(0);
	setKeyDirection(key, dx, dy); 	//find direction indicated by key
	//check new target position in grid 
	//and update spot coordinates if move is possible
	const int targetY(sp.y + dy);
	const int targetX(sp.x + dx);
	switch (g[targetY][targetX])
	{		//...depending on what's on the target position in grid...
	case TUNNEL:      //can move
		sp.y += dy;   //go in that Y direction
		sp.x += dx;   //go in that X direction
		break;
	case WALL:        //hit a wall and stay there
		cout << '\a'; //beep the alarm
		mess = "Cannot go there!    ";
		break;
	case HOLE:
		sp.y += dy;
		sp.x += dx;
		mess = "Fallen in hole, LIFE LOST!    ";
		sp.currentScore -= 50;
		sp.lives--;
		break;
	case PILL:      //can move
		sp.y += dy;   //go in that Y direction
		sp.x += dx;   //go in that X direction
		for (int i(0); i < MAXPILLS; ++i)
		{
			if ((pills[i].x == sp.x) && (pills[i].y == sp.y))
			{
				pills[i].x = 0;
				pills[i].y = 0;
				sp.empowered = true;
				sp.powerCounter = 10;
				sp.lives++;
				sp.currentScore += 50;
				remainingPills--;
			}
		}
		break;
	}
} //end of updateSpotCoordinates

void checkForZombieBites(vector<Item>& zombies, string& mess, Player& spot, const int zombieStartingX[], const int zombieStartingY[])
{
	for (int i(0); i < MAXZOMBIES; ++i)
	{
		if ((zombies.at(i).y == spot.y) && (zombies.at(i).x == spot.x)&& (zombies.at(i).symbol==ZOMBIE))
		{
			mess = "Attacked by zombie, LIFE LOST!    ";
			spot.currentScore -= 100;
			spot.lives--;
			zombies[i].y = zombieStartingY[i];      //vertical coordinate
			zombies[i].x = zombieStartingX[i];    //horizontal coordinate
		}
	}
}

void updateZombieCoordinates(const char g[][SIZEX], vector<Item>& zombies, int key, string& mess, Player& spot, bool freezeZombies, int& remainingZombies, const int zombieStartingX[], const int zombieStartingY[])
{ 
	int dx(0), dy(0);
	if (freezeZombies == false)
	{
		for (int i(0); i < MAXZOMBIES; ++i)
		{
			if (!(zombies[i].x == 0 && zombies[i].y == 0))
			{
				if (spot.x > zombies.at(i).x)
					dx = 1;
				else
					if (spot.x < zombies.at(i).x)
						dx = -1;
					else
						dx = 0;
				if (spot.y > zombies.at(i).y)
					dy = 1;
				else
					if (spot.y < zombies.at(i).y)
						dy = -1;
					else
						dy = 0;
				if (spot.empowered == true)
				{
					dx = (dx*-1);
					dy = (dy*-1);
				}
			}
			const int targetY(zombies.at(i).y + dy);
			const int targetX(zombies.at(i).x + dx);
			switch (g[targetY][targetX])
			{		//...depending on what's on the target position in grid...
			case TUNNEL:      //can move
				zombies.at(i).y += dy;   //go in that Y direction
				zombies.at(i).x += dx;   //go in that X direction
				break;
			case WALL:        //hit a wall and stay there
				break;
			case HOLE:
				spot.currentScore += 100;
				zombies[i].y = 0;      //vertical coordinate
				zombies[i].x = 0;    //horizontal coordinate
				remainingZombies--;
				break;
			case PILL:      //can move
				zombies.at(i).y += dy;   //go in that Y direction
				zombies.at(i).x += dx;   //go in that X direction
				break;
			case ZOMBIE:
				zombies[i].y = zombieStartingY[i];      //vertical coordinate
				zombies[i].x = zombieStartingX[i];    //horizontal coordinate
				break;
			}
		}
	}
} //end of updateSpotCoordinates

//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(int key, int& dx, int& dy)
{ //
	switch (key)    //...depending on the selected key...
	{
	case LEFT:      //when LEFT arrow pressed...
		dx = -1;    //decrease the X coordinate
		dy = 0;
		break;
	case RIGHT:     //when RIGHT arrow pressed...
		dx = +1;    //increase the X coordinate
		dy = 0;
		break;
	case UP:      //when UP arrow pressed...
		dx = 0;   //decrease the Y coordinate
		dy = -1;  //will make player go up as y axis increaes as it goes DOWN the screen
		break;
	case DOWN:     //when DOWN arrow pressed...
		dx = 0;    //increase the Y coordinate
		dy = +1;
		break;
	}
} //end of setKeyDirection

int getKeyPress()
{ //get key or command selected by user
	int keyPressed;
	keyPressed = _getch();      //read in the selected arrow key or command letter
	while (keyPressed == 224)     //ignore symbol following cursor key
		keyPressed = _getch();
	return(keyPressed);   
} //end of getKeyPress

bool isArrowKey(int key)
{ //check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
	return ((key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN));
} //end of isArrowKey

bool cheatCheck(int key, int& remainingPills, int& remainingZombies, vector<Item>& zombies, vector<Item>& pills, bool& freezeZombies)
{//check if a cheat was pressed
	bool cheatValid;
	cheatValid = false;
	switch (key)    //...depending on the selected key...
	{
	case FREEZE:
		cheatValid = true;
		if (freezeZombies == true)
			freezeZombies = false;
		else
			freezeZombies = true;
		break;
	case EAT:
		cheatValid = true;
		for (int i = 0; i < MAXPILLS; ++i)
		{
			pills[i].symbol = TUNNEL;
			remainingPills--;
		}
		break;
	case EXTERMINATE:
		cheatValid = true;
		for (int i = 0; i < MAXZOMBIES; ++i)
		{
			zombies[i].x = 0;
			zombies[i].y = 0;
			remainingZombies--;
		}
		break;
	}
	return cheatValid;
}//end of cheatCheck

bool wantToQuit(int key)
{ //check if the key pressed is 'Q'
	return (key == QUIT);
} //end of wantToQuit

bool wantToPlay(int key)
{ //check if the key pressed is 'P'
	return (key == START);
} //end of wantToPlay


//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------
void clearMessage()
{ //clear message area on screen
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(40, 8);
	string str(20, ' ');
	cout << str;  //display blank message
	Gotoxy(40, 9);
	cout << str;  //display blank message
	Gotoxy(40, 10);
	cout << str;  //display blank message
	Gotoxy(40, 11);
	cout << str;  //display blank message

} //end of setMessage

void renderGame(const char gd[][SIZEX], string mess, bool playing, Player spot, int previousScore)
{ //display game title, messages, maze, spot and apples on screen
	void paintGrid(const char g[][SIZEX]);
	void showTitle();
	void showOptions();
	void showMessage(string m, bool playing, Player spot, int previousScore);

	Gotoxy(0, 0);
	//display grid contents
	paintGrid(gd);
	//display game title
	showTitle();
	//display menu options available
	showOptions();
	//display message if any
	showMessage(mess, playing, spot, previousScore);
} //end of paintGame

void paintGrid(const char g[][SIZEX])
{ //display grid content on screen
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(0, 2);
	for (int row(0); row < SIZEY; ++row)      //for each row (vertically)
	{
		for (int col(0); col < SIZEX; ++col)  //for each column (horizontally)
		{
			cout << g[row][col];              //output cell content
		} //end of col-loop
		cout << endl;
	} //end of row-loop
} //end of paintGrid

void showTitle()
{ //display game title
	string GetTime();
	string GetDate();
	SelectTextColour(clYellow);
	Gotoxy(0, 0);
	cout << "___ZOMBIES GAME BASIC VERSION___\n" << endl;
	SelectTextColour(clRed);
	Gotoxy(40, 0);
	cout << "The time is "<< GetTime() << "\n                                        and the date is " <<GetDate();
} //end of showTitle


void showOptions()
{ //show game options
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 5);
	cout << "TO MOVE USE KEYBOARD ARROWS  ";
	Gotoxy(40, 6);
	cout << "TO QUIT ENTER 'Q'   ";
} //end of showOptions

void showInfo(int key)
{ //show game options
	while (!(key == 13))
	{
		Gotoxy(0, 0);
		for (int i(0); i < screenWidth(); ++i)
		{
			for (int ii(0); ii < screenWidth(); ++ii)
				cout << TUNNEL;
		} // clear the screen
		SelectBackColour(clRed);
		SelectTextColour(clBlue);
		Gotoxy(0, 0);
		cout << "TO MOVE USE KEYBOARD ARROWS \nTO QUIT ENTER 'Q'\nCOLLECT ALL PILLS ':' OR\n KILL ALL ZOMBIES 'Z' TO WIN!\nZOMBIES DIE IF THEY FALL INTO HOLES 'O'\nBUT SO DO YOU!\n";
		cout << " ZOMBIE KILLS EARN 100 POINTS AND PILLS EARN 50\n FALLING IN HOLES WILL LOSE YOU 50 POINTS AND BEING EATEN WILL LOSE YOU 100\n YOU WILL ALSO LOSE A LIFE IN BOTH CASES\n EAT PILLS TO EARN MORE LIVES!";
		SelectBackColour(clBlue);
		SelectTextColour(clYellow);
		cout << "\nPress enter to return...";
		SelectBackColour(clBlack);
		SelectTextColour(clWhite);
		key = getKeyPress();
	}
} //end of showOptions

void showMessage(string m, bool playing, Player spot, int previousScore)
{ //print auxiliary messages if any
	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(40, 8);
	cout << m;	//display current message
	if (playing == true)
	{
		Gotoxy(40, 9);
		cout << spot.lives << " lives left!";
		Gotoxy(40, 10);
		cout << spot.currentScore << " points!";
		Gotoxy(40, 11);
		cout <<"Previous Best: "<< previousScore << " points!";
	}
} //end of showMessage

void saveLoadScore(int& previousScore,string& playerName, Player spot)
{
	ifstream inputFile;
	ofstream outputFile;
	inputFile.open(playerName.c_str(), ios::in);
	if (inputFile.fail())
	{
		outputFile.open(playerName.c_str(), ios::out);
		outputFile << spot.currentScore;
		outputFile.close();
		previousScore = -1;
	}
	else
	{
		string previousScoreString;
		getline(inputFile, previousScoreString);
		previousScore = atoi(previousScoreString.c_str());
		inputFile.close();
		if (previousScore < spot.currentScore)
		{
			outputFile.open(playerName.c_str(), ios::out);
			outputFile << spot.currentScore;
			outputFile.close();
		}

	}
}

void endProgram(Player& spot, int remainingPills, int remainingZombies)
{ //end program with appropriate message
	SelectBackColour(clBlack);
	SelectTextColour(clYellow);
	clearMessage();
	Gotoxy(40, 8);
	if (spot.lives < 1)
		cout << "GAME OVER!          ";
	else
		if (remainingPills == 0)
			cout << "Congratulations! YOU WIN!          ";
		else
			if (remainingZombies == 0)
			{
		cout << "Congratulations!"; 
		Gotoxy(40, 9);
		cout <<"YOU WON WITH " << remainingPills << " PILLS REMAINING!";
			}
			else
			cout << "PLAYER QUITS!          ";			
		
} //end of endProgram
