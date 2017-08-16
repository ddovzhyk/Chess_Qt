import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

ApplicationWindow {
  id: screen1
  title: qsTr("Chess")
  visible: true
  width: 800
  height: 560

  property int squareSize: 70

  property string endGameStr: qsTr("... win!")

  property var images: [
    {'imgPath' : "/images/white_rook.svg"},
    {'imgPath' : "/images/black_rook.svg"},

    {'imgPath' : "/images/white_knight.svg"},
    {'imgPath' : "/images/black_knight.svg"},

    {'imgPath' : "/images/white_bishop.svg"},
    {'imgPath' : "/images/black_bishop.svg"},

    {'imgPath' : "/images/white_queen.svg"},
    {'imgPath' : "/images/black_queen.svg"},

    {'imgPath' : "/images/white_king.svg"},
    {'imgPath' : "/images/black_king.svg"},

    {'imgPath' : "/images/white_pawn.svg"},
    {'imgPath' : "/images/black_pawn.svg"},
  ]

  Item {
    id: gameBoardScreen1
    x: 0
    y: 0

    width : logic.boardSize * squareSize
    height: logic.boardSize * squareSize

    Image {
      source: "/images/chess_board.jpg"
      height: parent.height
      width: parent.width
    }
  }

  Button {
    id: startButton
    anchors.top: parent.top
    anchors.left: gameBoardScreen1.right
    anchors.right: parent.right
    anchors.topMargin: 50
    anchors.leftMargin: 10
    anchors.rightMargin: 10
    height: 110
    text: qsTr("Start")

    onClicked: {
      screen2.show()
      screen1.hide()
      logic.newGame()
      gameBoardScreen2.enabled = true
      whiteRook2.visible = logic.getCurrentTurn() === 0 ? true : false
      blackRook2.visible = logic.getCurrentTurn() === 0 ? false : true
      saveButton.enabled = false;
    }
  }

  Button {
    id: loadButton
    anchors.top: startButton.bottom
    anchors.left: gameBoardScreen1.right
    anchors.right: parent.right
    anchors.topMargin: 50
    anchors.leftMargin: 10
    anchors.rightMargin: 10
    enabled: logic.isCorrectSavedGame() ? true : false
    height: 110
    text: qsTr("Load")
    onClicked: {
      screen3.show()
      logic.loadGame()
      saveButtonScreen3.enabled = false;
      gameBoardScreen3.enabled = logic.isDeadKingWhite() || logic.isDeadKingBlack() ? false : true
      whiteRook3.visible = logic.getCurrentTurn() === 0 ? true : false
      blackRook3.visible = logic.getCurrentTurn() === 0 ? false : true
      if (logic.isDeadKingWhite() || logic.isDeadKingBlack())
      {
        whiteRook3.visible = false
        blackRook3.visible = false
      }
      nextButton.enabled = false
      prevButton.enabled = true
      screen1.hide()
    }
  }

  Button {
    anchors.left: gameBoardScreen1.right
    anchors.right: parent.right
    anchors.bottom: gameBoardScreen1.bottom
    anchors.leftMargin: 40
    anchors.rightMargin: 40
    anchors.bottomMargin: 80
    height: 40
    text: qsTr("Quit")
    onClicked: {
      Qt.quit()
    }
  }

  ApplicationWindow {
    id: screen2
    title: qsTr("Chess")
    visible: false
    width: 800
    height: 560

    Item {
      id: gameBoardScreen2
      x: 0
      y: 0

      width : logic.boardSize * squareSize
      height: logic.boardSize * squareSize

      enabled: false

      Image {
        source: "/images/chess_board.jpg"
        height: gameBoardScreen2.height
        width: gameBoardScreen2.width
      }

      Repeater {
        model: logic

        Image {
          height: squareSize
          width : squareSize

          x: squareSize * positionX
          y: squareSize * positionY

          source: images[type].imgPath

          visible: (positionX == logic.boardSize && positionY == logic.boardSize) ? false : true

          MouseArea {
            anchors.fill: parent
            drag.target: parent

            property int startX: 0
            property int startY: 0

            cursorShape: Qt.PointingHandCursor

            visible: (logic.isCorrectTurn(type)) ? true : false

            onPressed: {
              startX = parent.x;
              startY = parent.y;
            }

            onReleased: {
              var fromX = startX / squareSize;
              var fromY = startY / squareSize;
              var toX   = (parent.x + mouseX) / squareSize;
              var toY   = (parent.y + mouseY) / squareSize;

              if (logic.move(fromX, fromY, toX, toY))
              {
                saveButton.enabled = true;
                whiteRook2.visible = logic.getCurrentTurn() === 0 ? true : false
                blackRook2.visible = logic.getCurrentTurn() === 0 ? false : true
                if (logic.isDeadKingWhite())
                {
                  endGameStr = qsTr("Black win!");
                  whiteRook2.visible = false
                  blackRook2.visible = false
                  endGameDialog.visible = true;
                  gameBoardScreen2.enabled = false;
                }
                else if (logic.isDeadKingBlack())
                {
                  endGameStr = qsTr("White win!");
                  whiteRook2.visible = false
                  blackRook2.visible = false
                  endGameDialog.visible = true;
                  gameBoardScreen2.enabled = false;
                }
              }
            }
          }
        }
      }
    }

    Button {
      id: saveButton
      anchors.top: parent.top
      anchors.left: gameBoardScreen2.right
      anchors.right: parent.right
      anchors.topMargin: 50
      anchors.leftMargin: 30
      anchors.rightMargin: 30
      height: 80
      text: qsTr("Save")
      enabled: false
      onClicked: {
        logic.saveHistory();
        saveButton.enabled = false
        loadButton.enabled = true
      }
    }

    Button {
      id: stopButton
      anchors.top: saveButton.bottom
      anchors.left: gameBoardScreen2.right
      anchors.right: parent.right
      anchors.topMargin: 30
      anchors.leftMargin: 30
      anchors.rightMargin: 30
      height: 80
      text: qsTr("Menu")
      onClicked: {
        screen1.show()
        screen2.close()
      }
    }
    Text {
      id: currentTurnString2
      text: "Current turn:"
      color: "black"
      font.pixelSize: 25
      font.bold: true
      anchors.top: stopButton.bottom
      anchors.left: gameBoardScreen2.right
      anchors.topMargin: 75
      anchors.leftMargin: 40
    }
    Image {
      id: whiteRook2
      source: "/images/white_rook.svg"
      height: squareSize
      width : squareSize
      x: squareSize * 9 + 12
      y: squareSize * 5
      visible: logic.getCurrentTurn() === 0 ? true : false
    }
    Image {
      id: blackRook2
      source: "/images/black_rook.svg"
      height: squareSize
      width : squareSize
      x: squareSize * 9 + 12
      y: squareSize * 5
      visible: logic.getCurrentTurn() === 0 ? false : true
    }


    Button {
      anchors.left: gameBoardScreen2.right
      anchors.right: parent.right
      anchors.bottom: gameBoardScreen2.bottom
      anchors.leftMargin: 50
      anchors.rightMargin: 50
      anchors.bottomMargin: 50
      height: 30
      text: qsTr("Quit")
      onClicked: {
        Qt.quit()
      }
    }
  }

  ApplicationWindow {
    id: screen3
    title: qsTr("Chess")
    width: 800
    height: 560

    Item {
      id: gameBoardScreen3
      x: 0
      y: 0

      width : logic.boardSize * squareSize
      height: logic.boardSize * squareSize
      Image {
        source: "/images/chess_board.jpg"
        height: gameBoardScreen3.height
        width: gameBoardScreen3.width
      }

      Repeater {
        model: logic

        Image {
          height: squareSize
          width : squareSize

          x: squareSize * positionX
          y: squareSize * positionY

          source: images[type].imgPath

          visible: (positionX == logic.boardSize && positionY == logic.boardSize) ? false : true

          MouseArea {
            anchors.fill: parent
            drag.target: parent

            property int startX: 0
            property int startY: 0

            cursorShape: Qt.PointingHandCursor

            visible: (logic.isCorrectTurn(type)) ? true : false

            onPressed: {
              startX = parent.x;
              startY = parent.y;
            }

            onReleased: {
              var fromX = startX / squareSize;
              var fromY = startY / squareSize;
              var toX   = (parent.x + mouseX) / squareSize;
              var toY   = (parent.y + mouseY) / squareSize;

              if (logic.move(fromX, fromY, toX, toY))
              {
                prevButton.enabled = false
                nextButton.enabled = false
                saveButtonScreen3.enabled = true
                whiteRook3.visible = logic.getCurrentTurn() === 0 ? true : false
                blackRook3.visible = logic.getCurrentTurn() === 0 ? false : true

                if (logic.isDeadKingWhite())
                {
                  endGameStr = qsTr("Black win!");
                  whiteRook3.visible = false
                  blackRook3.visible = false
                  endGameDialog.visible = true;
                  gameBoardScreen3.enabled = false;
                }
                else if (logic.isDeadKingBlack())
                {
                  endGameStr = qsTr("White win!");
                  whiteRook3.visible = false
                  blackRook3.visible = false
                  endGameDialog.visible = true;
                  gameBoardScreen3.enabled = false;
                }
              }
            }
          }
        }
      }
    }

    Button {
      id: newGameButton
      anchors.top: parent.top
      anchors.left: gameBoardScreen3.right
      anchors.right: parent.right
      anchors.topMargin: 25
      anchors.leftMargin: 45
      anchors.rightMargin: 45
      height: 35
      text: qsTr("New game")

      onClicked: {
        screen3.close()
        screen2.show()
        logic.newGame()
        gameBoardScreen2.enabled = true
        saveButton.enabled = false;
        whiteRook2.visible = logic.getCurrentTurn() === 0 ? true : false
        blackRook2.visible = logic.getCurrentTurn() === 0 ? false : true
      }
    }

    Button {
      id: menuButton
      anchors.top: newGameButton.bottom
      anchors.left: gameBoardScreen3.right
      anchors.right: parent.right
      anchors.topMargin: 20
      anchors.leftMargin: 45
      anchors.rightMargin: 45
      height: 35

      text: qsTr("Menu")

      onClicked: {
        screen1.show()
        screen3.close()
        logic.newGame()
      }
    }

    Button {
      id: saveButtonScreen3
      anchors.top: menuButton.bottom
      anchors.left: gameBoardScreen3.right
      anchors.right: parent.right
      anchors.topMargin: 20
      anchors.leftMargin: 45
      anchors.rightMargin: 45
      height: 35
      enabled: false

      text: qsTr("Save")

      onClicked: {
        logic.saveHistory();
        saveButtonScreen3.enabled = false
      }
    }

    Text {
      id: currentTurnString3
      text: "Current turn:"
      color: "black"
      font.pixelSize: 20
      font.bold: true
      anchors.top: saveButtonScreen3.bottom
      anchors.left: gameBoardScreen3.right
      anchors.topMargin: 20
      anchors.leftMargin: 55
    }
    Image {
      id: whiteRook3
      source: "/images/white_rook.svg"
      height: squareSize
      width : squareSize
      x: squareSize * 9 + 12
      y: squareSize * 3
      visible: logic.getCurrentTurn() === 0 ? true : false
    }
    Image {
      id: blackRook3
      source: "/images/black_rook.svg"
      height: squareSize
      width : squareSize
      x: squareSize * 9 + 12
      y: squareSize * 3
      visible: logic.getCurrentTurn() === 0 ? false : true
    }

    Button {
      id: nextButton
      anchors.top: whiteRook3.bottom
      anchors.left: gameBoardScreen3.right
      anchors.right: parent.right
      anchors.topMargin: 15
      anchors.leftMargin: 30
      anchors.rightMargin: 30
      height: 75
      enabled: false
      text: qsTr("Next")

      onClicked: {
        logic.nextMove()
        prevButton.enabled = true
        if (logic.isNewestMove())
          nextButton.enabled = false;
        gameBoardScreen3.enabled = logic.isDeadKingWhite() || logic.isDeadKingBlack() ? false : true
        whiteRook3.visible = logic.getCurrentTurn() === 0 ? true : false
        blackRook3.visible = logic.getCurrentTurn() === 0 ? false : true
        if (logic.isDeadKingWhite() || logic.isDeadKingBlack())
        {
          whiteRook3.visible = false
          blackRook3.visible = false
        }
      }
    }
    Button {
      id: prevButton
      anchors.top: nextButton.bottom
      anchors.left: gameBoardScreen3.right
      anchors.right: parent.right
      anchors.topMargin: 20
      anchors.leftMargin: 30
      anchors.rightMargin: 30
      height: 75

      text: qsTr("Prev")

      onClicked: {
        logic.prevMove()
        nextButton.enabled = true;
        if (logic.getIndexHistory() < 0)
          prevButton.enabled = false;
        gameBoardScreen3.enabled = logic.isDeadKingWhite() || logic.isDeadKingBlack() ? false : true
        whiteRook3.visible = logic.getCurrentTurn() === 0 ? true : false
        blackRook3.visible = logic.getCurrentTurn() === 0 ? false : true
      }
    }
    Button {
      anchors.left: gameBoardScreen3.right
      anchors.right: parent.right
      anchors.bottom: gameBoardScreen3.bottom
      anchors.leftMargin: 45
      anchors.rightMargin: 45
      anchors.bottomMargin: 30
      height: 40

      text: qsTr("Quit")

      onClicked: {
        Qt.quit()
      }
    }
  }

  Dialog {
    id: endGameDialog
    visible: false
    title: qsTr("Game over")
    contentItem: Rectangle {
      color: "white"
      implicitWidth: 300
      implicitHeight: 200

      Text {
        id: endGameText
        text: endGameStr
        color: "black"
        font.pixelSize: 40
        font.bold: true
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 40
        anchors.leftMargin: 40
      }

      Button {
        anchors.top: endGameText.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 30
        anchors.leftMargin: 70
        anchors.rightMargin: 70
        height: 50

        text: qsTr("OK")
        onClicked: {
          endGameDialog.close()
        }
      }
    }
  }
}
