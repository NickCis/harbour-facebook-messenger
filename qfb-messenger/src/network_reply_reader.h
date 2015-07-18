#ifndef __NETWORK_REPLY_READER_H__
#define __NETWORK_REPLY_READER_H__

#include <QObject>
#include <QNetworkReply>
#include "network_reply_handler.h"

class NetworkReplyReader : public NetworkReplyHandler {
		Q_OBJECT

	public:
		explicit NetworkReplyReader(QNetworkReply* reply);

	signals:
		void readLine(const QByteArray& data);
		void finished();

	protected:
		void emitAllLines();

	protected slots:
		void readyRead();
		void replyFinished();
};

#endif
