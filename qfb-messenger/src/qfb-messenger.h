#ifndef __QFB_MESSENGER_H__
#define __QFB_MESSENGER_H__

#include <QObject>
#include <QVariant>
#include <QSettings>
#include <QJsonArray>
#include <QJsonObject>
#include <QByteArray>
#include <QNetworkReply>

#include "network_manager.h"

class QFbMessenger : public QObject {
		Q_OBJECT

	public:
		explicit QFbMessenger(QObject *parent = 0);

		/** Initialices facebook variables (and cookies):
		 *   * requestId
		 *   * identifier
		 *   * jsDatr
		 *   * lsd
		 */
		Q_INVOKABLE void init();

		/** Performs Login, init must be called previously
		 * @param email: fb email
		 * @param pass: fb password
		 */
		Q_INVOKABLE void login(const QString &email, const QString &pass);

		/** Gets lasts conversations (MessengerMount) and friend list (InitialChatFriendsList)
		 * y DTSGInitialData
		 */
		Q_INVOKABLE void getBasicInformation();

		Q_INVOKABLE void getUserInfo(const QJsonArray& ids);

		Q_INVOKABLE void getThreadInfo(const QString& id, int offset=0, int limit=20);

		Q_INVOKABLE void sendMessages(const QString& id, const QString& msg);

		Q_INVOKABLE void pull();

	signals:
		void initResponse(bool error, QString desc);
		void loginResponse(bool error, QString desc);
		
		/** Data: {
		 * conversations: []
		 * friendsList: []
		 * }
		 */
		void getBasicInformationResponse(bool error, QJsonValue data);

		/**
		 * data: { <user id>: {}, ... }
		 */
		void getUserInfoResponse(bool error, QJsonValue data);

		void getThreadInfoResponse(bool error, QJsonValue data);

		void sendMessagesResponse(bool error, QJsonValue data);

		void pullResponse(bool error, QJsonValue data);

	protected slots:
		void initFinished(QNetworkReply::NetworkError, const QByteArray&);
		void loginFinished(QNetworkReply::NetworkError, const QByteArray&);
		void getBasicInformationFinished(QNetworkReply::NetworkError, const QByteArray&);
		void getUserInfoFinished(QNetworkReply::NetworkError, const QByteArray&);
		void getThreadInfoFinished(QNetworkReply::NetworkError, const QByteArray&);
		void sendMessagesFinished(QNetworkReply::NetworkError, const QByteArray&);
		void pullMessage(const QByteArray&);
		void pullFinished();

	protected:
		NetworkManager man;

		QString getMatch(const QString& regexp, const QString& text);
		void getRequestId(const QString&);
		void getIdentifier(const QString&);
		void getJsDatr(const QString&);
		void getLsd(const QString&);

		QJsonArray parseLastConversations(const QString& response);
		QJsonArray parseInitialChatFriendsList(const QString& response);
		void parseDtsg(const QString& response);

		QJsonValue parseProfiles(const QString& response);
		QJsonValue parseThread(const QString& response);

		void getUserIdFromCookies();

		QString generateCb();
		QString generateSessionId();

		QString requestId;
		QString identifier;
		QString jsDatr;
		QString lsd;
		QString dtsg;

		QString email;
		QString userId;

		QString pullSeq;
		QString sessionId;
		QString stickyToken;
		QString stickyPool;
};

#endif
