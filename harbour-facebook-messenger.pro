# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-facebook-messenger

CONFIG += sailfishapp

SOURCES += src/harbour-facebook-messenger.cpp

OTHER_FILES += qml/harbour-facebook-messenger.qml \
    qml/cover/CoverPage.qml \
    rpm/harbour-facebook-messenger.changes.in \
    rpm/harbour-facebook-messenger.spec \
    rpm/harbour-facebook-messenger.yaml \
    harbour-facebook-messenger.desktop \
    qml/pages/MessengerPage.qml \
    qml/pages/helper/console.js \
    qml/pages/helper/messenger.js \
    qml/pages/helper/messenger.css

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
