import QtQuick 2.0
import Sailfish.Silica 1.0

import QtWebKit 3.0
import QtWebKit.experimental 1.0

Page {
    id: page
    SilicaWebView {
        url: "https://www.messenger.com/login"
        anchors.fill: parent
        overridePageStackNavigation: true
        experimental.preferences.developerExtrasEnabled: true
        experimental.userStyleSheet: Qt.resolvedUrl("helper/messenger.css")
        experimental.userScripts: [
            Qt.resolvedUrl("helper/console.js"),
            Qt.resolvedUrl("helper/messenger.js")
        ]
        experimental.preferences.navigatorQtObjectEnabled: true
        experimental.preferences.fullScreenEnabled: true
        experimental.preferences.offlineWebApplicationCacheEnabled: true
        onTitleChanged: {

        }
        experimental.onMessageReceived: {
            var data;
            try{
                data = JSON.parse(message.data);
            }catch(e){
                return console.log("Error: "+message.data);
            }

            switch(data.type){
                case "log":
                    console.log(data.msg);
                    break;
                default:
                    console.log("Unhandled type: "+data.type);
                    break;
            }
        }
    }
}
