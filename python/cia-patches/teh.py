from LibCIA.IRC.Bots import Bot
channelList = ["#tacobeam", "#commits", "#bzflag"]
def pm(bot, user, channel, message):
    if channel.lower() in channelList:
        m = message.lower()
        if m.startswith("teh ") or m.find(" teh ") > 0:
            bot.me(channel, "smacks %s" % user.split("!")[0])

Bot.privmsg = pm

