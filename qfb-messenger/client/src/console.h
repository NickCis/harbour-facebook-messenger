#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <QObject>
#include <QSocketNotifier>
#include <string>

class Console : public QObject {
	Q_OBJECT

	public:
		Console();
		virtual void run();

	signals:
		void quit();

	private slots:
		void readCommand();

	protected:
		typedef enum {
			Default=0,
			Input
		} Status;

		virtual void executeCommand(const std::string&) = 0;
		virtual void input(const std::string&) = 0;
		void hideStdinKeyStrokes();
		void showStdinKeyStrokes();
		std::string title;
		std::string commandLine;
		void setStatus(Console::Status);

	private:
		QSocketNotifier *notifier;
		Console::Status status;

};
#endif
