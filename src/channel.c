/*
 * RtgChannel - The channel object retrieves and stores timestamped
 *              scalar values. One or more channels are used to draw
 *              the content in a graph. Each channel maintains
 *              its own history buffer, as each channel can have an
 *              independent sampling rate and history length.
 *
 * rtgraph real-time graphing package
 * Copyright (C) 2004 Micah Dowty <micah@navi.cx>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

static void rtg_channel_class_init(RtgChannelClass *klass);
static void rtg_channel_init(RtgChannel *self);

/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType rtg_channel_get_type(void) {
  static GType im_type = 0;
  if (!im_type) {
    static const GTypeInfo im_info = {
      sizeof(RtgChannelClass),
      NULL, /* base init */
      NULL, /* base finalize */
      (GClassInitFunc) rtg_channel_class_init,
      NULL, /* class finalize */
      NULL, /* class data */
      sizeof(RtgChannel),
      0,
      (GInstanceInitFunc) rtg_channel_init,
    };

    im_type = g_type_register_static(RTG_CHANNEL_TYPE, "RtgChannel", &im_info, 0);
  }

  return im_type;
}

static void rtg_channel_class_init(RtgChannelClass *klass) {
  /* Nothing to do here */
}

static void rtg_channel_init(RtgChannel *self) {
  /* Nothing to do here */
}

RtgChannel* animation_new() {
  return RTG_CHANNEL(g_object_new(rtg_channel_get_type(), NULL));
}

/* The End */
