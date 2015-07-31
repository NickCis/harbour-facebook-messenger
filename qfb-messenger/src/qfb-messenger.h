#ifndef __QFB_MESSENGER_H__
#define __QFB_MESSENGER_H__

#include <QObject>
#include <QVariant>
#include <QSettings>
#include <QJsonArray>
#include <QJsonObject>
#include <QByteArray>

#include "qfb-network.h"
#include "qfb-storage.h"

/** This class provides a nice facade to use QFb in QML projects.
 * The idea is to export this class to the QML instance.
 */
class QFbMessenger : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged);

	public:
		explicit QFbMessenger(QFbStorage*, QObject *parent = 0);

		/** Connects using the stored data
		 */
		Q_INVOKABLE void login();

		/** Connects using the provided information
		 */
		Q_INVOKABLE void login(const QString &email, const QString &pass);

		bool isConnected() const;

	signals:
		/** Signal triggered when the login phase ended
		 */
		void loginResponse(bool error, QString desc);
		void connectedChanged(bool connected);

	protected slots:
		void networkInitAnswer(bool error, QString desc);
		void networkLoginAnswer(bool error, QString desc);
		void networkGetBasicInformationAnswer(bool error, QJsonValue data);
		void setConnected(bool);
		void networkNewConfigurationValue(const QString&, const QString&);

	protected:
		QFbStorage* storage;
		QFbNetwork network;
		QString email;
		QString pass;
		bool connected;
};


#endif
