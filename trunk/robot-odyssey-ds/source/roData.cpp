/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Accessor functions for Robot Odyssey's in-memory data.
 *
 * Copyright (c) 2009 Micah Dowty <micah@navi.cx>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

#include "roData.h"


ROWorld *ROWorld::fromProcess(SBTProcess *proc) {
    return (ROWorld*) (proc->memSeg(proc->reg.ds) + RO_MEM_WORLD_DATA);
}

ROCircuit *ROCircuit::fromProcess(SBTProcess *proc) {
    return (ROCircuit*) (proc->memSeg(proc->reg.ds) + RO_MEM_CIRCUIT_DATA);
}

RORoomId ROWorld::getObjectRoom(ROObjectId obj) {
    return (RORoomId) objects.room[obj];
}

void ROWorld::getObjectXY(ROObjectId obj, int &x, int &y) {
    x = objects.x[obj];
    y = objects.y[obj];
}

void ROWorld::setObjectRoom(ROObjectId obj, RORoomId room) {
    removeObjectFromRoom(obj, getObjectRoom(obj));
    objects.room[obj] = room;
    addObjectToRoom(obj, room);
}

void ROWorld::setObjectXY(ROObjectId obj, int x, int y) {
    objects.x[obj] = x;
    objects.y[obj] = y;
}

void ROWorld::removeObjectFromRoom(ROObjectId obj, RORoomId room) {
    /*
     * Remove an object from a room's linked list.  If 'room' is
     * RO_ROOM_NONE or the object isn't found, has no effect.
     */

    if (room == RO_ROOM_NONE) {
        return;
    }

    uint8_t *head = &rooms.objectListHead[room];

    while (*head != RO_OBJ_NONE) {
        if (*head == obj) {
            /* Found it */
            *head = objects.nextInRoom[obj];
            return;
        }
        head = &objects.nextInRoom[*head];
    }
}

void ROWorld::addObjectToRoom(ROObjectId obj, RORoomId room) {
    /*
     * Add an object to a room's linked list.
     * If 'room' is RO_ROOM_NONE, has no effect.
     */

    if (room == RO_ROOM_NONE) {
        return;
    }

    objects.nextInRoom[obj] = rooms.objectListHead[room];
    rooms.objectListHead[room] = obj;
}

void ROWorld::setRobotRoom(ROObjectId obj, RORoomId room) {
    /*
     * Robots come in two halves. Set both of them to the same room.
     */

    ROObjectId left = (ROObjectId) (obj & ~1);
    ROObjectId right = (ROObjectId) (left + 1);

    setObjectRoom(left, room);
    setObjectRoom(right, room);
}

void ROWorld::setRobotXY(ROObjectId obj, int x, int y) {
    /*
     * Robots come in two halves. Set the left one to the given
     * position, and the right one needs to be 5 pixels away from the
     * left one.
     */

    ROObjectId left = (ROObjectId) (obj & ~1);
    ROObjectId right = (ROObjectId) (left + 1);

    setObjectXY(left, x, y);
    setObjectXY(right, x + 5, y);
}
