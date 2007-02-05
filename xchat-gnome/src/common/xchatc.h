#ifndef XCHAT_C_H
#define XCHAT_C_H

extern struct xchatprefs prefs;

extern int xchat_is_quitting;
extern gint arg_skip_plugins;	/* command-line args */
extern gint arg_dont_autoconnect;
extern char *arg_url;
extern gint arg_existing;

extern session *current_sess;
extern session *current_tab;

extern GList *command_list;
extern GList *ctcp_list;
extern GList *replace_list;
extern GSList *sess_list;
extern GSList *dcc_list;
extern GSList *ignore_list;

session * find_channel (server *serv, char *chan);
session * find_dialog (server *serv, char *nick);
session * new_ircwindow (server *serv, char *name, int type, int focus);
int is_session (session * sess);
void session_free (session *killsess);
void lag_check (void);
void xchat_exit (void);
void xchat_exec (const char *cmd);
void xchat_execv (char * const argv[]);

#endif
