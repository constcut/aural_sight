import QtQuick 2.7
import mther.app 1.0
import QtQuick.Controls 1.4

Item {
    id: item

    property string filename: "last.wav"

    function reloadFile() {
        waveShape.loadFile(item.filename)
        console.log("reload file of waveshape qml called ", filename)
    }

    ScrollView
    {
        //Flickable
        width: parent.width
        height: parent.height / 3

        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOn
        verticalScrollBarPolicy:  Qt.ScrollBarAlwaysOff

        Flickable
        {
            id: flick
            y: 5
            x: 0
            width: parent.width
            height: parent.height-20

            contentWidth: 3000
            contentHeight:  parent.height

            //QML2.ScrollBar.horizontal:  QML2.ScrollBar { }

            property int pressedX : 0
            MouseArea {
                //anchors.fill: parent

                x:0
                y:20

                width: parent.width
                height: 250

                onPressed: {
                    flick.pressedX = mouseX
                }
                onReleased:
                {
                    var diff =  flick.pressedX - mouseX
                    flick.contentX += diff
                    if (flick.contentX < 0)
                        flick.contentX = 0
                    if (flick.contentX >wavPos.width)
                        flick.contentX = wavPos.width
                }

                onClicked:
                {
                    waveShape.setWindowPosition(mouseX*125.0/2.0)
                    spectrum.loadSpectrum(item.filename,mouseX*125.0/2.0)
                    rmsYinIngo.text = "RMS = " + spectrum.getRMS() + ". RMS no win = " + spectrum.getRMSNoWindow()
                    + "\n pitch = " + spectrum.getPitch() + ". midi = " + spectrum.freqToMidi(spectrum.getPitch());
                }
            }

            WavePosition
            {
                id:wavPos
                height: 20
                width: 1000 //actually must calculate it for each file
                y:  5

                property int time : 0
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        flick.contentX = 0
                        //soundEngine.loadFile(item.filename) //TODO
                        //soundEngine.startPlayback()
                    }
                }

            } //try to make transparent and put on the waveshape

            Waveshape
            {
                id: waveShape
                height:  parent.height
                width: 1000

                y: wavPos.y  + wavPos.height

                Component.onCompleted: {
                    waveShape.loadFile(item.filename)
                    flick.contentWidth = waveShape.getPixelsLength() + 10
                    waveShape.width = flick.contentWidth
                    wavPos.width = waveShape.width
                    console.log("component competed waveshape ", item.filename)

                }
            }

        }
    }

    Text{
        y:  spectrum.y + spectrum.height + 10
        x: parent.width / 3
        id: specLabel
        text: "Big Window size: "
    }

    ComboBox
    {
        id: sizeComboBox

        y:  spectrum.y + spectrum.height + 10
        x: specLabel.x + specLabel.width + 10
        model: ["125", "1024","2048","4096","8192","16384"]

        currentIndex: 3

        onCurrentTextChanged: {
            console.log("Selected " + sizeComboBox.currentIndex + sizeComboBox.currentText )

            var windowWidth = parseInt(currentText)

            waveShape.setWindowWidth(windowWidth)
            if (spectrum != null)
                spectrum.setSamplesAmount(windowWidth)
        }
    }

    Text{
        y:  spectrum.y + spectrum.height + 10
        x: sizeComboBox.x + sizeComboBox.width + 10
        id: yinLabel
        text: "Yin limit size: "
    }
    ComboBox {
        id: yinLimitCombo

        y: spectrum.y + spectrum.height + 10
        x: yinLabel.x + yinLabel.width + 10
        currentIndex: 1
        model: ["3000", "4096", "6000"] //TODO cuctom size

        onCurrentTextChanged: {
            if (spectrum)
                spectrum.setYinLimit(parseInt(currentText))
        }
    }
    Text{
        y:  spectrum.y + spectrum.height + 10
        x: yinLimitCombo.x + yinLimitCombo.width + 10
        id: windowLimitCombo
        text: "Window limit: "
    }

    ComboBox {
        id: windowCutCombo
        y: spectrum.y + spectrum.height + 10
        x: windowLimitCombo.x + windowLimitCombo.width + 10
        currentIndex: 4
        model: ["256","512","1024", "2048", "4096", "8192"] //TODO cuctom size

        onCurrentIndexChanged: {
            if (spectrum)
                spectrum.setFFTLimit(parseInt(currentText))
        }
    }


    Text {
        id: specInfo
        y : spectrum.y + spectrum.height + 10
        x : 25
        text: "Spectrum info"
    }
    Text {
        id : rmsYinIngo
        y : specInfo.y + specInfo.height + 5
        x: 25
        text: "rms yin info"
    }//TODO different parameters for yin to be set!

    Spectrograph
    {
        y: waveShape.height + waveShape.y + 5
        width: parent.width
        height: parent.height / 6

        id:spectrum
        Component.onCompleted: {
            //spectrum.setSoundEngine(soundEngine)
            spectrum.changeBarsCount(200)
        }
        MouseArea {
            anchors.fill: parent
            function log10(val) {
              return Math.log(val) / Math.LN10;
            }
            onClicked: {
                spectrum.onPress(mouseX, mouseY, spectrum.width, spectrum.height)
                specInfo.text = spectrum.getFreq1() + "-" + spectrum.getFreq2() + " Hz"
                + " value " + spectrum.getValue() + " dB = " + 10*log10(spectrum.getValue())
               // https://stackoverflow.com/questions/3019278/how-can-i-specify-the-base-for-math-log-in-javascript
            }
        }
    }


    /*
    Connections
    {
        target: soundEngine

        id: positionConnection

        property int counter : 0
        onPlayPositionChanged:
        {
            //console.log("Play change position " + position);

            wavPos.changePosition(position)
            positionConnection.counter += 1
            //console.log(positionConnection.counter)
            if (positionConnection.counter >= 10)
            {
                flick.contentX += 100;
                positionConnection.counter -= 10
            }
        }
    }*/
}
