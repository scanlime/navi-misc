MODS = \
	cbot_irc_behaviour \
	cbot_irc_connection \
	cbot_irc_protocol \
	cbot_irc_supervisor \
	cbot_logger \
	cbot_socket \
	test

SRCDIR = src
BINDIR = ebin
EFLAGS = -W

OBJS = ${MODS:%=$(BINDIR)/%.beam}

all: $(OBJS)

$(BINDIR)/%.beam: $(SRCDIR)/%.erl
	erlc -o $(BINDIR) $(EFLAGS) $<

shell:
	erl -pa $(BINDIR)

clean:
	rm -f $(OBJS)

