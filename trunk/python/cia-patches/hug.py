from LibCIA.IRC.Bots import Bot
def hug_action(bot, user, channel, message):
    if message.lower().strip() == "hugs %s" % bot.nickname.lower():
        bot.me(channel, "hugs %s" % user.split("!")[0])

Bot.action = hug_action


