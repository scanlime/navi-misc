/*
 * Authors: David Trowbridge <trowbrds@cs.colorado.edu>
 *
 * Copyright (C) 2005 Novell, Inc. (www.novell.com)
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "e-cal-event.h"

static GObjectClass *ece_parent;

static void
ece_init (GObject *o)
{
}

static void
ece_finalize (GObject *o)
{
	((GObjectClass *) ece_parent)->finalize (o);
}

static void
ece_target_free (EEvent *ev, EEventTarget *t)
{
	switch (t->type) {
	case E_CAL_EVENT_TARGET_SOURCE: {
		ECalEventTargetSource *s = (ECalEventTargetSource *) t;
		if (s->source)
			g_object_unref (s->source);
		break; }
	}
}

static void
ece_class_init (GObjectClass *klass)
{
	klass->finalize = ece_finalize;
	((EEventClass *)klass)->target_free = ece_target_free;
}

GType
e_cal_event_get_type (void)
{
	static GType type = 0;

	if (!type) {
		static const GTypeInfo info = {
			sizeof (ECalEventClass),
			NULL, NULL,
			(GClassInitFunc) ece_class_init,
			NULL, NULL,
			sizeof (ECalEvent), 0,
			(GInstanceInitFunc) ece_init
		};
		ece_parent = g_type_class_ref (e_event_get_type ());
		type = g_type_register_static (e_event_get_type (), "ECalEvent", &info, 0);
	}

	return type;
}

ECalEvent *
e_cal_event_peek (void)
{
	static ECalEvent *e_cal_event = NULL;
	if (!e_cal_event) {
		e_cal_event = g_object_new (e_cal_event_get_type (), 0);
		e_event_construct (&e_cal_event->event, "org.gnome.evolution.calendar.events");
	}
	return e_cal_event;
}

ECalEventTargetSource *
e_cal_event_target_new_source (ECalEvent *ece, struct _ESource *source, guint32 flags)
{
	ECalEventTargetSource *t = e_event_target_new (&ece->event, E_CAL_EVENT_TARGET_SOURCE, sizeof (*t));

	t->source = g_object_ref (source);
	t->target.mask = ~flags;

	return t;
}

ECalEventTargetComponent *
e_cal_event_target_new_component (ECalEvent *ece, struct _CalendarComponent *component, guint32 flags)
{
	ECalEventTargetComponent *t = e_event_target_new (&ece->event, E_CAL_EVENT_TARGET_COMPONENT, sizeof (*t));

	t->component = g_object_ref (component);
	t->target.mask = ~flags;

	return t;
}

/* ********************************************************************** */

static void *eceh_parent_class;

static const EEventHookTargetMask eceh_source_masks[] = {
	{ "changed", E_CAL_EVENT_SOURCE_CALENDAR_CHANGED },
	{ 0 },
};

static const EEventHookTargetMask eceh_component_masks[] = {
	{ "initialized", E_CAL_EVENT_COMPONENT_INITIALIZED },
	{ 0 },
};

static const EEventHookTargetMap eceh_targets[] = {
	{ "source", E_CAL_EVENT_TARGET_SOURCE, eceh_source_masks },
	{ "component", E_CAL_EVENT_TARGET_COMPONENT, eceh_component_masks },
	{ 0 },
};

static void
eceh_finalize (GObject *o)
{
	((GObjectClass *) eceh_parent_class)->finalize (o);
}

static void
eceh_class_init (EPluginHookClass *klass)
{
	GObjectClass *object_class = (GObjectClass *) klass;

	object_class->finalize = eceh_finalize;
}

GType
e_cal_event_hook_get_type ()
{
	static GType type = 0;

	if (!type) {
		static const GTypeInfo info = {
			sizeof (ECalEventHookClass),
			NULL, NULL,
			(GClassInitFunc) eceh_class_init,
			NULL, NULL,
			sizeof (ECalEventHook), 0,
			(GInstanceInitFunc) NULL,
		};

		eceh_parent_class = g_type_class_ref (e_event_hook_get_type ());
		type = g_type_register_static (e_event_hook_get_type (), "ECalEventHook", &info, 0);
	}

	return type;
}
