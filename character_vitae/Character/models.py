from character_vitae.MetaCharacter import models as MetaCharacter
from django.contrib.auth.models import User
from django.db import models

ALIGNMENTS = (
        ("CE", "Chaotic Evil"),
        ("CN", "Chaotic Neutral"),
        ("CG", "Chaotic Good"),
        ("NE", "Neutral Evil"),
        ("NN", "Neutral"),
        ("NG", "Neutral Good"),
        ("LE", "Lawful Evil"),
        ("LN", "Lawful Neutral"),
        ("LG", "Lawful Good"),
)

SIZES = (
        ("S", "Small"),
        ("M", "Medium"),
        ("L", "Large"),
)

GENDER = (
        ("M", "Male"),
        ("F", "Female"),
)

class Character(models.Model):
    # Basic info
    player = models.ForeignKey(User)
    name = models.CharField(maxlength=128)
    race = models.ForeignKey(MetaCharacter.Race)
    alignment = models.CharField(maxlength=2, choices=ALIGNMENTS)
    size = models.CharField(maxlength=2, choices=SIZES)
    gender = models.CharField(maxlength=1, choices=GENDER)

    # Abilities
    strength = models.IntegerField()
    dexterity = models.IntegerField()
    constitution = models.IntegerField()
    intelligence = models.IntegerField()
    wisdom = models.IntegerField()
    charisma = models.IntegerField()

    # Saving throws
    fortitude = models.IntegerField()
    reflex = models.IntegerField()
    will = models.IntegerField()

    attack = models.IntegerField()
    spell_resist = models.IntegerField()

    # Optional information
    deity = models.CharField(maxlength=64)
    age = models.IntegerField(null=True, blank=True)
    height = models.IntegerField(null=True, blank=True)
    weight = models.IntegerField(null=True, blank=True)
    eye_color = models.CharField(maxlength=32)
    hair_color = models.CharField(maxlength=32)
    skin_color = models.CharField(maxlength=32)

    def __str__(self):
        return str(self.name)

    class Admin:
        list_display = ["player", "name"]

class Skill(models.Model):
    character = models.ForeignKey(Character, edit_inline=models.TABULAR,
            related_name="skills")
    skill = models.ForeignKey(MetaCharacter.Skill)
    ranks = models.IntegerField(core=True)
    misc_mod = models.IntegerField()

    def get_modifier(self):
        return self.ranks + self.misc_mod

    modifier = property(get_modifier)

    def __str__(self):
        return str(self.modifier)

# vim: ts=4:sw=4:et
