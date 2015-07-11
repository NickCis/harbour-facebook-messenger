#include <QCoreApplication>
#include "basic_client.h"

int main(int argc, char* argv[]){
	QCoreApplication app(argc, argv);
	BasicClient console;
	console.run();
	QObject::connect(&console, SIGNAL(quit()), &app, SLOT(quit()));
	return app.exec();
}
