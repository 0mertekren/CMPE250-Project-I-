#include "Character.h"
#include <string>
#include <iostream>

Character::Character(string _name, string _type, int _attack, int _defense, int _remainingHealth, int _nMaxRounds) {
	this->name.assign(_name);
	this->type.assign(_type);
	this->attack = _attack;
	this->defense = _defense;
	this->remainingHealth = _remainingHealth;
	this->nMaxRounds = _nMaxRounds;
	this->nRoundsSinceSpecial = 0;
	healthHistory = new int[_nMaxRounds+1];
	healthHistory[0] = _remainingHealth;
}

Character::Character(const Character& character) {

}

Character Character::operator=(const Character& character) {

}

bool Character::operator<(const Character& other) {
	cout << "here";
	/*if(attack > this->defense)
	{
		cout << "htere";
		this->remainingHealth = attack - this->defense;
		if(this->remainingHealth <= 0)
		{
			this->remainingHealth = 0;
			this->isAlive = false;
		}
		return true;
	}

	else
		return false;*/

}

Character::~Character() {
	delete[] healthHistory;

}
