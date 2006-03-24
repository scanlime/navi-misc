/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * parameter-editor.c - Automatically constructs a GUI for editing the
 *                      parameters of a ParameterHolder instance.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2006 David Trowbridge and Micah Dowty
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

#include "parameter-editor.h"
#include "color-button.h"
#include <gtk/gtk.h>
#include <string.h>

static void parameter_editor_class_init(ParameterEditorClass *klass);
static void parameter_editor_init(ParameterEditor *self);
static void parameter_editor_finalize(GObject *object);

static void parameter_editor_attach(ParameterEditor *self, ParameterHolder *holder);
static void parameter_editor_add_paramspec(ParameterEditor *self, GParamSpec *spec);
static void parameter_editor_add_group_heading(ParameterEditor *self, const gchar *group);
static void parameter_editor_add_row(ParameterEditor *self, GParamSpec *spec, GtkWidget *row);
static void parameter_editor_add_dependency(ParameterEditor *self, GtkWidget *widget, const gchar *dependency_name);
static void parameter_editor_add_labeled_row(ParameterEditor *self, GParamSpec *spec, GtkWidget *row);

static void parameter_editor_connect_notify(ParameterEditor *self,
					    GtkWidget       *widget,
					    const gchar     *property_name,
					    GCallback        func);

static void parameter_editor_add_numeric(ParameterEditor *self, GParamSpec *spec);
static void parameter_editor_add_color(ParameterEditor *self, GParamSpec *spec);
static void parameter_editor_add_boolean(ParameterEditor *self, GParamSpec *spec);
static void parameter_editor_add_enum(ParameterEditor *self, GParamSpec *spec);

static void on_changed_numeric(GtkWidget *widget, ParameterEditor *self);
static void on_changed_color(GtkWidget *widget, ParameterEditor *self);
static void on_changed_boolean(GtkWidget *widget, ParameterEditor *self);
static void on_changed_enum(GtkWidget *widget, ParameterEditor *self);

static void on_notify_numeric(ParameterHolder *holder, GParamSpec *spec, GtkWidget *widget);
static void on_notify_color(ParameterHolder *holder, GParamSpec *spec, GtkWidget *widget);
static void on_notify_opacity(ParameterHolder *holder, GParamSpec *spec, GtkWidget *widget);
static void on_notify_boolean(ParameterHolder *holder, GParamSpec *spec, GtkWidget *widget);
static void on_notify_dependency(ParameterHolder *holder, GParamSpec *spec, GtkWidget *widget);
static void on_notify_enum(ParameterHolder *holder, GParamSpec *spec, GtkWidget *widget);


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType parameter_editor_get_type(void) {
    static GType pe_type = 0;

    if (!pe_type) {
	static const GTypeInfo pe_info = {
	    sizeof(ParameterEditorClass),
	    NULL, /* base_init */
	    NULL, /* base_finalize */
	    (GClassInitFunc) parameter_editor_class_init,
	    NULL, /* class_finalize */
	    NULL, /* class_data */
	    sizeof(ParameterEditor),
	    0,
	    (GInstanceInitFunc) parameter_editor_init,
	};

	pe_type = g_type_register_static(GTK_TYPE_VBOX, "ParameterEditor", &pe_info, 0);
    }

    return pe_type;
}

static void parameter_editor_class_init(ParameterEditorClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->finalize = parameter_editor_finalize;
}

static void parameter_editor_init(ParameterEditor *self) {
    self->label_sizegroup = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
}

static void parameter_editor_finalize(GObject *object) {
    ParameterEditor *self = PARAMETER_EDITOR(object);

    if (self->holder) {
	g_object_unref(self->holder);
	self->holder = NULL;
    }

    if (self->label_sizegroup) {
	g_object_unref(self->label_sizegroup);
	self->label_sizegroup = NULL;
    }

    if (self->previous_group) {
	g_free(self->previous_group);
	self->previous_group = NULL;
    }
}

GtkWidget* parameter_editor_new(ParameterHolder *holder) {
    ParameterEditor *self = g_object_new(parameter_editor_get_type(), NULL);
    parameter_editor_attach(self, holder);
    return GTK_WIDGET(self);
}


/************************************************************************************/
/********************************************************************* GUI Building */
/************************************************************************************/

static void parameter_editor_attach(ParameterEditor *self, ParameterHolder *holder) {
    /* Attach this parameter editor to a holder, adding widgets for
     * each paramspec in the holder with a PARAM_IN_GUI flag
     */
    GParamSpec** properties;
    guint n_properties;
    int i;

    self->holder = g_object_ref(holder);

    properties = g_object_class_list_properties(G_OBJECT_GET_CLASS(holder), &n_properties);
    for (i=0; i<n_properties; i++) {
	if (properties[i]->flags & PARAM_IN_GUI)
	    parameter_editor_add_paramspec(self, properties[i]);
    }
    g_free(properties);
}

static void parameter_editor_add_paramspec(ParameterEditor *self, GParamSpec *spec) {
    const gchar *group;

    /* Get this parameter's group name, adding a new group header if it's changed */
    group = param_spec_get_group(spec);
    if (group) {
	if ((!self->previous_group) || strcmp((void *) group, self->previous_group))
	    parameter_editor_add_group_heading(self, group);
	if (self->previous_group)
	    g_free(self->previous_group);
	self->previous_group = g_strdup(group);
    }

    /* Pick a type-dependent procedure for adding this parameter to the GUI */

    if (spec->value_type == G_TYPE_DOUBLE)
	parameter_editor_add_numeric(self, spec);

    else if (spec->value_type == G_TYPE_UINT)
	parameter_editor_add_numeric(self, spec);

    else if (spec->value_type == GDK_TYPE_COLOR)
	parameter_editor_add_color(self, spec);

    else if (spec->value_type == G_TYPE_BOOLEAN)
	parameter_editor_add_boolean(self, spec);

    else if (g_type_is_a (spec->value_type, G_TYPE_ENUM))
	parameter_editor_add_enum (self, spec);

    else
	g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "Can't edit values of type %s",
	      g_type_name(spec->value_type));
}

static void parameter_editor_add_group_heading(ParameterEditor *self, const gchar *group) {
    GtkWidget *label;
    gchar *markup;

    /* Add a separator if this isn't the first group */
    if (self->previous_group)
	gtk_box_pack_start(GTK_BOX(self), gtk_hseparator_new(), FALSE, FALSE, 4);

    /* Make a label with the group name bold and centered */
    markup = g_strdup_printf("<b>%s</b>", group);
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), markup);
    g_free(markup);
    gtk_box_pack_start(GTK_BOX(self), label, FALSE, FALSE, 4);
}

static void parameter_editor_add_row(ParameterEditor *self, GParamSpec *spec, GtkWidget *row) {
    const gchar *dep;

    gtk_box_pack_start(GTK_BOX(self), row, FALSE, FALSE, 2);

    dep = param_spec_get_dependency(spec);
    if (dep)
	parameter_editor_add_dependency(self, row, dep);
}

static void parameter_editor_add_labeled_row(ParameterEditor *self, GParamSpec *spec, GtkWidget *row) {
    GtkWidget *hbox, *label;
    gchar *text;

    hbox = gtk_hbox_new(FALSE, 0);
    text = g_strdup_printf("%s:", g_param_spec_get_nick(spec));
    label = gtk_label_new(text);
    g_free(text);
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
    gtk_size_group_add_widget(self->label_sizegroup, label);
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 6);
    gtk_box_pack_start(GTK_BOX(hbox), row, TRUE, TRUE, 6);

    parameter_editor_add_row(self, spec, hbox);
}

static void parameter_editor_add_dependency(ParameterEditor *self, GtkWidget *widget, const gchar *dependency_name) {
    /* Add a notify callback to our dependency that enables or disables the given widget.
     * Call the notify callback once right away to set up our initial sensitivity.
     */
    gchar *signal_name;
    GParamSpec *spec;

    signal_name = g_strdup_printf("notify::%s", dependency_name);
    g_signal_connect(self->holder, signal_name, G_CALLBACK(on_notify_dependency), widget);
    g_free(signal_name);

    spec = g_object_class_find_property(G_OBJECT_GET_CLASS(self->holder), dependency_name);
    on_notify_dependency(self->holder, spec, widget);
}


/************************************************************************************/
/********************************************************************* Type Editors */
/************************************************************************************/

static void parameter_editor_connect_notify(ParameterEditor *self,
					    GtkWidget       *widget,
					    const gchar     *property_name,
					    GCallback        func) {
    /* Connect to the notify::<property> signal so we can update the
     * GUI automatically when someone else changes the property. The
     * callback needs to know which widget is associated with this property,
     * and it needs a pointer to our ParameterEditor instance. We do this by
     * passing the widget as user_data but using g_object_set_data to
     * attach the ParameterEditor to the widget.
     */
    gchar *signal_name;
    signal_name = g_strdup_printf("notify::%s", property_name);
    g_object_set_data(G_OBJECT(widget), "ParameterEditor", self);
    g_signal_connect(self->holder, signal_name, func, widget);
    g_free(signal_name);
}

static void parameter_editor_add_numeric(ParameterEditor *self, GParamSpec *spec) {
    GtkWidget *spinner;
    GtkObject *adjustment;
    gdouble climb_rate = 0.1;
    int digits = 1;
    gdouble value = 0;
    gdouble upper = 1;
    gdouble lower = 0;
    gdouble step_increment = 0.1;
    gdouble page_increment = 0.1;
    gdouble page_size = 0;
    const ParameterIncrements *increments;
    GValue gv, double_gv;

    /* Look for upper and lower bounds in the GParamSpec to override our defaults */
    if (G_IS_PARAM_SPEC_DOUBLE(spec)) {
	GParamSpecDouble *s = G_PARAM_SPEC_DOUBLE(spec);
	upper = s->maximum;
	lower = s->minimum;
    }
    else if (G_IS_PARAM_SPEC_UINT(spec)) {
	GParamSpecUInt *s = G_PARAM_SPEC_UINT(spec);
	upper = s->maximum;
	lower = s->minimum;
    }

    /* Look for a ParameterIncrements structure attached to this parameter.
     * If we find one, set the increments and number of digits from it.
     */
    increments = param_spec_get_increments(spec);
    if (increments) {
	digits = increments->digits;
	step_increment = increments->step;
	page_increment = increments->page;
	climb_rate = increments->step;
    }

    /* Get the parameter's current value */
    memset(&gv, 0, sizeof(gv));
    memset(&double_gv, 0, sizeof(double_gv));
    g_value_init(&gv, spec->value_type);
    g_value_init(&double_gv, G_TYPE_DOUBLE);
    g_object_get_property(G_OBJECT(self->holder), spec->name, &gv);
    g_value_transform(&gv, &double_gv);
    value = g_value_get_double(&double_gv);
    g_value_unset(&gv);
    g_value_unset(&double_gv);

    adjustment = gtk_adjustment_new(value, lower, upper, step_increment, page_increment, page_size);
    spinner = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), climb_rate, digits);

    /* Set up our callback on change */
    g_object_set_data(G_OBJECT(spinner), "ParamSpec", spec);
    g_signal_connect(spinner, "value-changed", G_CALLBACK(on_changed_numeric), self);

    parameter_editor_connect_notify(self, spinner, spec->name, G_CALLBACK(on_notify_numeric));
    parameter_editor_add_labeled_row(self, spec, spinner);
}

static void parameter_editor_add_color(ParameterEditor *self, GParamSpec *spec) {
    GtkWidget *color_button;
    const gchar* opacity_property;
    guint16 opacity = 0xFFFF;
    GdkColor color = {0,0,0,0};
    GValue gv;

    /* See if this color property has a matching opacity property */
    opacity_property = g_param_spec_get_qdata(spec, g_quark_from_static_string("opacity-property"));
    if (opacity_property) {

	/* Get the current opacity */
	memset(&gv, 0, sizeof(gv));
	g_value_init(&gv, G_TYPE_UINT);
	g_object_get_property(G_OBJECT(self->holder), opacity_property, &gv);
	opacity = g_value_get_uint(&gv);
	g_value_unset(&gv);
    }

    /* Get the current color */
    memset(&gv, 0, sizeof(gv));
    g_value_init(&gv, GDK_TYPE_COLOR);
    g_object_get_property(G_OBJECT(self->holder), spec->name, &gv);
    color = *(GdkColor*)g_value_get_boxed(&gv);
    g_value_unset(&gv);

    color_button = color_button_new_with_defaults(g_param_spec_get_nick(spec), &color, opacity);

    /* Set up our callback on change */
    g_object_set_data(G_OBJECT(color_button), "ParamSpec", spec);
    g_signal_connect(color_button, "changed", G_CALLBACK(on_changed_color), self);

    parameter_editor_connect_notify(self, color_button, spec->name, G_CALLBACK(on_notify_color));
    if (opacity_property)
	parameter_editor_connect_notify(self, color_button, opacity_property, G_CALLBACK(on_notify_opacity));

    parameter_editor_add_labeled_row(self, spec, color_button);
}

static void parameter_editor_add_boolean(ParameterEditor *self, GParamSpec *spec) {
    GtkWidget *check;
    GValue gv;

    check = gtk_check_button_new();

    /* Get the parameter's current value */
    memset(&gv, 0, sizeof(gv));
    g_value_init(&gv, spec->value_type);
    g_object_get_property(G_OBJECT(self->holder), spec->name, &gv);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), g_value_get_boolean(&gv));
    g_value_unset(&gv);

    /* Set up our callback on change */
    g_object_set_data(G_OBJECT(check), "ParamSpec", spec);
    g_signal_connect(check, "toggled", G_CALLBACK(on_changed_boolean), self);

    parameter_editor_connect_notify(self, check, spec->name, G_CALLBACK(on_notify_boolean));
    parameter_editor_add_labeled_row(self, spec, check);
}

static void parameter_editor_add_enum(ParameterEditor *self, GParamSpec *spec) {
    GtkWidget *combo;
    GValue gv;
    GEnumClass *klass;
    gint i;
#if (GTK_MINOR_VERSION < 4)
    GtkWidget *menu;
#endif

    klass = (GEnumClass*) g_type_class_ref (spec->value_type);

#if (GTK_MINOR_VERSION >= 4)
    combo = gtk_combo_box_new_text ();
    for (i = 0; i < klass->n_values; i++)
	{
	    GEnumValue *value;

	    value = g_enum_get_value (klass, i);

	    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), value->value_nick);
	}
#else
    combo = gtk_option_menu_new ();
    menu = gtk_menu_new ();

    for (i = 0; i < klass->n_values; i++)
	{
	    GEnumValue *value;

	    value = g_enum_get_value (klass, i);

	    gtk_menu_shell_append (GTK_MENU_SHELL (menu), gtk_menu_item_new_with_label (value->value_nick));
	}

    gtk_option_menu_set_menu (GTK_OPTION_MENU (combo), menu);
#endif

    /* Get the parameter's current value */
    memset (&gv, 0, sizeof (gv));
    g_value_init (&gv, spec->value_type);
    g_object_get_property (G_OBJECT (self->holder), spec->name, &gv);
#if (GTK_MINOR_VERSION >= 4)
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), g_value_get_enum (&gv));
#else
    gtk_option_menu_set_history (GTK_OPTION_MENU (combo), g_value_get_enum (&gv));
#endif
    g_value_unset (&gv);

    /* Set up our callback on change */
    g_object_set_data (G_OBJECT (combo), "ParamSpec", spec);
    g_signal_connect (combo, "changed", G_CALLBACK (on_changed_enum), self);

    parameter_editor_connect_notify (self, combo, spec->name, G_CALLBACK (on_notify_enum));
    parameter_editor_add_labeled_row (self, spec, combo);
}


/************************************************************************************/
/***************************************************************** Widget callbacks */
/************************************************************************************/

static void on_changed_numeric(GtkWidget *widget, ParameterEditor *self) {
    GParamSpec *spec = g_object_get_data(G_OBJECT(widget), "ParamSpec");
    GValue gv, double_gv;

    if (self->suppress_changed)
	return;

    /* Convert the current spinner value from a double to whatever type we need,
     * and set the property from that converted value.
     */
    memset(&gv, 0, sizeof(gv));
    memset(&double_gv, 0, sizeof(double_gv));
    g_value_init(&gv, spec->value_type);
    g_value_init(&double_gv, G_TYPE_DOUBLE);
    g_value_set_double(&double_gv, gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget)));
    g_value_transform(&double_gv, &gv);

    self->suppress_notify = TRUE;
    g_object_set_property(G_OBJECT(self->holder), spec->name, &gv);
    self->suppress_notify = FALSE;

    g_value_unset(&gv);
    g_value_unset(&double_gv);
}

static void on_changed_color(GtkWidget *widget, ParameterEditor *self) {
    GParamSpec *spec = g_object_get_data(G_OBJECT(widget), "ParamSpec");
    GdkColor c;
    const gchar* opacity_property;
    guint opacity;

    if (self->suppress_changed)
	return;

    opacity_property = g_param_spec_get_qdata(spec, g_quark_from_static_string("opacity-property"));
    color_button_get_color(COLOR_BUTTON(widget), &c);
    opacity = color_button_get_alpha(COLOR_BUTTON(widget));

    /* Set both color and opacity if we have both. If we don't have
     * an opacity property, opacity_property will be NULL and the opacity
     * parameter will be ignored.
     */
    self->suppress_notify = TRUE;
    g_object_set(self->holder,
		 spec->name, &c,
		 opacity_property, opacity,
		 NULL);
    self->suppress_notify = FALSE;
}

static void on_changed_boolean(GtkWidget *widget, ParameterEditor *self) {
    GParamSpec *spec = g_object_get_data(G_OBJECT(widget), "ParamSpec");
    gboolean active;

    if (self->suppress_changed)
	return;

    active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    self->suppress_notify = TRUE;
    g_object_set(self->holder,
		 spec->name, active,
		 NULL);
    self->suppress_notify = FALSE;
}

static void on_changed_enum(GtkWidget *widget, ParameterEditor *self) {
    GParamSpec *spec = g_object_get_data (G_OBJECT (widget), "ParamSpec");
    gint active;

    if (self->suppress_changed)
	return;

#if (GTK_MINOR_VERSION >= 4)
    active = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));
#else
    active = gtk_option_menu_get_history (GTK_OPTION_MENU (widget));
#endif

    self->suppress_notify = TRUE;
    g_object_set (self->holder,
		  spec->name,
		  active,
		  NULL);
    self->suppress_notify = FALSE;
}


/************************************************************************************/
/***************************************************************** Notify callbacks */
/************************************************************************************/

static void on_notify_numeric(ParameterHolder *holder, GParamSpec *spec, GtkWidget *widget) {
    ParameterEditor *self = g_object_get_data(G_OBJECT(widget), "ParameterEditor");
    GValue gv, double_gv;

    if (self->suppress_notify)
	return;

    /* Conver the current property value to a double and set our spin button */
    memset(&gv, 0, sizeof(gv));
    memset(&double_gv, 0, sizeof(double_gv));
    g_value_init(&gv, spec->value_type);
    g_value_init(&double_gv, G_TYPE_DOUBLE);
    g_object_get_property(G_OBJECT(holder), spec->name, &gv);
    g_value_transform(&gv, &double_gv);

    self->suppress_changed = TRUE;
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), g_value_get_double(&double_gv));
    self->suppress_changed = FALSE;

    g_value_unset(&gv);
    g_value_unset(&double_gv);
}

static void on_notify_color(ParameterHolder *holder, GParamSpec *spec, GtkWidget *widget) {
    ParameterEditor *self = g_object_get_data(G_OBJECT(widget), "ParameterEditor");
    GdkColor *c;

    if (self->suppress_notify)
	return;

    g_object_get(holder, spec->name, &c, NULL);

    self->suppress_changed = TRUE;
    color_button_set_color(COLOR_BUTTON(widget), c);
    self->suppress_changed = FALSE;
}

static void on_notify_opacity(ParameterHolder *holder, GParamSpec *spec, GtkWidget *widget) {
    ParameterEditor *self = g_object_get_data(G_OBJECT(widget), "ParameterEditor");
    guint opacity;

    if (self->suppress_notify)
	return;

    g_object_get(holder, spec->name, &opacity, NULL);

    self->suppress_changed = TRUE;
    color_button_set_alpha(COLOR_BUTTON(widget), opacity);
    self->suppress_changed = FALSE;
}

static void on_notify_boolean(ParameterHolder *holder, GParamSpec *spec, GtkWidget *widget) {
    ParameterEditor *self = g_object_get_data(G_OBJECT(widget), "ParameterEditor");
    gboolean active;

    if (self->suppress_notify)
	return;

    g_object_get(holder, spec->name, &active, NULL);

    self->suppress_changed = TRUE;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), active);
    self->suppress_changed = FALSE;
}

static void on_notify_dependency(ParameterHolder *holder, GParamSpec *spec, GtkWidget *widget) {
    gboolean active;
    g_object_get(holder, spec->name, &active, NULL);
    gtk_widget_set_sensitive(widget, active);
}

static void on_notify_enum(ParameterHolder *holder, GParamSpec *spec, GtkWidget *widget) {
    ParameterEditor *self = g_object_get_data (G_OBJECT (widget), "ParameterEditor");
    GValue gv;

    if (self->suppress_notify)
	return;

    memset (&gv, 0, sizeof (gv));
    g_value_init (&gv, spec->value_type);
    g_object_get_property (G_OBJECT (holder), spec->name, &gv);

    self->suppress_changed = TRUE;
#if (GTK_MINOR_VERSION >= 4)
    gtk_combo_box_set_active (GTK_COMBO_BOX (widget), g_value_get_enum (&gv));
#else
    gtk_option_menu_set_history (GTK_OPTION_MENU (widget), g_value_get_enum (&gv));
#endif
    self->suppress_changed = FALSE;

    g_value_unset (&gv);
}


/* The End */
