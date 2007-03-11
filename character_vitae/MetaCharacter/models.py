from django.db import models

ABILITIES = (
        ("Str", "Strength"),
        ("Dex", "Dexterity"),
        ("Con", "Constitution"),
        ("Int", "Intelligence"),
        ("Wis", "Wisdom"),
        ("Cha", "Charisma"),
)

class Skills(models.Model):
    name = models.CharField(maxlength=64, primary_key=True)
    modifier = models.CharField(maxlength=3, choices=ABILITIES)

    def __str__(self):
        return str(self.name)

    class Admin:
        pass

class Class(models.Model):
    name = models.CharField(maxlength=64, primary_key=True)
    class_skills = models.ManyToManyField(Skills)
    hit_dice = models.IntegerField()
    skill_pts = models.IntegerField()

    def __str__(self):
        return str(self.name)

    class Admin:
        pass

class Race(models.Model):
    name = models.CharField(maxlength=64, primary_key=True)
    size = models.CharField(maxlength=64)
    speed = models.IntegerField()
    favored_class = models.ForeignKey(Class)

    def __str__(self):
        return str(self.name)

    class Admin:
        pass

class RaceTrait(models.Model):
    race = models.ForeignKey(Race, edit_inline=models.TABULAR)
    description = models.CharField(maxlength=128, core=True)
    full_txt = models.TextField(null=True, blank=True)

    def __str__(self):
        return str(self.description)

class RaceAbilityMods(models.Model):
    race = models.ForeignKey(Race, edit_inline=models.TABULAR)
    ability = models.CharField(maxlength=3, choices=ABILITIES, core=True)
    modifier = models.IntegerField(core=True)

    def __str__(self):
        return "%s %d" % (self.ability, self.modifier)

# vim: ts=4:sw=4:et
