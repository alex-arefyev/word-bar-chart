import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.2

Item {
    id: chartItem

    property string name: ""
    property int rating: 0
    property int index: 0
    property real count: 0
    property real maxCount: 0
    property color ratingColor: "white"

    property real globalWordRectWidth: 0
    property real globalRatingRectWidth: 0

    readonly property alias wordRectWidth: wordText.contentWidth
    readonly property alias ratingRectWidth: ratingText.contentWidth

    // word rect
    Rectangle {
        height: parent.height
        width: globalWordRectWidth
        color: Qt.lighter("gray", 1.2)
        Text {
            id: wordText
            anchors.fill: parent
            anchors.rightMargin: 5
            text: chartItem.name
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            font.bold: true
        }
    }
    // rating item
    Item {
        id: ratingItem

        x: globalWordRectWidth
        height: parent.height
        width: {
            if (chartItem.maxCount > 0) {
                const w = chartItem.width - chartItem.globalWordRectWidth;
                return w * (chartItem.count / chartItem.maxCount);
            }
            return 0;
        }

        Rectangle {
            id: ratingBar
            width: ratingItem.width - chartItem.globalRatingRectWidth
            height: parent.height
            color: chartItem.ratingColor

            Behavior on color {
                enabled: WordBarChart.animate && WordBarChart.reading
                ColorAnimation { duration: 234 }
            }
        }
        Text {
            id: ratingText
            anchors.left: ratingBar.right
            anchors.leftMargin: 5
            height: parent.height
            text: "" + chartItem.count
            visible: parent.width > 0
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        Behavior on width {
            enabled: WordBarChart.animate && WordBarChart.reading
            NumberAnimation { duration: 234 }
        }
    }

}
