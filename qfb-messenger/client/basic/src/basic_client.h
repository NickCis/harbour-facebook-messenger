#ifndef __BASIC_CLIENT_H__
#define __BASIC_CLIENT_H__

#include "console.h"
#include "qfb-network.h"

class BasicClient : public Console {
	Q_OBJECT

	public:
		BasicClient(QObject* parent = NULL);

	protected:
		QFbNetwork qfb;
		QString email;

		QJsonArray friendsList;
		QJsonArray conversations;
		QJsonObject profiles;
		void executeCommand(const std::string& line);
		void input(const std::string& line);

	private:
		typedef enum {
			Default=0,
			Login
		} Status;

		BasicClient::Status status;

	protected slots:
		void initResponse(bool error, QString desc);
		void loginResponse(bool error, QString desc);
		void getBasicInformationResponse(bool error, QJsonValue data);
		void getUserInfoResponse(bool error, QJsonValue data);
		void getThreadInfoResponse(bool error, QJsonValue data);
		void sendMessagesResponse(bool error, QJsonValue data);
		void pullResponse(bool error, QJsonValue data);

		std::string getUsername(const QString& fbid);
};

#endif
