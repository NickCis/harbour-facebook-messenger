#include "console.h"
#include <iostream>
#include <QString>

#include <termios.h>

using namespace std;

Console::Console() : status(Console::DEFAULT){
	this->notifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
}

void Console::run() {
	cout << "QFb Messenger Client" << endl;
	cout << "> " << flush;
	connect(notifier, SIGNAL(activated(int)), this, SLOT(readCommand()));
}

void Console::readCommand() {
	string line;
	getline(cin, line);
	switch(this->status){
		case Console::DEFAULT:
			this->executeCommand(line);
			break;

		default:
			this->statusCommand(line);
			break;
	}
}

void Console::statusCommand(const string& line) {
	switch(this->status){
		case Console::LOGIN:
			this->status = Console::DEFAULT;
			this->showStdinKeyStrokes();
			this->qfb.login(this->email, QString(line.c_str()));
			break;

		case Console::DEFAULT:
		default:
			break;
	}

	cout << endl << "> " << flush;
}

void Console::executeCommand(const string& line) {
	QString qline(line.c_str());
	if(cin.eof() || line == "quit"){
		cout << "Good bye!" << endl;
		emit quit();
		return;
	}else if(line == "init"){
		qfb.init();
	}else if(qline.startsWith("login")){
		QStringList list = qline.split(" ");
		if(list.count() < 2)
			cout << "login <email>" << endl;
		else{
			this->email = list[1];
			status = Console::LOGIN;
			cout << "password (won't be echoed): ";
			this->hideStdinKeyStrokes();
			cout << flush;
			return;
		}
	}else if(qline.startsWith("getConversations")){
		qfb.getConversations();
	}

	cout << "> " << flush;
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
