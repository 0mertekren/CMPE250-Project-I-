
//============================================================================
// Name        : main.cpp
// Author      : mekren
// Description : Project1 cmpe250 2020
//============================================================================

#include <iostream>
#include "Character.h"
#include <fstream>
#include <string>

#define NUM_OF_TEAMS   2
#define NUM_OF_MEMBERS_IN_TEAM   5
#define NUM_OF_CHARS   (NUM_OF_MEMBERS_IN_TEAM*NUM_OF_TEAMS)

using namespace std;

struct fightscenario{
	unsigned char attackerIndex;
	unsigned char defenderIndex;
	bool use_of_special_force;
	bool temp;// for structure alignment
};



enum result{
	HOBBIT_DIED,
	OTHER_MEMBERS_DIED,
	CONTINUE_WAR
};
/**GLOBAL VARIABLES*/
int maximumRounds;
bool team_index[NUM_OF_CHARS];
int sortedTeamIndex[NUM_OF_TEAMS][NUM_OF_MEMBERS_IN_TEAM];
int winner = -1;
int numofCausalties=0;
int lastRounds;
int damage = 0 ;
int defenseforce = 0;
int team0KilledIndex = INT16_MAX;
int team1KilledIndex = INT16_MAX;
/*FUNC PROTO*/
bool freadAndInitChars(Character **&chars, struct fightscenario *&fights, char **argv);
int war(Character **&chars, struct fightscenario *fights);
void sortCharNames(Character **chars);
int findTheFighter(Character **&chars, struct fightscenario *fights, int round, bool fightingPosition);
int checkStatus(Character **&chars, int team, int round);
int printResults(Character **chars, char **argv);
void execSpecialSkills(Character **&chars, int charindex);
void checkWizard(Character **&chars, int charindex);


int main(int argc, char** argv ) {

	if(argc!=3)
	{
		cout << "enter right commands/give full dir";
		return false;
	}

	Character **chars;
	struct fightscenario *fights;
	chars = new Character*[NUM_OF_CHARS];

	if(!freadAndInitChars(chars, fights, argv))
		return true;

	sortCharNames(chars);

	if(war(chars,fights))
	{}

	if(!printResults(chars, argv))
		return false;

	delete[] fights;
	delete[] chars;				 

	return true;
}

int war(Character **&chars, struct fightscenario *fights)
{
	int round, team;
	bool fightingPosition;
	int attackerIndex , defenderIndex;


	for(round=1; round <= chars[0]->nMaxRounds; round++)
	{
		//select the eligible fighters
		attackerIndex = findTheFighter(chars, fights, round-1, false);
		defenderIndex = findTheFighter(chars, fights, round-1, true);

		if(defenderIndex == INT16_MAX || attackerIndex == INT16_MAX )
			return false; // error case

		damage = chars[attackerIndex]->attack - chars[defenderIndex]->defense;

		if(fights[round-1].use_of_special_force == true)
			checkWizard(chars, attackerIndex);

		if(chars[attackerIndex]->type.compare("Elves") == 0)
		{
			if(fights[round-1].use_of_special_force == true)
				execSpecialSkills(chars, attackerIndex);
		}

		if(damage>=0)
		{
			if(chars[attackerIndex]->type.compare("Dwarfs") == 0)
			{
				if(fights[round-1].use_of_special_force == true)
					execSpecialSkills(chars, attackerIndex);
			}

			chars[defenderIndex]->remainingHealth -= damage;
			if(chars[defenderIndex]->remainingHealth <= 0)
			{
				chars[defenderIndex]->remainingHealth = 0;
				chars[defenderIndex]->isAlive = false;
				numofCausalties++;
				if(defenderIndex/NUM_OF_MEMBERS_IN_TEAM == 0)
					team0KilledIndex = defenderIndex;
				else
					team1KilledIndex = defenderIndex;
			}
		}

		for(team=0; team < NUM_OF_TEAMS; team++)
		{
			if(checkStatus(chars, team, round) != CONTINUE_WAR)
			{
				winner = NUM_OF_TEAMS-team-1;//
				lastRounds = round;
				return false;
			}
		}
	}
	return true;
}

int printResults(Character **chars, char **argv)
{
	int i,j;
	ofstream myfile;
	myfile.open (argv[2]);
	if(winner == -1)
		myfile << "Draw\n";
	else if (winner == 0)
		myfile << "Community-1\n";
	else if (winner == 1)
		myfile << "Community-2\n";
	else
		myfile << "winner is wrong\n";

	myfile << (lastRounds) << "\n";
	myfile << (numofCausalties) << "\n";

	for(i = 0; i<NUM_OF_CHARS ; i++)
	{
		myfile << chars[i]->name << " ";

		for(j=0; j<=lastRounds; j++)
		{
			myfile << chars[i]->healthHistory[j] << " ";
		}
		myfile << "\n";
	}

	myfile.close();
	return true;

}

void sortCharNames(Character **chars)
{
	int i,j,k;
	string names[NUM_OF_CHARS];
	string temp;

	for (i = 0; i < NUM_OF_CHARS; i++)
	{
		sortedTeamIndex[i/NUM_OF_MEMBERS_IN_TEAM][i%NUM_OF_MEMBERS_IN_TEAM] = i;
		names[i].assign(chars[i]->name);
	}

	for (i=0; i<NUM_OF_TEAMS; i++){
		for(k=0; k<NUM_OF_MEMBERS_IN_TEAM; k++){
			for (j = 0; j < NUM_OF_MEMBERS_IN_TEAM-k-1; j++){
				if(names[j+i*NUM_OF_MEMBERS_IN_TEAM].compare(names[j+1+i*NUM_OF_MEMBERS_IN_TEAM]) > 0){
					swap(sortedTeamIndex[i][j],sortedTeamIndex[i][j+1]);
					swap(names[j+i*NUM_OF_MEMBERS_IN_TEAM],names[j+1+i*NUM_OF_MEMBERS_IN_TEAM]);
				}
			}
		}
	}
}


int checkStatus(Character **&chars, int team, int round)
{
	int i,j;
	bool anyAlive=false;

	for(i=0; i<NUM_OF_CHARS; i++)
		chars[i]->healthHistory[round] = chars[i]->remainingHealth;

	for(i=team*NUM_OF_MEMBERS_IN_TEAM; i<(team+1)*NUM_OF_MEMBERS_IN_TEAM; i++){
		chars[i]->nRoundsSinceSpecial++;
		if(chars[i]->type.compare("Hobbit") == 0) {
			if(chars[i]->isAlive == false)
				return HOBBIT_DIED;
		}

		else {
			if(chars[i]->isAlive == true)
				anyAlive = true;
		}
	}
	if(anyAlive == false)
		return OTHER_MEMBERS_DIED;
	if(anyAlive == true)
		return CONTINUE_WAR;

}

int findTheFighter(Character **&chars, struct fightscenario *fights, int round, bool fightingPosition)
{
	int charindex;
	int i,j;
	int k, l;
	if(fightingPosition == 0)
		charindex = fights[round].attackerIndex;
	else
		charindex = fights[round].defenderIndex;

	bool reachedEndFlag = false;

	for(i=0;i< NUM_OF_TEAMS; i++)
	{
		for(j=0; j< NUM_OF_MEMBERS_IN_TEAM; j++)
		{
			if(charindex == sortedTeamIndex[i][j])
			{
				k = i;
				l = j;
				break;
			}
		}
	}

	while(1)
	{
		if(l<0)
			return INT16_MAX;

		if(l%NUM_OF_MEMBERS_IN_TEAM == NUM_OF_MEMBERS_IN_TEAM-1)
			reachedEndFlag = true;

		if(chars[sortedTeamIndex[k][l%NUM_OF_MEMBERS_IN_TEAM]]->isAlive == true)
			break;

		else
		{
			if(reachedEndFlag == false)
			{
				l++;
				continue;
			}
			if(reachedEndFlag == true)
			{
				if(sortedTeamIndex[i][j]>charindex)
					l = charindex%NUM_OF_MEMBERS_IN_TEAM - 1;
				else
					l--;
			}
		}
	}
	return sortedTeamIndex[k][l];
}

bool freadAndInitChars(Character **&chars, struct fightscenario *&fights, char **argv)
{
	ifstream file(argv[1]);
	string name, type;
	string fighter1, fighter2, use_of_special_skill;
	int attack, defense, health, i, j, n;

	if(file.is_open()){
		if(file >> n){
			fights = new struct fightscenario[n];
			maximumRounds = n;
			lastRounds = n;
		}
		for(i=0; i<NUM_OF_CHARS; i++){
			//team_index[NUM_OF_CHARS] = i % NUM_OF_MEMBERS_IN_TEAM;// 0 if team 0, 1 if team 1...
			if(file >> name >> type >> attack >> defense >> health ){
				chars[i] = new Character(name, type, attack, defense, health, n);
			}
		}
		for(i=0; i<n; i++){
			if(file >> fighter1 >> fighter2 >> use_of_special_skill){
				for(j=0; j<NUM_OF_CHARS; j++)
				{
					if(fighter1.compare(chars[j]->name) == 0){
						fights[i].attackerIndex = j;
					}
					if(fighter2.compare(chars[j]->name) == 0){
						fights[i].defenderIndex = j;
					}
					if(use_of_special_skill.compare("SPECIAL") == 0)
						fights[i].use_of_special_force = true;
					else if(use_of_special_skill.compare("NO-SPECIAL") == 0)
						fights[i].use_of_special_force = false;
				}
			}
		}
		file.close();
	}
	else
	{
		cout << "File couldn't be opened.\n";
		return false;
	}

	return true;
}
void execSpecialSkills(Character **&chars, int charindex)
{
	int i;
	int team = charindex/NUM_OF_MEMBERS_IN_TEAM;
	if(chars[charindex]->type.compare("Elves") == 0)
	{
		if(chars[charindex]->nRoundsSinceSpecial >= 10)
		{
			chars[charindex]->nRoundsSinceSpecial = 0;

			for(i=team*NUM_OF_MEMBERS_IN_TEAM; i<(team+1)*NUM_OF_MEMBERS_IN_TEAM; i++)
			{
				if(chars[i]->type.compare("Hobbit") == 0)
				{
					chars[i]->remainingHealth += chars[charindex]->remainingHealth/2;//elf heals hobbit
					chars[charindex]->remainingHealth -= chars[charindex]->remainingHealth/ 2;
					break;
				}
			}
		}
	}
	else if(chars[charindex]->type.compare("Dwarfs") == 0)
	{
		if(chars[charindex]->nRoundsSinceSpecial >= 20)
		{
			chars[charindex]->nRoundsSinceSpecial = 0;
			damage *= 2;
		}
	}

	else
	{
		//not suitable for special skills
	}
}

void checkWizard(Character **&chars, int charindex)
{
	if(chars[charindex]->type.compare("Wizards") == 0){
		if(chars[charindex]->nRoundsSinceSpecial >= 50){
			if(charindex/NUM_OF_MEMBERS_IN_TEAM == 0){
				if(team0KilledIndex != INT16_MAX){
					chars[team0KilledIndex]->isAlive = true;
					chars[team0KilledIndex]->remainingHealth = chars[team0KilledIndex]->healthHistory[0];
					chars[charindex]->nRoundsSinceSpecial = 0;
					numofCausalties--;
				}
			}
			if(charindex/NUM_OF_MEMBERS_IN_TEAM == 1)
			{
				if(team1KilledIndex != INT16_MAX)
				{
					chars[team1KilledIndex]->isAlive = true;
					chars[team1KilledIndex]->remainingHealth = chars[team1KilledIndex]->healthHistory[0];
					chars[charindex]->nRoundsSinceSpecial = 0;
					numofCausalties--;
				}
			}
		}
	}
}
