/*
 * pong.cpp - Simple pong-like game using the field sensors
 *
 * Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Contributors:
 * 
 * 
 * 
 */

#include <PongObjects.h>

int main(void) {
  SimpleGame game;

  game.objects.push_back(new PlayfieldSegment(&game,                      /* Left edge:  */
					      Vector3(0.0, 1.0, 0.0),     /*   top        */
					      Vector3(0.0, 0.0, 0.0),     /*   bottom     */
					      Vector4(1.0, 1.0, 1.0, 1.0) /*   color        */
					      ));

  game.objects.push_back(new PlayfieldSegment(&game,                      /* Right edge:  */
					      Vector3(1.0, 1.0, 0.0),     /*   top        */
					      Vector3(1.0, 0.0, 0.0),     /*   bottom     */
					      Vector4(1.0, 1.0, 1.0, 1.0) /*   color        */
					      ));

  game.objects.push_back(new HumanPaddle(&game,                      /* Human player:  */
					 Vector3(0.0, 0.0, 0.0),     /*   origin       */
					 Vector3(1.0, 0.0, 0.0),     /*   direction    */
					 0.2,                        /*   width        */
					 0.01,                       /*   thickness    */
					 Vector4(0.0, 1.0, 0.0, 1.0) /*   color        */
					 ));

  Ball *b = new Ball(&game, Vector4(1.0, 1.0, 0.0, 0.0));
  game.objects.push_back(b);

  game.run();
  return 0;
}
