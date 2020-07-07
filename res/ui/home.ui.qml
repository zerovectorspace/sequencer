import QtQuick 2.15
import QtQuick.Controls 2.2
// import com.app 1.0 // Wav
import QtGraphicalEffects 1.12 // RadialGradient
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.12
import Qt.labs.platform 1.1

Page {
  function resetRandomOptionsBoxPosition() {
    randomOptionsBox.x = page.width / 2 - randomOptionsBox.width / 2
    randomOptionsBox.y = page.height / 2 - randomOptionsBox.height / 2
  }

  id: page
  onWidthChanged: resetRandomOptionsBoxPosition()
  onHeightChanged: onWidthChanged

  /* top controls */
  GridLayout {
    columns: 4
    anchors.bottom: padGrid.top;
    anchors.horizontalCenter: padGrid.horizontalCenter;

    /* play/stop */
    Button {
      id: playStop
      property bool isEngaged

      text: isEngaged ? "Stop" : "Play";
      highlighted: isEngaged
      onPressed: isEngaged = isEngaged ? engineModel.stop() : engineModel.play();
    }

    /* tempo */
    SpinBox {
      id: tempo
      value: engineModel.bpm
      editable: true
      stepSize: 1
      from: 30
      to: 300

      onValueModified: engineModel.bpm = value;
    }

    /* grid size */
    SpinBox {
      id: gridSize
      value: padModel.gridHeight
      editable: true
      stepSize: 1
      from: 1
      to: 16

      onValueModified: {
        engineModel.stop();

        playStop.isEngaged = false;
        padModel.gridHeight = value;
        padModel.newGrid();
      }
    }

    /* clear */
    Button {
      id: clearButton
      text: "Clear"
      onPressed: {
        engineModel.stop();

        playStop.isEngaged = false;
        padModel.clear();
      }
    }

    /* empty */
    Rectangle {
      Layout.minimumWidth: playStop.width
    }

    Label {
      text: "bpm"
      Layout.minimumWidth: tempo.width
      horizontalAlignment: Text.AlignHCenter
    }

    Label {
      text: "size"
      Layout.minimumWidth: gridSize.width
      horizontalAlignment: Text.AlignHCenter
    }

    /* empty */
    Rectangle {
      Layout.minimumWidth: clearButton.width
    }
  }

  /* side controls */
  Column {
  }

  /* pads */
  GridView {
    /* data */
    id: padGrid
    model: padModel

    required model

    /* anchors */
    anchors.centerIn: parent

    /* visual */
    height: Math.min( page.width, page.height  ) * 0.75
    cellHeight: height / padModel.gridHeight
    cellWidth: cellHeight
    width: cellHeight * padModel.gridWidth

    interactive: false

    delegate: Component {
      Item {
        id: cont
        height: padGrid.cellHeight
        width: height

        // Rectangle {
        // anchors.fill: parent
        // color: "#220000ff"
        // }

        Button {
          // required property int index
          // required property bool engaged
          // required property bool playing

          /* data */
          id: delegate
          highlighted: engaged

          /* anchors */
          anchors.centerIn: parent
          anchors.fill: parent
          background.anchors.centerIn: delegate

          /* make actual square */
          background.height: padGrid.cellHeight - 2
          background.width: padGrid.cellHeight - 12

          /* visual */

          onPressed: {
            padModel.toggleEngaged( index );

            // background.height = background.width
            // background.width = padGrid.cellHeight - 12
            // console.log( background.width + " " + background.height );

            // padBlink.duration = engineModel.beatDuration()
            // console.log( "*** clicked "  + index + " " + engaged + " " + playing );
          }

          Rectangle {
            // required property bool playing
            // required property bool engaged

            /* data */
            id: playingRect
            anchors.centerIn: parent
            visible: playing && engaged

            /* visual */
            height: delegate.height
            width: delegate.height
            radius: 50
            color: "#00000000"

            RadialGradient {
              id: padGradient
              anchors.fill: parent
              gradient: Gradient {
                GradientStop { position: 0.0; color: "#ffffffff" }
                GradientStop { position: 0.5; color: "#00ffffff" }
              }
            }

            // states: State {
            // when: playing
            // PropertyChanges { target: playingRect; opacity: 0 }
            // }

            // transitions: Transition {
            // id: padBlink
            // PropertyAnimation {
            // properties: "opacity";
            // easing.type: Easing.InCubic
            // }
            // }
          } // Rectangle
        } // Button
      } // Item
    } // delegate

    DragHandler {
      yAxis.enabled: false
    }
  } // GridView

  /* bottom controls */
  Row {
    anchors.top: padGrid.bottom
    anchors.horizontalCenter: padGrid.horizontalCenter
    spacing: 2
    Button {
      text: "Random"
      onClicked: randomOptionsBox.visible = ! randomOptionsBox.visible
    }

    ComboBox {
      model: ListModel {
        ListElement { text: "C" }
        ListElement { text: "Db" }
        ListElement { text: "D" }
        ListElement { text: "Eb" }
        ListElement { text: "E" }
        ListElement { text: "F" }
        ListElement { text: "Gb" }
        ListElement { text: "G" }
        ListElement { text: "Ab" }
        ListElement { text: "A" }
        ListElement { text: "Bb" }
        ListElement { text: "B" }
      }
      onActivated: {
        var wasRunning = engineModel.isRunning();
        if ( wasRunning )
        {
          engineModel.stop();
        }

        engineModel.setRootNote( currentValue );

        if ( wasRunning )
        {
          engineModel.play( false );
        }
      }
    }
    ComboBox {
      model: ListModel {
        ListElement { text: "Maj7" }
        ListElement { text: "Maj9" }
        ListElement { text: "7" }
        ListElement { text: "9" }
        ListElement { text: "-7" }
        ListElement { text: "+7" }
        ListElement { text: "half dim" }
        ListElement { text: "O" }
        ListElement { text: "minMaj" }
        ListElement { text: "Maj7#5" }
        ListElement { text: "7#11" }
        ListElement { text: "7#9" }
        ListElement { text: "7#5#9" }
        ListElement { text: "7b9" }
        ListElement { text: "-9" }
        ListElement { text: "-7b9" }
        ListElement { text: "min" }
      }
      onActivated: {
        var wasRunning = engineModel.isRunning();
        if ( wasRunning )
        {
          engineModel.stop();
        }

        engineModel.setQuality( currentValue );

        if ( wasRunning )
        {
          engineModel.play( false );
        }
      }
    }
    Button {
      text: "Save as midi"
      onClicked: fileDialog.open()
    }
  }

  /* Random Options Box */
  Rectangle {
    id: randomOptionsBox
    visible: false
    width: 250
    height: 150
    x: page.width / 2 - width / 2
    y: page.height / 2 - height / 2
    color: "#ff303030"

    DragHandler { }

    Column {
      /* Pattern */
      anchors.fill: parent
      padding: 5
      topPadding: 10

      Label {
        text: "Notes per column pattern (e.g. 1212)"
        anchors.horizontalCenter: parent.horizontalCenter
        wrapMode: Label.WordWrap
        width: 200
      }

      TextField {
        id: randomPattern
        anchors.horizontalCenter: parent.horizontalCenter
        width: 175
        text: "31103021"
        horizontalAlignment: Qt.AlignHCenter
      }

      /* Apply/Close */
      Row {
        anchors.horizontalCenter: parent.horizontalCenter
        padding: 5
        Button {
          text: "Apply"

          onClicked: {
            var wasRunning = engineModel.isRunning();
            if ( wasRunning )
            {
              engineModel.stop();
            }

            padModel.random( randomPattern.text );

            if ( wasRunning )
            {
              engineModel.play( false );
            }
          }
        }
        Button {
          text: "Close"

          onPressed: randomOptionsBox.visible = false
        }
      }
    }
  }

  DropShadow {
    id: randomOptionsBoxShadow
    visible: randomOptionsBox.visible
    anchors.fill: randomOptionsBox
    horizontalOffset: 3
    verticalOffset: 3
    spread: 0.0
    radius: 20.0
    samples: 25
    color: "#80000000"
    source: randomOptionsBox
  }

  FileDialog {
    id: fileDialog
    title: "Please choose a file"
    // folder: shortcuts.home
    folder: StandardPaths.writableLocation( StandardPaths.HomeLocation )
    fileMode: FileDialog.SaveFile
    visible: false
    onAccepted: {
      engineModel.saveMidiFile( fileDialog.file );
      visible = false;
    }
    onRejected: {
      visible = false;
    }
  }
}
