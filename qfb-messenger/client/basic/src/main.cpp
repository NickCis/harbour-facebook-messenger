#include <QCoreApplication>
#include <QtGlobal>
#include "basic_client.h"


int main(int argc, char* argv[]){
	qSetMessagePattern("%{time} %{file}(%{line}) :: %{function} :: %{message}");
	QCoreApplication app(argc, argv);
	BasicClient console;
	console.run();
	QObject::connect(&console, SIGNAL(quit()), &app, SLOT(quit()));
	return app.exec();
}
