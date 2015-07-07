#include <QCoreApplication>
#include "console.h"

int main(int argc, char* argv[]){
	QCoreApplication app(argc, argv);
	Console console;
	console.run();
	QObject::connect(&console, SIGNAL(quit()), &app, SLOT(quit()));
	return app.exec();
}
