/*
 * Stellarium
 * Copyright (C) 2015 Guillaume Chereau
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

Item {
    id: bottomBar
    property bool up: true

    width: row.width + 100
    height: up ? 75 : 30

    RowLayout {
        id: infoRow
        anchors {
            left: row.left
            right: row.right
            top: parent.top
            margins: 8
        }
        Text {
            text: "Paris"
            color: "white"
        }
        Item { Layout.fillWidth: true }
        Text {
            text: "FOV 60"
            color: "white"
        }
        Item { Layout.fillWidth: true }
        Text {
            text: "23.3 FPS"
            color: "white"
        }
        Item { Layout.fillWidth: true }
        Text {
            text: "2015-08-7 14:50:30 UTC+08:00"
            color: "white"
        }
    }

    Row {
        id: row
        height: childrenRect.height
        width: childrenRect.width
        spacing: 4
        anchors {
            right: parent.right
            top: infoRow.bottom
            margins: 8
            rightMargin: 18
        }

        Repeater {
            model: bars.bottomGroups
            delegate: Rectangle {
                id: groupItem
                height: childrenRect.height
                width: childrenRect.width
                radius: 4
                color: "#2eb4b8c0"
                property var buttons: bars.getButtons(bars.bottomGroups[index])

                Row {
                    height: childrenRect.height
                    width: childrenRect.width
                    Repeater {
                        model: groupItem.buttons
                        delegate: StelActionButton {
                            image: groupItem.buttons[index].pixOn
                            imageOff: groupItem.buttons[index].pixOff
                            action: groupItem.buttons[index].action
                            onHoveredChanged: {
                                if (hovered)
                                    bottomBar.setTooltip(tooltip, shortcut)
                                else
                                    bottomBar.setTooltip("")
                            }
                        }
                    }
                }
            }
        }
    }

    function setTooltip(txt, shortcut) {
        if (shortcut)
            tooltipItem.text = "%1 [%2]".arg(txt).arg(shortcut);
        else
            tooltipItem.text = txt
    }

    Text {
        id: tooltipItem
        visible: text != ""
        y: -28
        x: 100
        color: "white"
    }
}
