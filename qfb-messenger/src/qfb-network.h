#ifndef __QFB_NETWORK_H__
#define __QFB_NETWORK_H__

#include <QObject>
#include <QVariant>
#include <QSettings>
#include <QJsonArray>
#include <QJsonObject>
#include <QByteArray>
#include <QNetworkReply>

#include "network_manager.h"

/**
 * The responsability of this class is to make all network related duties.
 * It has a minimum logic to determine if a request has failed, in addition, it stores
 * the data needed in order to continue doing requests.
 */
class QFbNetwork : public QObject {
		Q_OBJECT

	public:
		QFbNetwork(QObject *parent = 0);

		/** Initialices facebook variables (and cookies):
		 *   * requestId
		 *   * identifier
		 *   * jsDatr -> this is going to be a cookie (datr)
		 *   * lsd
		 */
		void init();

		/** Performs Login, init must be called previously.
		 * init has to be called before!.
		 * This method sets the cookies ( datr, c_user, xs, csm, s, lu )
		 * @param email: fb email
		 * @param pass: fb password
		 */
		void login(const QString &email, const QString &pass);

		/** Gets lasts conversations (MessengerMount) and friend list (InitialChatFriendsList)
		 * y DTSGInitialData.
		 * In order to work, login has to be called before. It needs the cookies already setted: datr, c_user, xs, csm, s, lu.
		 *
		 * This method provides contact list (only user id) and recent coversations.
		 * It sets the `dtsg` value
		 */
		void getBasicInformation();

		void getUserInfo(const QJsonArray& ids);

		void getThreadInfo(const QString& id, int offset=0, int limit=20);

		void sendMessages(const QString& id, const QString& msg);

		void pull();

		/** 
		 * @return true: if it is setted correctly, false if not
		 */
		bool setConfigurationValue(const QString& name, const QString& value);

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
		void pullEnd();

		/** Signaled when a configuration value (requestId, identifier, datr, lsd, c_user, xs, csm, s, lu, dtsg) is created
		 */
		void newConfigurationValue(const QString& name, const QString& value);

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

		QString generateCb();
		QString generateSessionId();

		QString requestId;
		QString identifier;
		QString lsd;
		QString dtsg;

		QString userId;

		QString pullSeq;
		QString sessionId;
		QString stickyToken;
		QString stickyPool;
};

#endif
