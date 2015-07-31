#ifndef __FB_CLIENT_H__
#define __FB_CLIENT_H__

#include "console.h"
#include "qfb-messenger.h"
#include "dummy_storage.h"

class FbClient : public Console {
	Q_OBJECT

	public:
		FbClient(QObject* parent=NULL);

	protected:
		DummyStorage dummyStorage;
		QFbMessenger qfb;
		QString email;

		void executeCommand(const std::string& line);
		void input(const std::string& line);

	private:
		typedef enum {
			Default=0,
			Login
		} Status;

		FbClient::Status status;

	protected slots:
		void qfbLoginResponse(bool error, QString desc);
		void qfbConnectedChanged(bool connected);
};

#endif
