#include <QCoreApplication>
#include <QtGlobal>
#include "fb_client.h"


int main(int argc, char* argv[]){
	qSetMessagePattern("%{time} %{file}(%{line}) :: %{function} :: %{message}");
	QCoreApplication app(argc, argv);
	FbClient console;
	console.run();
	QObject::connect(&console, SIGNAL(quit()), &app, SLOT(quit()));
	return app.exec();
}
