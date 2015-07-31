#ifndef __DUMMY_STORAGE_H__
#define __DUMMY_STORAGE_H__

#include "qfb-storage.h"

class DummyStorage : public QFbStorage {
	Q_OBJECT

	public:
		DummyStorage(QObject* parent=0);
		~DummyStorage();

		QHash<QString, QString> getConfigurationValues();
		void setConfigurationValue(const QString& name, const QString& value);
		void setAccount(const QString& email);

	protected:
		void readAccount();
		QHash<QString, QString> configValues;
		QString account;
};

#endif
