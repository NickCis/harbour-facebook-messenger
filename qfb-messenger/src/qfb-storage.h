#ifndef __QFB_STORAGE_H__
#define __QFB_STORAGE_H__

#include <QHash>
#include <QString>
#include <QObject>

/** Interface to provide a storage for fb variables, messages and users.
 * This interface is used for the qfb-messenger facade
 */
class QFbStorage : public QObject {
	Q_OBJECT

	public:
		QFbStorage(QObject* parent) : QObject(parent){}
		virtual QHash<QString, QString> getConfigurationValues() = 0;
		virtual void setConfigurationValue(const QString& name, const QString& value) = 0;
		virtual void setAccount(const QString& email) = 0;
};

#endif
