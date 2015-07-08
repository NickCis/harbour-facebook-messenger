#ifndef __QFB_MESSENGER_H__
#define __QFB_MESSENGER_H__

#include <QObject>
#include <QUrl>
#include <QUrlQuery>
#include <QVariant>
#include <QSettings>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QJsonObject>

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

	protected slots:
		void initFinished(QObject* o);
		void loginFinished(QObject* o);
		void getBasicInformationFinished(QObject* o);
		void getUserInfoFinished(QObject* o);

	protected:
		enum RequestMethod {
			Get = 0,
			Post,
			Delete
		};

		QNetworkAccessManager man;

		QNetworkReply* createRequest(const QString& node);
		QNetworkReply* createRequest(const QString& node, QUrlQuery* query, QFbMessenger::RequestMethod method);

		QString getMatch(const QString& regexp, const QString& text);
		void getRequestId(const QString&);
		void getIdentifier(const QString&);
		void getJsDatr(const QString&);
		void getLsd(const QString&);

		QJsonArray parseLastConversations(const QString& response);
		QJsonArray parseInitialChatFriendsList(const QString& response);
		void parseDtsg(const QString& response);

		QJsonValue parseProfiles(const QString& response);

		QString requestId;
		QString identifier;
		QString jsDatr;
		QString lsd;
		QString dtsg;

		QString email;
};

#endif
