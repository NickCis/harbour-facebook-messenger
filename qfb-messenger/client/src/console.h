#ifndef __CONSOLE_H__
#define __CONSOLE_H__
#include <QObject>
#include <QSocketNotifier>
#include <string>
#include "qfb-messenger.h"

class Console : public QObject {
	Q_OBJECT

	public:
		Console();
		void run();

	signals:
		void quit();

	private:
		typedef enum {
			DEFAULT=0,
			LOGIN
		} Status;

		QFbMessenger qfb;
		QSocketNotifier *notifier;
		QString email;
		Console::Status status;

		void executeCommand(const std::string&);
		void statusCommand(const std::string&);

		QJsonArray friendsList;
		QJsonArray conversations;
		QJsonObject profiles;

	protected slots:
		void loginResponse(bool error, QString desc);
		void getBasicInformationResponse(bool error, QJsonValue data);
		void getUserInfoResponse(bool error, QJsonValue data);

	private slots:
		void readCommand();

	protected:
		void hideStdinKeyStrokes();
		void showStdinKeyStrokes();
};
#endif
