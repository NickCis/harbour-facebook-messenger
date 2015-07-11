#include "console.h"
#include <iostream>

#include <stdio.h>
#include <termios.h>

using namespace std;

Console::Console() :
	title("Console"),
	commandLine("> "),
	status(Console::Default)
{
	this->notifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
}

void Console::run() {
	cout << this->title << endl;
	cout << this->commandLine << flush;
	connect(notifier, SIGNAL(activated(int)), this, SLOT(readCommand()));
}

void Console::readCommand() {
	string line;
	getline(cin, line);
	switch(this->status){
		case Console::Default:
			this->executeCommand(line);
			break;

		default:
			this->input(line);
			break;
	}

	if(this->status == Console::Default)
		cout << this->commandLine << flush;
	else
		cout << flush;
}

void Console::hideStdinKeyStrokes(){
	termios tty;
	tcgetattr(fileno(stdin), &tty);
	tty.c_lflag &= ~ECHO;
	tcsetattr(fileno(stdin), TCSANOW, &tty);
}

void Console::showStdinKeyStrokes(){
	termios tty;
	tcgetattr(fileno(stdin), &tty);
	tty.c_lflag |= ECHO;
	tcsetattr(fileno(stdin), TCSANOW, &tty);
}

void Console::setStatus(Console::Status s){
	if(this->status != s && s == Console::Default)
		cout << this->commandLine << flush;

	this->status = s;
}
