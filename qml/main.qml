import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.3
import QtGraphicalEffects 1.2

ApplicationWindow {
    id: root

    property real wordRectWidth: 0
    property real ratingRectWidth: 0

    visible: true
    width: 640
    height: 480
    minimumWidth: 640
    title: qsTr("Word Bar Chart")

    Text {
        id: text
        visible: false
    }

    Component.onCompleted: {
        x = (Screen.width - width) / 2
        y = (Screen.height - height) / 2
        setMinHeight();
    }

    function setMinHeight() {
        var min = chartModel.size * text.font.pixelSize * 2 + 80; // approx.
        minimumHeight = min;
        height = min;
    }

    function updateWordRectWidth(width) {
        wordRectWidth = chartBars.getMaxWordRectWidth();
    }

    function updateRatingRectWidth(width) {
        var max = chartBars.getMaxRatingRectWidth();
        ratingRectWidth = (Math.round(max / 4) + 1) * 4;
    }

    Behavior on wordRectWidth {
        NumberAnimation { duration: 234 }
    }
    Behavior on ratingRectWidth {
        NumberAnimation { duration: 234 }
    }

    menuBar: MenuBar {
        id: menuBarItem
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("&Read from file...")
                shortcut: "Ctrl+R"
                enabled: !WordBarChart.reading
                onTriggered: fileDialog.open()
            }
            MenuSeparator {}
            MenuItem {
                text: qsTr("&Quit")
                shortcut: "Ctrl+Q"
                onTriggered: Qt.quit()
            }
        }
    }

    toolBar: ToolBar {
        RowLayout {
            id: row
            anchors.fill: parent
            anchors.margins: 0
            spacing: 5
            Label {
                text: WordBarChart.filePath
                elide: Label.ElideMiddle
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
            Item { Layout.minimumWidth: row.spacing }
            Label {
                Layout.minimumWidth: contentWidth
                text: qsTr("Words:")
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }
            Label {
                id: wordCount
                Layout.minimumWidth: contentWidth
                text: WordBarChart.wordCounter.toString()
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
            }
            Item { Layout.minimumWidth: row.spacing }
            Label {
                Layout.minimumWidth: contentWidth
                text: qsTr("Total:")
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }
            Label {
                id: totalCount
                Layout.minimumWidth: contentWidth
                text: WordBarChart.totalCounter.toString()
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
            }
            Item { Layout.minimumWidth: row.spacing }
            Button {
                text: WordBarChart.reading ? qsTr("Stop") : qsTr("Read from file...")
                onClicked: {
                    if (WordBarChart.reading) {
                        WordBarChart.stop();
                    } else {
                        fileDialog.open();
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Choose a text file")
        folder: shortcuts.home
        nameFilters: [ "Text files (*.txt;*.h;*.c;*.hpp;*.cpp;*.qml;*.js;*.xml;*.html)", "All files (*)" ]
        selectExisting: true
        onAccepted: WordBarChart.load(fileUrl)
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.minimumHeight: 3
            Layout.fillWidth: true

            Rectangle {
                anchors.left: parent.left
                height: parent.height
                width: parent.width * (WordBarChart.progress / 100)
                color: "red"
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Rectangle {
                id: chart
                anchors.fill: parent
                anchors.margins: 5
                color: "lightgray"

                Text {
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 10
                    text: qsTr("Word Bar Chart")
                    opacity: 0.25
                    color: "white"
                    font.pixelSize: 42
                    lineHeight: 1.0
                }

                Repeater {
                    id: chartBars

                    function getMaxWordRectWidth() {
                        var max = 0;
                        for (var i = 0; i < count; ++i) {
                            const item = itemAt(i);
                            if (item) {
                                max = Math.max(max, 5 + item.wordRectWidth + 5);
                            }
                        }
                        return max;
                    }

                    function getMaxRatingRectWidth() {
                        var max = 0;
                        for (var i = 0; i < count; ++i) {
                            const item = itemAt(i);
                            if (item) {
                                max = Math.max(max, 5 + item.ratingRectWidth + 5);
                            }
                        }
                        return max;
                    }

                    model: chartModel.content
                    delegate: Item {
                        readonly property alias wordRectWidth: chartItem.wordRectWidth
                        readonly property alias ratingRectWidth: chartItem.ratingRectWidth

                        height: chart.height / chartModel.size
                        x: 0
                        y: model.rating * height
                        width: parent.width
                        ChartItem {
                            id: chartItem
                            anchors.fill: parent
                            anchors.margins: 1
                            name: model.name
                            count: model.count
                            maxCount: chartModel.maxCount
                            rating: model.rating
                            index: model.index
                            ratingColor: model.color
                            globalWordRectWidth: root.wordRectWidth
                            globalRatingRectWidth: root.ratingRectWidth
                            onWordRectWidthChanged: root.updateWordRectWidth(wordRectWidth)
                            onRatingRectWidthChanged: root.updateRatingRectWidth(ratingRectWidth)
                        }
                        Behavior on y {
                            enabled: WordBarChart.animate && WordBarChart.reading
                            NumberAnimation { duration: 234 }
                        }
                    }
                }
            }
        }
    }
}
