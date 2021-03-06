import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import MoonPlayer 1.0

Popup {
    id: playlistPopup

    signal openFileRequested()
    signal openUrlRequested()

    width: 180
    height: 300
    
    GridLayout {
        anchors.fill: parent
        columns: 3
        
        Label {
            text: qsTr("Playlist")
            font.pixelSize: 16
            font.bold: true
            Layout.columnSpan: 3
        }
        
        ScrollView {
            Layout.columnSpan: 3
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ListView {
                id: listView
                
                model: playlistModel
                delegate: Rectangle {
                    width: 150
                    height: 20
                    radius: 4
                    clip: true
                    color: index == listView.currentIndex ? Color.listItemSelected : (index == playlistModel.playingIndex ? Color.listItemCurrentActive : "transparent")
                    Label { text: title }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: listView.currentIndex = index
                        onDoubleClicked: playlistModel.playItem(index)
                    }
                }
            }
        }
        
        Button {
            id: addButton
            text: qsTr("Add")
            implicitWidth: 50
            onClicked: {
                addMenu.x = addButton.x
                addMenu.y = addButton.y - addMenu.height
                addMenu.open()
            }
        }
        
        Button {
            id: delButton
            text: qsTr("Del")
            implicitWidth: 50
            onClicked: playlistModel.removeItem(listView.currentIndex)
        }
        
        Button {
            id: clearButton
            text: qsTr("Clear")
            implicitWidth: 50
            onClicked: playlistModel.clear()
        }
    }
    
    Menu {
        id: addMenu
        width: 100
        MenuItem {
            text: qsTr("File...")
            onTriggered: openFileRequested()
        }
        MenuItem {
            text: qsTr("Url...")
            onTriggered: openUrlRequested()
        }
    }
}
