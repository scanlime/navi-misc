/* -*- Mode: C++; c-basic-offset: 4 -*-
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

#include <string.h>
#include "roData.h"


ROWorld *ROWorld::fromProcess(SBTProcess *proc) {
    return (ROWorld*) (proc->memSeg(proc->reg.ds) +
                       proc->getAddress(SBTADDR_WORLD_DATA));
}

ROCircuit *ROCircuit::fromProcess(SBTProcess *proc) {
    return (ROCircuit*) (proc->memSeg(proc->reg.ds) +
                       proc->getAddress(SBTADDR_CIRCUIT_DATA));
}

RORobot *RORobot::fromProcess(SBTProcess *proc) {
    return (RORobot*) (proc->memSeg(proc->reg.ds) +
                       proc->getAddress(SBTADDR_ROBOT_DATA_MAIN));
}

RORobotGrabber *RORobotGrabber::fromProcess(SBTProcess *proc) {
    return (RORobotGrabber*) (proc->memSeg(proc->reg.ds) +
                              proc->getAddress(SBTADDR_ROBOT_DATA_GRABBER));
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

ROData::ROData(SBTProcess *proc) {
    world = ROWorld::fromProcess(proc);
    circuit = ROCircuit::fromProcess(proc);
    robots.state = RORobot::fromProcess(proc);
    robots.grabbers = RORobotGrabber::fromProcess(proc);

    /*
     * We can infer the number of robots by looking at the
     * size of the robotGrabbers table, which is just prior
     * to the 'robots' table.
     *
     * Also, we can double-check this by looking for an 0xFF
     * termination byte just after the end of the RORobot table.
     */

    robots.count = (RORobotGrabber*)robots.state - robots.grabbers;
    sassert(robots.count == 3 || robots.count == 4,
            "Robot table sanity check failed");

    uint8_t *endOfTable = (uint8_t*)&robots.state[robots.count];
    sassert(*endOfTable == 0xFF, "End of robot table not found");

    /*
     * Just after the robot's table terminator, there is an array of
     * robot battery accumulators.
     */

    robots.batteryAcc = (RORobotBatteryAcc*) (endOfTable + 1);
}

void ROData::copyFrom(ROData *source) {
    /*
     * Copy all world data from another process.
     */

    const int copyRobots = robots.count < source->robots.count ?
                           robots.count : source->robots.count;

    memcpy(world, source->world, sizeof *world);
    memcpy(circuit, source->circuit, sizeof *circuit);

    memcpy(robots.grabbers, source->robots.grabbers,
           sizeof robots.grabbers[0] * copyRobots);
    memcpy(robots.state, source->robots.state,
           sizeof robots.state[0] * copyRobots);
    memcpy(robots.batteryAcc, source->robots.batteryAcc,
           sizeof robots.batteryAcc[0] * copyRobots);

    /*
     * Move the grabber sprites, if necessary.
     */

    if (robots.count == 4 && source->robots.count == 3) {
        /* Convert sprites from TUT/LAB to GAME IDs */
        memcpy(&world->sprites[RO_SPR_GAME_GRABBER_UP],
               &source->world->sprites[RO_SPR_GRABBER_UP], sizeof(ROSprite));
        memcpy(&world->sprites[RO_SPR_GAME_GRABBER_RIGHT],
               &source->world->sprites[RO_SPR_GRABBER_RIGHT], sizeof(ROSprite));
        memcpy(&world->sprites[RO_SPR_GAME_GRABBER_LEFT],
               &source->world->sprites[RO_SPR_GRABBER_LEFT], sizeof(ROSprite));
    } else if (robots.count == 3 && source->robots.count == 4) {
        /* Convert sprites from GAME to TUT/LAB IDs */
        memcpy(&world->sprites[RO_SPR_GRABBER_UP],
               &source->world->sprites[RO_SPR_GAME_GRABBER_UP], sizeof(ROSprite));
        memcpy(&world->sprites[RO_SPR_GRABBER_RIGHT],
               &source->world->sprites[RO_SPR_GAME_GRABBER_RIGHT], sizeof(ROSprite));
        memcpy(&world->sprites[RO_SPR_GRABBER_LEFT],
               &source->world->sprites[RO_SPR_GAME_GRABBER_LEFT], sizeof(ROSprite));
    }
}
