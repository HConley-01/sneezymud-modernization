/////////////////////////////////////////////////////////////////
//
//     spell_parser.cc : All functions related to spell parsing
//
//     Copyright (c) 1998, SneezyMUD Development Team
//     All Rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "handler.h"
#include "extern.h"
#include "room.h"
#include "being.h"
#include "low.h"
#include "monster.h"
#include "disc_shaman_armadillo.h"
#include "disc_shaman_healing.h"
#include "disc_mage_air.h"
#include "disc_mage_alchemy.h"
#include "disc_mage_earth.h"
#include "disc_mage_fire.h"
#include "disc_mage_sorcery.h"
#include "disc_mage_spirit.h"
#include "disc_mage_water.h"
#include "disc_cleric_aegis.h"
#include "disc_cleric_wrath.h"
#include "disc_shaman.h"
#include "disc_cleric_afflictions.h"
#include "disc_cleric_cures.h"
#include "disc_cleric_hand_of_god.h"
#include "disc_deikhan_guardian.h"
#include "disc_ranger_animal.h"
#include "disc_ranger_nature.h"
#include "disc_advanced_ranged.h"
#include "disc_basic_adventuring.h"
#include "disc_shaman_frog.h"
#include "disc_shaman_spider.h"
#include "disc_shaman_skunk.h"
#include "disc_shaman_control.h"
#include "disc_shaman_alchemy.h"
#include "disc_shaman.h"
#include "obj_opal.h"
#include "weather.h"

#include <boost/algorithm/string.hpp>

std::tuple<spellNumT, sstring> TBeing::parseSpellNum(const sstring& base, const sstring& args) const {

  if (base.trim() == "")
    return std::make_tuple(TYPE_UNDEFINED, args);

  spellNumT found = searchForSpellNum(base, EXACT_NO, TRUE);
  if (found != TYPE_UNDEFINED)
    return std::make_tuple(found, args.trimLeft());

  return parseSpellNum(base.dropLastWord(), " " + base.lastWord() + args);
}

int TBeing::useMana(spellNumT spl) {
  int arrayMana;
  int rounds;
  spl = getSkillNum(spl);
  discNumT das = getDisciplineNumber(spl, FALSE);
  if (das == DISC_NONE) {
    vlogf(LOG_BUG, format("useMana() with bad discipline for spell=%d") % spl);
    return 0;
  }

  arrayMana =
    getDiscipline(das)->useMana(getSkillValue(spl), discArray[spl]->minMana);

  // divide total mana/rounds for each spell if spell tasked
  if (IS_SET(discArray[spl]->comp_types, SPELL_TASKED) &&
      discArray[spl]->lag >= 0) {
    rounds = discArray[spl]->lag + 2;
    return arrayMana / rounds;
  } else {
    return arrayMana;
  }
}

// LIFEFORCE
int TBeing::useLifeforce(spellNumT spl) {
  int arrayLifeforce;
  int rounds;
  int temp;
  int temper;
  spl = getSkillNum(spl);
  discNumT das = getDisciplineNumber(spl, FALSE);
  if (das == DISC_NONE) {
    vlogf(LOG_BUG,
      format("useLifeforce() with bad discipline for spell=%d") % spl);
    return 0;
  }
  temp = discArray[spl]->minLifeforce;
  temper = getSkillValue(spl);
  arrayLifeforce = getDiscipline(das)->useLifeforce(temper, temp);
  //  arrayLifeforce =
  //  getDiscipline(das)->useLifeforce(getSkillValue(spl),discArray[spl]->minLifeforce);

  // divide total LF/rounds for each spell if spell tasked
  if (IS_SET(discArray[spl]->comp_types, SPELL_TASKED) &&
      discArray[spl]->lag >= 0) {
    rounds = discArray[spl]->lag + 2;
    return arrayLifeforce / rounds;
  } else {
    return arrayLifeforce;
  }
}

// END LIFEFORCE

double TBeing::usePiety(spellNumT spl) {
  double arrayPiety;
  int rounds;

  spl = getSkillNum(spl);
  discNumT das = getDisciplineNumber(spl, FALSE);
  if (das == DISC_NONE) {
    vlogf(LOG_BUG, format("usePiety() with bad discipline for spell=%d") % spl);
    return 0;
  }

  arrayPiety = getDiscipline(das)->usePerc(getSkillValue(spl),
    (double)discArray[spl]->minPiety);

  // divide total piety/rounds for each prayer if prayer tasked
  if (IS_SET(discArray[spl]->comp_types, SPELL_TASKED) &&
      discArray[spl]->lag > 0) {
    rounds = discArray[spl]->lag + 2;
    return arrayPiety / rounds;
  } else {
    return arrayPiety;
  }
}

bool TBeing::circleFollow(const TBeing* victim) const {
  const TBeing* k;

  for (k = victim; k; k = k->master) {
    if (k == this)
      return TRUE;
  }
  return FALSE;
}

void TBeing::stopFollower(bool remove,
  stopFollowerT textLimits)  // default argument
{
  followData *j, *k;
  affectedData aff;

  if (!master)
    return;

  if (affectedBySpell(AFFECT_PET) || affectedBySpell(AFFECT_CHARM) ||
      affectedBySpell(AFFECT_THRALL)) {
    // make pet retrainable
    aff.type = AFFECT_ORPHAN_PET;
    aff.level = 0;
    aff.duration = 80 * Pulse::UPDATES_PER_MUDHOUR;
    aff.location = APPLY_NONE;
    aff.modifier = 0;
    aff.bitvector = 0;
    if (master) {
      char* tmp = mud_str_dup(master->name);
      aff.be = reinterpret_cast<TThing*>(tmp);
    }
    if (roomp) {
      affectTo(&aff, -1);
    } else {
      vlogf(LOG_BUG, format("%s having AFFECT_ORPHAN_PET without a roomp  in "
                            "stop follower, master is %s") %
                       getName() % master->getName());
    }
    // take charm off so text is sent
    REMOVE_BIT(specials.affectedBy, AFF_CHARM | AFF_GROUP);
  }
  if (isAffected(AFF_CHARM)) {
    act("You realize that $N is a jerk!", FALSE, this, 0, master, TO_CHAR);
    if (textLimits == STOP_FOLLOWER_SILENT) {
    } else {
      act("$n realizes that $N is a jerk!", FALSE, this, 0, master, TO_NOTVICT);
      act("$n hates your guts!", FALSE, this, 0, master, TO_VICT);
    }
    if (affectedBySpell(SPELL_ENSORCER) && remove)
      affectFrom(SPELL_ENSORCER);
    if (affectedBySpell(SPELL_HYPNOSIS) && remove)
      affectFrom(SPELL_HYPNOSIS);
  } else {
    int levelLimit = (isPlayerAction(PLR_STEALTH) ? MAX_MORT : 0);

    act("You stop following $N.", FALSE, this, 0, master, TO_CHAR);
    if (textLimits == STOP_FOLLOWER_SILENT) {
    } else if (textLimits == STOP_FOLLOWER_DEFAULT) {
      act("$n stops following $N.", FALSE, this, 0, master, TO_NOTVICT, NULL,
        levelLimit);
      act("$n stops following you.", FALSE, this, 0, master, TO_VICT, NULL,
        levelLimit);
    } else if (textLimits == STOP_FOLLOWER_CHAR_NOTVICT) {
      act("$n stops following $N.", FALSE, this, 0, master, TO_NOTVICT, NULL,
        levelLimit);
    } else if (textLimits == STOP_FOLLOWER_CHAR_VICT) {
      act("$n stops following you.", FALSE, this, 0, master, TO_VICT, NULL,
        levelLimit);
    }
  }
  if (!master->followers) {
    vlogf(LOG_BUG, "master->followers is NULL in stopFollowers");
    REMOVE_BIT(specials.affectedBy, AFF_CHARM | AFF_GROUP);
    master = NULL;
    return;
  }
  if (master->followers->follower == this) {  // Head of follower-list? */
    k = master->followers;
    master->followers = k->next;
    delete k;
  } else {  // locate follower who is not head of list
    for (k = master->followers; k->next && k->next->follower != this;
         k = k->next)
      ;

    if ((j = k->next)) {
      k->next = j->next;
      delete j;
    }
  }
  master = NULL;
  REMOVE_BIT(specials.affectedBy, AFF_CHARM | AFF_GROUP);
}

// Called when a character that follows/is followed dies
void TBeing::dieFollower() {
  followData *j, *k;

  if (master)
    stopFollower(TRUE);

  for (k = followers; k; k = j) {
    j = k->next;
    k->follower->stopFollower(TRUE);
  }
}

void TBeing::addFollower(TBeing* foll, bool textLimits)  // default argument
{
  followData *k, *followIndex;

  if (foll->master) {
    vlogf(LOG_BUG,
      format("add_follower error: this: %s, leader %s, master %s.") %
        foll->getName() % getName() % foll->master->getName());
    foll->master = NULL;
  }
  foll->master = this;

  k = new followData;

  k->follower = foll;

  if (followers) {
    for (followIndex = followers; followIndex && followIndex->next;
         followIndex = followIndex->next)
      ;

    // This is bad and shouldn't happen, but you never know.
    if (!followIndex) {
      k->next = followers;
      followers = k;
    } else {
      k->next = NULL;
      followIndex->next = k;
    }
  } else {
    k->next = followers;
    followers = k;
  }

  int levelLimit = (foll->isPlayerAction(PLR_STEALTH) ? MAX_MORT : 0);

  act("You now follow $N.", FALSE, foll, 0, this, TO_CHAR);
  if (textLimits) {
  } else {
    act("$n starts following you.", TRUE, foll, 0, this, TO_VICT, NULL,
      levelLimit);
    act("$n now follows $N.", TRUE, foll, 0, this, TO_NOTVICT, NULL,
      levelLimit);
  }
  if (desc && IS_SET(desc->autobits, AUTO_AUTOGROUP)) {
    if (!isAffected(AFF_GROUP))
      SET_BIT(specials.affectedBy, AFF_GROUP);
    doGroup(foll->name.c_str(), true);
  }
}

void TBeing::saySpell(spellNumT si) {
  sstring spellWords;
  TThing* t = NULL;
  sstring toLayman;
  sstring toExpert;

  static const sstring syls[][2] = {
    {" ", " "},
    {"ar", "snee"},
    {"au", "ba"},
    {"bless", "tor"},
    {"blind", "jul"},
    {"bur", "flag"},
    {"cu", "dol"},
    {"ca", "pul"},
    {"de", "umo"},
    {"en", "thar"},
    {"from", "al"},
    {"heal", "dufi"},
    {"hands", "sunt"},
    {"light", "eybo"},
    {"lo", "yi"},
    {"mor", "arb"},
    {"move", "ido"},
    {"ness", "izza"},
    {"ning", "nar"},
    {"ous", "gal"},
    {"per", "duda"},
    {"ra", "gru"},
    {"re", "sar"},
    {"son", "sabru"},
    {"se", "or"},
    {"tect", "infra"},
    {"tri", "cula"},
    {"ven", "nofo"},
    {"a", "a"},
    {"b", "b"},
    {"c", "q"},
    {"d", "e"},
    {"e", "z"},
    {"f", "y"},
    {"g", "o"},
    {"h", "p"},
    {"i", "u"},
    {"j", "y"},
    {"k", "t"},
    {"l", "r"},
    {"m", "w"},
    {"n", "i"},
    {"o", "a"},
    {"p", "s"},
    {"q", "d"},
    {"r", "f"},
    {"s", "g"},
    {"t", "h"},
    {"u", "j"},
    {"v", "z"},
    {"w", "x"},
    {"x", "n"},
    {"y", "l"},
    {"z", "k"},
  };

  spellWords = discArray[si]->name;
  for (unsigned int iReplace = 0; iReplace < cElements(syls); iReplace++)
    spellWords.inlineReplaceString(syls[iReplace][0], syls[iReplace][1]);

  if (discArray[si]->minMana) {
    toLayman = format("$n utters the incantation, '%s'") % spellWords;
    toExpert = format("$n utters the incantation, '%s'") % discArray[si]->name;
  } else if (discArray[si]->minLifeforce) {
    toLayman = format("$n chants the invokation, '%s'") % spellWords;
    toExpert = format("$n chants the invokation, '%s'") % discArray[si]->name;
  } else {
    toLayman = format("$n utters the holy words, '%s'") % spellWords;
    toExpert = format("$n utters the holy words, '%s'") % discArray[si]->name;
  }

  for (StuffIter it = roomp->stuff.begin();
       it != roomp->stuff.end() && (t = *it); ++it) {
    TBeing* temp_char = dynamic_cast<TBeing*>(t);
    if (!temp_char)
      continue;
    if (temp_char != this) {
      if (temp_char->doesKnowSkill(si))
        act(toExpert, FALSE, this, 0, temp_char, TO_VICT);
      else
        act(toLayman, FALSE, this, 0, temp_char, TO_VICT);
    }
  }
  updatePos();
}

static int preflight_mana(TBeing* ch, spellNumT spl) {
  int howMuch = 0, totalAmt = 0;

  howMuch = ch->useMana(spl);
  totalAmt = ch->getMana() + ch->mostPowerstoneMana();

  if (IS_SET(discArray[spl]->comp_types, SPELL_TASKED))
    howMuch *= discArray[spl]->lag + 2;

  return ((howMuch <= totalAmt));
}

// LIFEFORCE
static int preflight_lifeforce(TBeing* ch, spellNumT spl) {
  int howMuch = 0, totalAmt = 0;

  howMuch = ch->useLifeforce(spl);
  totalAmt = ch->getLifeforce();

  if (IS_SET(discArray[spl]->comp_types, SPELL_TASKED))
    howMuch *= discArray[spl]->lag + 2;

  return ((howMuch <= totalAmt));
}
// END LIFEFORCE

static int preflight_piety(TBeing* ch, spellNumT spl) {
  double howMuch = 0, totalAmt = 0;

  howMuch = ch->usePiety(spl);
  totalAmt = ch->getPiety();

  if (IS_SET(discArray[spl]->comp_types, SPELL_TASKED))
    howMuch *= discArray[spl]->lag + 2;

  return ((howMuch <= totalAmt));
}

int TBeing::reconcilePiety(spellNumT spl, bool checking) {
  double total;

  if (desc && isImmortal())
    return TRUE;

  if (!isImmortal() && !preflight_piety(this, spl)) {
    // this is possible.
    // spell is only denied if piety < min-piety for spell
    // distraction or bad wizardry could require more piety than min-piety
    //     vlogf(LOG_BUG, format("%s (spell=%s(%d)) Failed the second of two
    //     consecutive prefligh_piety() tests.") %  getName() %
    //     discArray[spl]->name % spl);
    if (checking)
      return FALSE;

  } else if (checking)
    return TRUE;

  total = usePiety(spl);

  if (getPiety() > total)
    addToPiety(-total);
  else {
    int moveTotal;
    total -= getPiety();
    moveTotal = max(15, (int)total);
    setPiety(0);
    if (getMove() >= moveTotal) {
      sendTo(
        "You have run out of piety and your prayer starts to eat your "
        "vitality.\n\r");
      addToMove(-moveTotal);
    } else
      return FALSE;
  }
  return TRUE;
}

// if spl == TYPE_UNDEFINED, will use mana
// otherwise uses mana appropriate for spl
int TBeing::reconcileMana(spellNumT spl, bool checking, int mana) {
  int mana_to_burn;

  if (desc && isImmortal())
    return TRUE;

  if (spl > TYPE_UNDEFINED) {
    if (!isImmortal() && !preflight_mana(this, spl)) {
      // this is possible.
      // spell is only denied if mana < min-mana for spell
      // distraction or bad wizardry could require more mana than min-mana
      //      vlogf(LOG_BUG, format("%s (spell=%s(%d)) Failed the second of two
      //      consecutive preflight_mana() tests.") %  getName() %
      //      discArray[spl]->name % spl);
      if (checking) {
        return FALSE;
      } else {
      }
    } else if (checking) {
      return TRUE;
    }
    mana_to_burn = useMana(spl);
  } else {
    mana_to_burn = mana;
  }

  if (mana_to_burn > 0) {
    if (getMana() >= mana_to_burn) {
      setMana(max(0, getMana() - mana_to_burn));
    } else {
      mana_to_burn -= getMana();
      mana_to_burn = max(15, mana_to_burn);
      setMana(0);
      if (getMove() >= mana_to_burn) {
        sendTo(
          "You have run out of mana and your spell starts to eat your "
          "vitality.\n\r");
        addToMove(-mana_to_burn);
      } else {
        return FALSE;
      }
    }
  }
  return TRUE;
}

// LIFEFORCE
int TBeing::reconcileLifeforce(spellNumT spl, bool checking, int lifeforce) {
  int lifeforce_to_burn;

  if (desc && isImmortal())
    return TRUE;

  if (spl > TYPE_UNDEFINED) {
    if (!isImmortal() && !preflight_lifeforce(this, spl)) {
      if (checking) {
        return FALSE;
      } else {
      }
    } else if (checking) {
      return TRUE;
    }
    lifeforce_to_burn = useLifeforce(spl);
  } else {
    lifeforce_to_burn = lifeforce;
  }
  if (lifeforce_to_burn > 0) {
    if (getLifeforce() >= lifeforce_to_burn) {
      setLifeforce(max(0, getLifeforce() - lifeforce_to_burn));
    } else {
      lifeforce_to_burn -= getLifeforce();
      lifeforce_to_burn = max(15, lifeforce_to_burn);
      setLifeforce(0);
    }
  }
  updatePos();
  return TRUE;
}
// END LIFEFORCE

char* skip_spaces(char* sstring) {
  for (; *sstring && (*sstring) == ' '; sstring++)
    ;

  return (sstring);
}

spellNumT searchForSpellNum(const sstring& arg, exactTypeT exact, bool unique) {
  spellNumT i = MIN_SPELL;
  spellNumT ret = TYPE_UNDEFINED;
  int matches = 0;

  for (i = MIN_SPELL; i < MAX_SKILL; i++) {
    if (hideThisSpell(i))
      continue;

    if (!exact) {
      if (is_abbrev(arg, discArray[i]->name, MULTIPLE_YES)) {
        if (!unique)
          return i;
        matches++;
        ret = i;
      }
    } else {
      if (is_exact_spellname(arg, discArray[i]->name)) {
        if (!unique)
          return i;
        matches++;
        ret = i;        
      }
    }
  }

  if (matches != 1)
    return TYPE_UNDEFINED;

  return ret;
}

static void badCastSyntax(const TBeing* ch, spellNumT which) {
  if (which <= TYPE_UNDEFINED) {
    // unknown spell, so make very generic
    ch->sendTo("Syntax : cast/pray <spell name> <argument>\n\r");
    ch->sendTo("See the CAST/PRAY help file for more details!\n\r");
    return;
  }

  bool cast = ((getSpellType(discArray[which]->typ) == SPELL_CASTER) ||
               (getSpellType(discArray[which]->typ) == SPELL_DANCER));

  if (discArray[which]->targets & TAR_IGNORE) {
    ch->sendTo("This spell isn't working right.\n\r");
    return;
  }
  if (IS_SET(discArray[which]->targets, TAR_NAME)) {
    if (cast) {
      ch->sendTo(
        format("Syntax : cast %s <argument>\n\r") % discArray[which]->name);
      ch->sendTo("See the CAST help file for more details!\n\r");
    } else {
      ch->sendTo(
        format("Syntax : pray %s <argument>\n\r") % discArray[which]->name);
      ch->sendTo("See the PRAY help file for more details!\n\r");
    }
    return;
  }
  sstring tars = "";
  unsigned int targets = discArray[which]->targets;
  if (!IS_SET(targets, TAR_SELF_NONO) &&
      (targets &
        (TAR_CHAR_ROOM | TAR_CHAR_WORLD | TAR_FIGHT_SELF | TAR_SELF_ONLY)))
    tars += (tars.empty() ? "self" : " | self");
  if (!IS_SET(targets, TAR_SELF_ONLY) &&
      (targets & (TAR_CHAR_ROOM | TAR_CHAR_WORLD)))
    tars += (tars.empty() ? "others" : " | others");

  if (targets & (TAR_OBJ_INV | TAR_OBJ_ROOM | TAR_OBJ_WORLD | TAR_OBJ_EQUIP))
    tars += (tars.empty() ? "object" : " | object");

  if (tars.empty()) {
    vlogf(LOG_BUG, format("Unknown targets for spell %d") % which);
    tars += "???";
  }

  if (cast) {
    ch->sendTo(
      format("Syntax : cast %s <%s>\n\r") % discArray[which]->name % tars);
    ch->sendTo("See the CAST help file for more details!\n\r");
  } else {
    ch->sendTo(
      format("Syntax : pray %s <%s>\n\r") % discArray[which]->name % tars);
    ch->sendTo("See the PRAY help file for more details!\n\r");
  }
}

// returns DELETE_THIS
int TBeing::doPray(const char* argument) {
  sstring args = sstring(argument).trim();

  if (isPc() && GetMaxLevel() > MAX_MORT && !hasWizPower(POWER_NO_LIMITS)) {
    sendTo("You are currently not permitted to cast prayers, sorry.\n\r");
    return FALSE;
  }

  if (!hasHands()) {
    sendTo("Sorry, you don't have the right form for that.\n\r");
    return FALSE;
  }

  if (!doesKnowSkill(SKILL_DEVOTION) && !hasClass(CLASS_CLERIC) &&
      !hasClass(CLASS_DEIKHAN)) {
    sendTo("You do not have the faith required to pray.\n\r");
    return FALSE;
  }

  if (nomagic("Sorry, your deity refuses to contact you here."))
    return FALSE;

  if (cantHit > 0) {
    sendTo("You're too busy.\n\r");
    return FALSE;
  }

  if (args.empty()) {
    badCastSyntax(this, TYPE_UNDEFINED);
    sendTo("You do NOT need to include ''s around the <prayer name>.\n\r");
    return FALSE;
  }

  auto findSpecialCaseByName = [](const sstring& name) {
    // vector, not map, because we need ordered lookup
    const std::vector<std::pair<sstring, spellNumT>> specialCases{
      {"paralyze limb", SPELL_PARALYZE_LIMB},
      {"paralyze", SPELL_PARALYZE},
      {"heal spray", SPELL_HEAL_SPRAY},
      {"heal light", SPELL_HEAL_LIGHT},
      {"heal serious", SPELL_HEAL_SERIOUS},
      {"heal critical spray", SPELL_HEAL_CRITICAL_SPRAY},
      {"heal critical", SPELL_HEAL_CRITICAL},
      {"heal full spray", SPELL_HEAL_FULL_SPRAY},
      {"heal full", SPELL_HEAL_FULL},
      {"heal", SPELL_HEAL},
      {"harm light", SPELL_HARM_LIGHT},
      {"harm serious", SPELL_HARM_SERIOUS},
      {"harm critical", SPELL_HARM_CRITICAL},
      {"harm", SPELL_HARM},
    };

    for (auto pair : specialCases) {
      if (name == pair.first)
        return pair.second;

      auto nw = name.words();
      auto sw = pair.first.words();
      if (nw.size() == sw.size()) {
        bool allWordsAreAbbrev = true;
        for (size_t i = 0; i < nw.size(); ++i) {
          if (!is_abbrev(nw[i], sw[i], MULTIPLE_NO, EXACT_YES)) {
            allWordsAreAbbrev = false;
            break;
          }
        }
        if (allWordsAreAbbrev)
          return pair.second;
      }
    }
    return TYPE_UNDEFINED;
  };

  auto findPrayerByName = [this](const sstring& name) {
    spellNumT which = TYPE_UNDEFINED;
    if (((which = searchForSpellNum(name, EXACT_YES)) > TYPE_UNDEFINED) ||
        ((which = searchForSpellNum(name, EXACT_NO)) > TYPE_UNDEFINED)) {
      if (discArray[which]->typ != SPELL_CLERIC &&
          discArray[which]->typ != SPELL_DEIKHAN) {
        sendTo("That's not a prayer request!\n\r");
        return TYPE_UNDEFINED;
      }
      if (!doesKnowSkill(getSkillNum(which))) {
        sendTo("You don't know that prayer!\n\r");
        return TYPE_UNDEFINED;
      }
      return which;
    }
    return TYPE_UNDEFINED;
  };

  spellNumT which = findSpecialCaseByName(args);
  if (which != TYPE_UNDEFINED)
    return doDiscipline(which, "");

  if (args.words().size() > 1) {
    which = findSpecialCaseByName(args.dropLastWord());
    if (which != TYPE_UNDEFINED)
      return doDiscipline(which, args.lastWord());
  }

  which = findPrayerByName(args);
  if (which != TYPE_UNDEFINED)
    return doDiscipline(which, "");

  if (args.words().size() > 1) {
    which = findPrayerByName(args.dropLastWord());
    if (which != TYPE_UNDEFINED)
      return doDiscipline(which, args.lastWord());
  }

  sendTo("No such prayer exists.\n\r");
  return FALSE;
}

int TBeing::preCastCheck() {
  if (!isPc() && !desc)
    return FALSE;

  if (!hasHands()) {
    sendTo("Sorry, you don't have the right form for that.\n\r");
    return FALSE;
  }

  if (isPc() && GetMaxLevel() > MAX_MORT && !hasWizPower(POWER_NO_LIMITS)) {
    sendTo("You are currently not permitted to cast spells, sorry.\n\r");
    return FALSE;
  }

  if (!isImmortal() && !hasClass(CLASS_MAGE | CLASS_RANGER | CLASS_SHAMAN)) {
    if (hasClass(CLASS_CLERIC)) {
      sendTo("Clerics can't cast spells, they pray!\n\r");
      return FALSE;
    } else if (hasClass(CLASS_DEIKHAN)) {
      sendTo("Deikhans can't cast spells, they pray!\n\r");
      return FALSE;
    } else if (hasClass(CLASS_WARRIOR)) {
      sendTo("Warriors can't cast spells!\n\r");
      return FALSE;
    } else if (hasClass(CLASS_THIEF)) {
      sendTo("Thieves can't cast spells!\n\r");
      return FALSE;
    } else if (hasClass(CLASS_MONK)) {
      sendTo("Monks can't cast spells!\n\r");
      return FALSE;
    }
  }
  if (nomagic("Sorry, you can't do that in this area."))
    return FALSE;

  if (cantHit > 0) {
    sendTo("You're too busy.\n\r");
    return FALSE;
  }
  return TRUE;
}

// returns DELETE_THIS
int TBeing::doCast(const char* argument) {
  if (!preCastCheck())
    return FALSE;

  auto [spell, target] = parseSpellNum(argument);

  if (spell == TYPE_UNDEFINED) {
    sendTo("No such spell exists.\n\r");
    return FALSE;
  }

  if (discArray[spell]->typ != SPELL_MAGE &&
          discArray[spell]->typ != SPELL_SHAMAN) {
    sendTo("That's not a magic spell!\n\r");
    return FALSE;
  }

  if (!doesKnowSkill(getSkillNum(spell))) {
    sendTo("You don't know that spell!\n\r");
    return FALSE;
  }

  return doDiscipline(spell, target);
}

// finds the target indicated in n, for spell which and sets ret to it
int TBeing::parseTarget(spellNumT which, char* n, TThing** ret) {
  TBeing* ch;
  TObj* o;
  TThing* t;
  bool ok;

  ok = FALSE;
  ch = NULL;
  o = NULL;
  bool cast = ((getSpellType(discArray[which]->typ) == SPELL_CASTER) ||
               (getSpellType(discArray[which]->typ) == SPELL_DANCER));

  if (*n) {
    if (IS_SET(discArray[which]->targets, TAR_CHAR_ROOM)) {
      // use a variety of increasingly more generic room checks to find target
      ch = get_best_char_room(this, n, VISIBLE_YES, INFRA_NO);
      if (!ch)
        ch = get_best_char_room(this, n, VISIBLE_YES, INFRA_YES);

      if (ch) {
        if (ch->isPlayerAction(PLR_SOLOQUEST) && (ch != this) &&
            !isImmortal() && isPc()) {
          if (!cast) {
            act("$N is on a quest, you can't invoke prayers on $M!", FALSE,
              this, NULL, ch, TO_CHAR);
          } else {
            act("$N is on a quest, you can't cast spells on $M!", FALSE, this,
              NULL, ch, TO_CHAR);
          }
          return FALSE;
        }
        if (ch->isPlayerAction(PLR_GRPQUEST) && (ch != this) && !isImmortal() &&
            isPc() && !isPlayerAction(PLR_GRPQUEST)) {
          if (!cast)
            act("$N is on a group quest you aren't on!  No prayers allowed!",
              FALSE, this, NULL, ch, TO_CHAR);
          else
            act("$N is on a group quest you aren't on! No spells allowed!",
              FALSE, this, NULL, ch, TO_CHAR);
          return FALSE;
        }
        if ((discArray[which]->targets & TAR_VIOLENT) && noHarmCheck(ch))
          return FALSE;

        ok = TRUE;
      }
    }
    if ((!ok && (discArray[which]->targets & TAR_CHAR_VIS_WORLD)) ||
        (discArray[which]->targets & TAR_CHAR_WORLD)) {
      if ((ch = get_pc_world(this, n, EXACT_YES, INFRA_NO,
             (discArray[which]->targets & TAR_CHAR_VIS_WORLD))) ||
          (ch = get_pc_world(this, n, EXACT_NO, INFRA_NO,
             (discArray[which]->targets & TAR_CHAR_VIS_WORLD)))) {
        if (ch->isPlayerAction(PLR_SOLOQUEST) && (ch != this) &&
            !isImmortal() && isPc()) {
          if (!cast)
            act("$N is on a quest, you can't invoke prayers on $M!", FALSE,
              this, NULL, ch, TO_CHAR);
          else
            act("$N is on a quest, you can't cast spells on $M!", FALSE, this,
              NULL, ch, TO_CHAR);
          return FALSE;
        }
        if ((discArray[which]->targets & TAR_VIOLENT) && noHarmCheck(ch))
          return FALSE;

        if (ch->isPlayerAction(PLR_GRPQUEST) && (ch != this) && !isImmortal() &&
            isPc() && !isPlayerAction(PLR_GRPQUEST)) {
          if (!cast)
            act("$N is on a group quest you aren't on!  No prayers allowed!",
              FALSE, this, NULL, ch, TO_CHAR);
          else
            act("$N is on a group quest you aren't on! No spells allowed!",
              FALSE, this, NULL, ch, TO_CHAR);
          return FALSE;
        }
        ok = TRUE;
      }
      if (discArray[which]->targets & TAR_CHAR_VIS_WORLD) {
        if (!ok && (ch = get_char_vis(this, n, NULL))) {
          ok = TRUE;
        }
      } else {
        if (!ok && (ch = get_char(n, EXACT_NO))) {
          ok = TRUE;
        }
      }
    }
    int dummy = 0;
    if (!ok && (discArray[which]->targets & TAR_OBJ_INV)) {
      if ((o = dynamic_cast<TObj*>(
             searchLinkedListVis(this, n, stuff, &dummy, TYPEOBJ)))) {
        ok = TRUE;
      }
    }
    if (!ok && (discArray[which]->targets & TAR_OBJ_ROOM)) {
      if ((o = dynamic_cast<TObj*>(
             searchLinkedListVis(this, n, roomp->stuff, &dummy, TYPEOBJ))))
        ok = TRUE;
    }
    if (!ok && (discArray[which]->targets & TAR_OBJ_WORLD)) {
      if ((o = get_obj_vis(ch, name.c_str(), NULL, EXACT_NO)))
        ok = TRUE;
    }
    if (!ok && (discArray[which]->targets & TAR_OBJ_EQUIP)) {
      for (int i = MIN_WEAR; i < MAX_WEAR && !ok; i++) {
        if ((t = equipment[i]) && is_abbrev(n, t->getName()) &&
            (o = dynamic_cast<TObj*>(t))) {
          ok = TRUE;
        }
      }
    }
    if (!ok && (discArray[which]->targets & TAR_SELF_ONLY)) {
      if (boost::iequals(getName(), n)) {
        ch = this;
        ok = TRUE;
      }
    }
    if (!ok && (discArray[which]->targets & TAR_NAME))
      ok = TRUE;

    if (ch && dynamic_cast<TMonster*>(ch)) {
      if ((ch->specials.act & ACT_IMMORTAL)) {
        if (!cast)
          sendTo("Invoke a prayer on an immortal being?!?! Never!!\n\r");
        else
          sendTo("Cast magic on an immortal being?!?! Never!!\n\r");
        return FALSE;
      }
    }
    if (!ok && (discArray[which]->targets & TAR_IGNORE))
      ok = TRUE;
  } else {
    if ((discArray[which]->targets & TAR_FIGHT_SELF)) {
      // if in a fight, cast this on caster
      // these are generally healing spells
      // just to be nice, if not in fight (and no args), also cast on caster
      // somewhat silly, because the above is the same as TAR_IGNORE
      ch = this;
      ok = TRUE;
    }
    if (!ok && (discArray[which]->targets & TAR_FIGHT_VICT)) {
      if (fight() && sameRoom(*fight())) {
        ch = fight();
        ok = TRUE;
      }
    }
    if (!ok && (discArray[which]->targets & TAR_SELF_ONLY)) {
      ch = this;
      ok = TRUE;
    }
    if (!ok && (discArray[which]->targets & TAR_IGNORE)) {
      ch = this;
      ok = TRUE;
    }
  }

  if (!ok) {
    if (*n) {
      if ((discArray[which]->targets & TAR_CHAR_VIS_WORLD) ||
          (discArray[which]->targets & TAR_CHAR_WORLD))
        sendTo("There is no such person in this realm!\n\r");
      else if (discArray[which]->targets & TAR_CHAR_ROOM)
        sendTo("There is no such person in this room!\n\r");
      else if (discArray[which]->targets & TAR_OBJ_INV)
        sendTo("There is no such object in your possession.\n\r");
      else if (discArray[which]->targets & TAR_OBJ_ROOM)
        sendTo("There is no such object in this room!\n\r");
      else if (discArray[which]->targets & TAR_OBJ_WORLD)
        sendTo("There is no such object in this realm!\n\r");
      else if (discArray[which]->targets & TAR_OBJ_EQUIP)
        sendTo("You are not wearing anything like that.\n\r");
      else
        sendTo("Invalid target flag for that spell.  Bug this!\n\r");
    } else {
      badCastSyntax(this, which);
    }
    return FALSE;

  } else {
    if ((ch == this) && (discArray[which]->targets & TAR_SELF_NONO)) {
      if (!cast)
        sendTo("You cannot invoke this prayer upon yourself.\n\r");
      else
        sendTo("You cannot cast this spell upon yourself.\n\r");
      return FALSE;
    } else if ((ch != this) && (discArray[which]->targets & TAR_SELF_ONLY)) {
      if (!cast)
        sendTo("You can only invoke this prayer upon yourself.\n\r");
      else
        sendTo("You can only cast this spell upon yourself.\n\r");

      return FALSE;
    } else if (isAffected(AFF_CHARM) && (master == ch)) {
      sendTo("You are afraid that it could harm your master.\n\r");
      return FALSE;
    }
  }

  if (ch)
    *ret = (TThing*)ch;
  else if (o)
    *ret = (TThing*)o;

  return TRUE;
}

int TBeing::preDiscCheck(spellNumT which) {
  if (isCombatMode(ATTACK_BERSERK)) {
    sendTo("You can't do that while berserking!\n\r");
    return FALSE;
  }
  if ((discArray[which]->minPosition >= POSITION_CRAWLING) && fight()) {
    sendTo("You can't concentrate enough while fighting!\n\r");
    return FALSE;
  } else if (getPosition() < discArray[which]->minPosition) {
    switch (getPosition()) {
      case POSITION_SLEEPING:
        sendTo("You can't do that while sleeping.\n\r");
        break;
      case POSITION_RESTING:
        sendTo("You can't do that while resting.\n\r");
        break;
      case POSITION_SITTING:
        sendTo("You can't do that while sitting.\n\r");
        break;
      case POSITION_CRAWLING:
        sendTo("You can't do that while crawling.\n\r");
        break;
      default:
        sendTo("You can't do that while unconscious.\n\r");
        break;
    }
    return FALSE;
  }

  if ((discArray[which]->targets & TAR_VIOLENT) &&
      checkPeaceful("Violent disciplines are not allowed here!\n\r"))
    return FALSE;

  if (isPc() && !isImmortal()) {
    if (discArray[which]->minMana) {
      if (!preflight_mana(this, which)) {
        sendTo("You can't summon enough energy to cast the spell.\n\r");
        return FALSE;
      }
    } else if (discArray[which]->minLifeforce) {
      if (!preflight_lifeforce(this, which)) {
        sendTo("You don't have enough lifeforce to perform the ritual.\n\r");
        return FALSE;
      }
    } else {
      if (!preflight_piety(this, which)) {
        sendTo("You lack the piety to pray to your deity for that.\n\r");
        return FALSE;
      }
    }
  }
  return TRUE;
}

namespace {
  std::map<spellNumT, sstring> spellNumToName = {
    {DAMAGE_RIPPED_OUT_HEART, "DAMAGE_RIPPED_OUT_HEART"},
    {DAMAGE_DISEMBOWLED_VR, "DAMAGE_DISEMBOWLED_VR"},
    {DAMAGE_NORMAL, "DAMAGE_NORMAL"},
    {DAMAGE_CAVED_SKULL, "DAMAGE_CAVED_SKULL"},
    {DAMAGE_BEHEADED, "DAMAGE_BEHEADED"},
    {DAMAGE_DISEMBOWLED_HR, "DAMAGE_DISEMBOWLED_HR"},
    {DAMAGE_STOMACH_WOUND, "DAMAGE_STOMACH_WOUND"},
    {DAMAGE_HACKED, "DAMAGE_HACKED"},
    {DAMAGE_IMPALE, "DAMAGE_IMPALE"},
    {DAMAGE_STARVATION, "DAMAGE_STARVATION"},
    {DAMAGE_FALL, "DAMAGE_FALL"},
    {DAMAGE_HEMORRHAGE, "DAMAGE_HEMORRHAGE"},
    {DAMAGE_DROWN, "DAMAGE_DROWN"},
    {DAMAGE_DRAIN, "DAMAGE_DRAIN"},
    {DAMAGE_DISRUPTION, "DAMAGE_DISRUPTION"},
    {DAMAGE_SUFFOCATION, "DAMAGE_SUFFOCATION"},
    {DAMAGE_RAMMED, "DAMAGE_RAMMED"},
    {DAMAGE_WHIRLPOOL, "DAMAGE_WHIRLPOOL"},
    {DAMAGE_ELECTRIC, "DAMAGE_ELECTRIC"},
    {DAMAGE_ACID, "DAMAGE_ACID"},
    {DAMAGE_HOLY, "DAMAGE_HOLY" },
    {DAMAGE_GUST, "DAMAGE_GUST"},
    {DAMAGE_EATTEN, "DAMAGE_EATTEN"},
    {DAMAGE_KICK_HEAD, "DAMAGE_KICK_HEAD"},
    {DAMAGE_KICK_SOLAR, "DAMAGE_KICK_SOLAR"},
    {DAMAGE_HEADBUTT_THROAT, "DAMAGE_HEADBUTT_THROAT"},
    {DAMAGE_HEADBUTT_BODY, "DAMAGE_HEADBUTT_BODY"},
    {DAMAGE_HEADBUTT_CROTCH, "DAMAGE_HEADBUTT_CROTCH"},
    {DAMAGE_HEADBUTT_LEG, "DAMAGE_HEADBUTT_LEG"},
    {DAMAGE_HEADBUTT_FOOT, "DAMAGE_HEADBUTT_FOOT"},
    {DAMAGE_HEADBUTT_JAW, "DAMAGE_HEADBUTT_JAW"},
    {DAMAGE_TRAP_SLEEP, "DAMAGE_TRAP_SLEEP"},
    {DAMAGE_TRAP_TELEPORT, "DAMAGE_TRAP_TELEPORT"},
    {DAMAGE_TRAP_FIRE, "DAMAGE_TRAP_FIRE"},
    {DAMAGE_TRAP_POISON, "DAMAGE_TRAP_POISON"},
    {DAMAGE_TRAP_ACID, "DAMAGE_TRAP_ACID"},
    {DAMAGE_TRAP_TNT, "DAMAGE_TRAP_TNT"},
    {DAMAGE_TRAP_ENERGY, "DAMAGE_TRAP_ENERGY"},
    {DAMAGE_TRAP_BLUNT, "DAMAGE_TRAP_BLUNT"},
    {DAMAGE_TRAP_PIERCE, "DAMAGE_TRAP_PIERCE"},
    {DAMAGE_TRAP_SLASH, "DAMAGE_TRAP_SLASH"},
    {DAMAGE_TRAP_FROST, "DAMAGE_TRAP_FROST"},
    {DAMAGE_TRAP_DISEASE, "DAMAGE_TRAP_DISEASE"},
    {DAMAGE_ARROWS, "DAMAGE_ARROWS"},
    {DAMAGE_FIRE, "DAMAGE_FIRE"},
    {DAMAGE_FROST, "DAMAGE_FROST"},
    {DAMAGE_HEADBUTT_SKULL, "DAMAGE_HEADBUTT_SKULL"},
    {DAMAGE_COLLISION, "DAMAGE_COLLISION"},
    {DAMAGE_KICK_SHIN, "DAMAGE_KICK_SHIN"},
    {DAMAGE_KNEESTRIKE_FOOT, "DAMAGE_KNEESTRIKE_FOOT"},
    {DAMAGE_KNEESTRIKE_SHIN, "DAMAGE_KNEESTRIKE_SHIN"},
    {DAMAGE_KNEESTRIKE_KNEE, "DAMAGE_KNEESTRIKE_KNEE"},
    {DAMAGE_KNEESTRIKE_THIGH, "DAMAGE_KNEESTRIKE_THIGH"},
    {DAMAGE_KNEESTRIKE_CROTCH, "DAMAGE_KNEESTRIKE_CROTCH"},
    {DAMAGE_KNEESTRIKE_SOLAR, "DAMAGE_KNEESTRIKE_SOLAR"},
    {DAMAGE_KNEESTRIKE_CHIN, "DAMAGE_KNEESTRIKE_CHIN"},
    {DAMAGE_KNEESTRIKE_FACE, "DAMAGE_KNEESTRIKE_FACE"},
    {DAMAGE_KICK_SIDE, "DAMAGE_KICK_SIDE"},
    {TYPE_UNDEFINED, "TYPE_UNDEFINED"},
    {SPELL_GUST, "SPELL_GUST"},
    {SPELL_SLING_SHOT, "SPELL_SLING_SHOT"},
    {SPELL_GUSHER, "SPELL_GUSHER"},
    {SPELL_HANDS_OF_FLAME, "SPELL_HANDS_OF_FLAME"},
    {SPELL_MYSTIC_DARTS, "SPELL_MYSTIC_DARTS"},
    {SPELL_FLARE, "SPELL_FLARE"},
    {SPELL_SORCERERS_GLOBE, "SPELL_SORCERERS_GLOBE"},
    {SPELL_FAERIE_FIRE, "SPELL_FAERIE_FIRE"},
    {SPELL_ILLUMINATE, "SPELL_ILLUMINATE"},
    {SPELL_DETECT_MAGIC, "SPELL_DETECT_MAGIC"},
    {SPELL_STUNNING_ARROW, "SPELL_STUNNING_ARROW"},
    {SPELL_MATERIALIZE, "SPELL_MATERIALIZE"},
    {SPELL_PROTECTION_FROM_EARTH, "SPELL_PROTECTION_FROM_EARTH"},
    {SPELL_PROTECTION_FROM_AIR, "SPELL_PROTECTION_FROM_AIR"},
    {SPELL_PROTECTION_FROM_FIRE, "SPELL_PROTECTION_FROM_FIRE"},
    {SPELL_PROTECTION_FROM_WATER, "SPELL_PROTECTION_FROM_WATER"},
    {SPELL_PROTECTION_FROM_ENERGY, "SPELL_PROTECTION_FROM_ENERGY"},
    {SPELL_PEBBLE_SPRAY, "SPELL_PEBBLE_SPRAY"},
    {SPELL_ARCTIC_BLAST, "SPELL_ARCTIC_BLAST"},
    {SPELL_COLOR_SPRAY, "SPELL_COLOR_SPRAY"},
    {SPELL_INFRAVISION, "SPELL_INFRAVISION"},
    {SPELL_IDENTIFY, "SPELL_IDENTIFY"},
    {SPELL_POWERSTONE, "SPELL_POWERSTONE"},
    {SPELL_FAERIE_FOG, "SPELL_FAERIE_FOG"},
    {SPELL_TELEPORT, "SPELL_TELEPORT"},
    {SPELL_SENSE_LIFE, "SPELL_SENSE_LIFE"},
    {SPELL_CALM, "SPELL_CALM"},
    {SPELL_ACCELERATE, "SPELL_ACCELERATE"},
    {SPELL_DUST_STORM, "SPELL_DUST_STORM"},
    {SPELL_LEVITATE, "SPELL_LEVITATE"},
    {SPELL_FEATHERY_DESCENT, "SPELL_FEATHERY_DESCENT"},
    {SPELL_STEALTH, "SPELL_STEALTH"},
    {SPELL_GRANITE_FISTS, "SPELL_GRANITE_FISTS"},
    {SPELL_ICY_GRIP, "SPELL_ICY_GRIP"},
    {SPELL_GILLS_OF_FLESH, "SPELL_GILLS_OF_FLESH"},
    {SPELL_TELEPATHY, "SPELL_TELEPATHY"},
    {SPELL_FEAR, "SPELL_FEAR"},
    {SPELL_SLUMBER, "SPELL_SLUMBER"},
    {SPELL_CONJURE_EARTH, "SPELL_CONJURE_EARTH"},
    {SPELL_CONJURE_AIR, "SPELL_CONJURE_AIR"},
    {SPELL_CONJURE_FIRE, "SPELL_CONJURE_FIRE"},
    {SPELL_CONJURE_WATER, "SPELL_CONJURE_WATER"},
    {SPELL_DISPEL_MAGIC, "SPELL_DISPEL_MAGIC"},
    {SPELL_ENHANCE_WEAPON, "SPELL_ENHANCE_WEAPON"},
    {SPELL_GALVANIZE, "SPELL_GALVANIZE"},
    {SPELL_DETECT_INVISIBLE, "SPELL_DETECT_INVISIBLE"},
    {SPELL_DISPEL_INVISIBLE, "SPELL_DISPEL_INVISIBLE"},
    {SPELL_TORNADO, "SPELL_TORNADO"},
    {SPELL_SAND_BLAST, "SPELL_SAND_BLAST"},
    {SPELL_ICE_STORM, "SPELL_ICE_STORM"},
    {SPELL_BLAST_OF_FURY, "SPELL_BLAST_OF_FURY"},
    {SPELL_ACID_BLAST, "SPELL_ACID_BLAST"},
    {SPELL_FIREBALL, "SPELL_FIREBALL"},
    {SPELL_FARLOOK, "SPELL_FARLOOK"},
    {SPELL_FALCON_WINGS, "SPELL_FALCON_WINGS"},
    {SPELL_INVISIBILITY, "SPELL_INVISIBILITY"},
    {SPELL_ENSORCER, "SPELL_ENSORCER"},
    {SPELL_EYES_OF_FERTUMAN, "SPELL_EYES_OF_FERTUMAN"},
    {SPELL_COPY, "SPELL_COPY"},
    {SPELL_HASTE, "SPELL_HASTE"},
    {SPELL_IMMOBILIZE, "SPELL_IMMOBILIZE"},
    {SPELL_SUFFOCATE, "SPELL_SUFFOCATE"},
    {SPELL_FLY, "SPELL_FLY"},
    {SPELL_ANTIGRAVITY, "SPELL_ANTIGRAVITY"},
    {SPELL_DIVINATION, "SPELL_DIVINATION"},
    {SPELL_SHATTER, "SPELL_SHATTER"},
    {SPELL_SPONTANEOUS_GENERATION, "SPELL_SPONTANEOUS_GENERATION"},
    {SPELL_METEOR_SWARM, "SPELL_METEOR_SWARM"},
    {SPELL_LAVA_STREAM, "SPELL_LAVA_STREAM"},
    {SPELL_DEATH_MIST, "SPELL_DEATH_MIST"},
    {SPELL_STONE_SKIN, "SPELL_STONE_SKIN"},
    {SPELL_TRAIL_SEEK, "SPELL_TRAIL_SEEK"},
    {SPELL_LAVA_LANCE, "SPELL_LAVA_LANCE"},
    {SPELL_INFERNO, "SPELL_INFERNO"},
    {SPELL_HELLFIRE, "SPELL_HELLFIRE"},
    {SPELL_FLAMING_FLESH, "SPELL_FLAMING_FLESH"},
    {SPELL_FLAMING_SWORD, "SPELL_FLAMING_SWORD"},
    {SPELL_ENERGY_DRAIN, "SPELL_ENERGY_DRAIN"},
    {SPELL_ATOMIZE, "SPELL_ATOMIZE"},
    {SPELL_ANIMATE, "SPELL_ANIMATE"},
    {SPELL_BIND, "SPELL_BIND"},
    {SPELL_FUMBLE, "SPELL_FUMBLE"},
    {SPELL_TRUE_SIGHT, "SPELL_TRUE_SIGHT"},
    {SPELL_CLOUD_OF_CONCEALMENT, "SPELL_CLOUD_OF_CONCEALMENT"},
    {SPELL_POLYMORPH, "SPELL_POLYMORPH"},
    {SPELL_SILENCE, "SPELL_SILENCE"},
    {SPELL_WATERY_GRAVE, "SPELL_WATERY_GRAVE"},
    {SPELL_TSUNAMI, "SPELL_TSUNAMI"},
    {SPELL_BREATH_OF_SARAHAGE, "SPELL_BREATH_OF_SARAHAGE"},
    {SPELL_PLASMA_MIRROR, "SPELL_PLASMA_MIRROR"},
    {SPELL_GARMULS_TAIL, "SPELL_GARMULS_TAIL"},
    {SPELL_ETHER_GATE, "SPELL_ETHER_GATE"},
    {SPELL_KNOT, "SPELL_KNOT"},
    {SPELL_HEAL_LIGHT, "SPELL_HEAL_LIGHT"},
    {SPELL_HARM_LIGHT, "SPELL_HARM_LIGHT"},
    {SPELL_CREATE_FOOD, "SPELL_CREATE_FOOD"},
    {SPELL_CREATE_WATER, "SPELL_CREATE_WATER"},
    {SPELL_ARMOR, "SPELL_ARMOR"},
    {SPELL_BLESS, "SPELL_BLESS"},
    {SPELL_CLOT, "SPELL_CLOT"},
    {SPELL_RAIN_BRIMSTONE, "SPELL_RAIN_BRIMSTONE"},
    {SPELL_HEAL_SERIOUS, "SPELL_HEAL_SERIOUS"},
    {SPELL_HARM_SERIOUS, "SPELL_HARM_SERIOUS"},
    {SPELL_STERILIZE, "SPELL_STERILIZE"},
    {SPELL_EXPEL, "SPELL_EXPEL"},
    {SPELL_CURE_DISEASE, "SPELL_CURE_DISEASE"},
    {SPELL_CURSE, "SPELL_CURSE"},
    {SPELL_REMOVE_CURSE, "SPELL_REMOVE_CURSE"},
    {SPELL_CURE_POISON, "SPELL_CURE_POISON"},
    {SPELL_HEAL_CRITICAL, "SPELL_HEAL_CRITICAL"},
    {SPELL_SALVE, "SPELL_SALVE"},
    {SPELL_POISON, "SPELL_POISON"},
    {SPELL_HARM_CRITICAL, "SPELL_HARM_CRITICAL"},
    {SPELL_INFECT, "SPELL_INFECT"},
    {SPELL_REFRESH, "SPELL_REFRESH"},
    {SPELL_NUMB, "SPELL_NUMB"},
    {SPELL_DISEASE, "SPELL_DISEASE"},
    {SPELL_FLAMESTRIKE, "SPELL_FLAMESTRIKE"},
    {SPELL_PLAGUE_LOCUSTS, "SPELL_PLAGUE_LOCUSTS"},
    {SPELL_CURE_BLINDNESS, "SPELL_CURE_BLINDNESS"},
    {SPELL_SUMMON, "SPELL_SUMMON"},
    {SPELL_HEAL, "SPELL_HEAL"},
    {SPELL_PARALYZE_LIMB, "SPELL_PARALYZE_LIMB"},
    {SPELL_WORD_OF_RECALL, "SPELL_WORD_OF_RECALL"},
    {SPELL_HARM, "SPELL_HARM"},
    {SPELL_BLINDNESS, "SPELL_BLINDNESS"},
    {SPELL_PILLAR_SALT, "SPELL_PILLAR_SALT"},
    {SPELL_EARTHQUAKE, "SPELL_EARTHQUAKE"},
    {SPELL_CALL_LIGHTNING, "SPELL_CALL_LIGHTNING"},
    {SPELL_SPONTANEOUS_COMBUST, "SPELL_SPONTANEOUS_COMBUST"},
    {SPELL_BLEED, "SPELL_BLEED"},
    {SPELL_PARALYZE, "SPELL_PARALYZE"},
    {SPELL_BONE_BREAKER, "SPELL_BONE_BREAKER"},
    {SPELL_WITHER_LIMB, "SPELL_WITHER_LIMB"},
    {SPELL_SANCTUARY, "SPELL_SANCTUARY"},
    {SPELL_CURE_PARALYSIS, "SPELL_CURE_PARALYSIS"},
    {SPELL_SECOND_WIND, "SPELL_SECOND_WIND"},
    {SPELL_HEROES_FEAST, "SPELL_HEROES_FEAST"},
    {SPELL_ASTRAL_WALK, "SPELL_ASTRAL_WALK"},
    {SPELL_PORTAL, "SPELL_PORTAL"},
    {SPELL_HEAL_FULL, "SPELL_HEAL_FULL"},
    {SPELL_HEAL_CRITICAL_SPRAY, "SPELL_HEAL_CRITICAL_SPRAY"},
    {SPELL_HEAL_SPRAY, "SPELL_HEAL_SPRAY"},
    {SPELL_HEAL_FULL_SPRAY, "SPELL_HEAL_FULL_SPRAY"},
    {SPELL_RESTORE_LIMB, "SPELL_RESTORE_LIMB"},
    {SPELL_KNIT_BONE, "SPELL_KNIT_BONE"},
    {SPELL_RELIVE, "SPELL_RELIVE"},
    {SPELL_CRUSADE, "SPELL_CRUSADE"},
    {SPELL_FLATULENCE, "SPELL_FLATULENCE"},
    {SPELL_ENLIVEN, "SPELL_ENLIVEN"},
    {SPELL_BLOOD_BOIL, "SPELL_BLOOD_BOIL"},
    {SPELL_STUPIDITY, "SPELL_STUPIDITY"},
    {SPELL_CELERITE, "SPELL_CELERITE"},
    {SPELL_CREATE_WOOD_GOLEM, "SPELL_CREATE_WOOD_GOLEM"},
    {SPELL_CREATE_ROCK_GOLEM, "SPELL_CREATE_ROCK_GOLEM"},
    {SPELL_CREATE_IRON_GOLEM, "SPELL_CREATE_IRON_GOLEM"},
    {SPELL_CREATE_DIAMOND_GOLEM, "SPELL_CREATE_DIAMOND_GOLEM"},
    {SPELL_ENTHRALL_SPECTRE, "SPELL_ENTHRALL_SPECTRE"},
    {SPELL_ENTHRALL_GHAST, "SPELL_ENTHRALL_GHAST"},
    {SPELL_ENTHRALL_GHOUL, "SPELL_ENTHRALL_GHOUL"},
    {SPELL_ENTHRALL_DEMON, "SPELL_ENTHRALL_DEMON"},
    {SPELL_DEATHWAVE, "SPELL_DEATHWAVE"},
    {SPELL_DISTORT, "SPELL_DISTORT"},
    {SPELL_SOUL_TWIST, "SPELL_SOUL_TWIST"},
    {SPELL_SQUISH, "SPELL_SQUISH"},
    {SPELL_CARDIAC_STRESS, "SPELL_CARDIAC_STRESS"},
    {SPELL_LEGBA, "SPELL_LEGBA"},
    {SPELL_DJALLA, "SPELL_DJALLA"},
    {SPELL_SENSE_LIFE_SHAMAN, "SPELL_SENSE_LIFE_SHAMAN"},
    {SPELL_DETECT_SHADOW, "SPELL_DETECT_SHADOW"},
    {SPELL_RAZE, "SPELL_RAZE"},
    {SPELL_INTIMIDATE, "SPELL_INTIMIDATE"},
    {SPELL_ROMBLER, "SPELL_ROMBLER"},
    {SPELL_CHRISM, "SPELL_CHRISM"},
    {SPELL_CHEVAL, "SPELL_CHEVAL"},
    {SPELL_HYPNOSIS, "SPELL_HYPNOSIS"},
    {SPELL_LICH_TOUCH, "SPELL_LICH_TOUCH"},
    {SPELL_SHADOW_WALK, "SPELL_SHADOW_WALK"},
    {SPELL_CLARITY, "SPELL_CLARITY"},
    {SPELL_AQUALUNG, "SPELL_AQUALUNG"},
    {SPELL_AQUATIC_BLAST, "SPELL_AQUATIC_BLAST"},
    {SPELL_THORNFLESH, "SPELL_THORNFLESH"},
    {SPELL_SHIELD_OF_MISTS, "SPELL_SHIELD_OF_MISTS"},
    {SPELL_DANCING_BONES, "SPELL_DANCING_BONES"},
    {SPELL_CONTROL_UNDEAD, "SPELL_CONTROL_UNDEAD"},
    {SPELL_RESURRECTION, "SPELL_RESURRECTION"},
    {SPELL_VOODOO, "SPELL_VOODOO"},
    {SPELL_VAMPIRIC_TOUCH, "SPELL_VAMPIRIC_TOUCH"},
    {SPELL_LIFE_LEECH, "SPELL_LIFE_LEECH"},
    {SPELL_CHASE_SPIRIT, "SPELL_CHASE_SPIRIT"},
    {SPELL_CLEANSE, "SPELL_CLEANSE"},
    {SPELL_HEALING_GRASP, "SPELL_HEALING_GRASP"},
    {SPELL_EMBALM, "SPELL_EMBALM"},
    {SPELL_ROOT_CONTROL, "SPELL_ROOT_CONTROL"},
    {SPELL_LIVING_VINES, "SPELL_LIVING_VINES"},
    {SPELL_STICKS_TO_SNAKES, "SPELL_STICKS_TO_SNAKES"},
    {SPELL_STORMY_SKIES, "SPELL_STORMY_SKIES"},
    {SPELL_TREE_WALK, "SPELL_TREE_WALK"},
    {SPELL_SHAPESHIFT, "SPELL_SHAPESHIFT"},
    {SPELL_EARTHMAW, "SPELL_EARTHMAW"},
    {SPELL_CREEPING_DOOM, "SPELL_CREEPING_DOOM"},
    {SPELL_FERAL_WRATH, "SPELL_FERAL_WRATH"},
    {SPELL_SKY_SPIRIT, "SPELL_SKY_SPIRIT"},
    {SPELL_HEAL_LIGHT_DEIKHAN, "SPELL_HEAL_LIGHT_DEIKHAN"},
    {SPELL_ARMOR_DEIKHAN, "SPELL_ARMOR_DEIKHAN"},
    {SPELL_BLESS_DEIKHAN, "SPELL_BLESS_DEIKHAN"},
    {SPELL_EXPEL_DEIKHAN, "SPELL_EXPEL_DEIKHAN"},
    {SPELL_CLOT_DEIKHAN, "SPELL_CLOT_DEIKHAN"},
    {SPELL_STERILIZE_DEIKHAN, "SPELL_STERILIZE_DEIKHAN"},
    {SPELL_REMOVE_CURSE_DEIKHAN, "SPELL_REMOVE_CURSE_DEIKHAN"},
    {SPELL_CURSE_DEIKHAN, "SPELL_CURSE_DEIKHAN"},
    {SPELL_INFECT_DEIKHAN, "SPELL_INFECT_DEIKHAN"},
    {SPELL_CURE_DISEASE_DEIKHAN, "SPELL_CURE_DISEASE_DEIKHAN"},
    {SPELL_CREATE_FOOD_DEIKHAN, "SPELL_CREATE_FOOD_DEIKHAN"},
    {SPELL_CREATE_WATER_DEIKHAN, "SPELL_CREATE_WATER_DEIKHAN"},
    {SPELL_HEAL_SERIOUS_DEIKHAN, "SPELL_HEAL_SERIOUS_DEIKHAN"},
    {SPELL_CURE_POISON_DEIKHAN, "SPELL_CURE_POISON_DEIKHAN"},
    {SPELL_POISON_DEIKHAN, "SPELL_POISON_DEIKHAN"},
    {SPELL_HARM_SERIOUS_DEIKHAN, "SPELL_HARM_SERIOUS_DEIKHAN"},
    {SPELL_HEAL_CRITICAL_DEIKHAN, "SPELL_HEAL_CRITICAL_DEIKHAN"},
    {SPELL_HARM_CRITICAL_DEIKHAN, "SPELL_HARM_CRITICAL_DEIKHAN"},
    {SPELL_HARM_LIGHT_DEIKHAN, "SPELL_HARM_LIGHT_DEIKHAN"},
    {SPELL_HEROES_FEAST_DEIKHAN, "SPELL_HEROES_FEAST_DEIKHAN"},
    {SPELL_REFRESH_DEIKHAN, "SPELL_REFRESH_DEIKHAN"},
    {SPELL_SYNOSTODWEOMER, "SPELL_SYNOSTODWEOMER"},
    {SPELL_SALVE_DEIKHAN, "SPELL_SALVE_DEIKHAN"},
    {SPELL_HARM_DEIKHAN, "SPELL_HARM_DEIKHAN"},
    {SPELL_NUMB_DEIKHAN, "SPELL_NUMB_DEIKHAN"},
    {SKILL_SLAM, "SKILL_SLAM"},
    {SKILL_BASH, "SKILL_BASH"},
    {SKILL_TRIP, "SKILL_TRIP"},
    {SKILL_HEADBUTT, "SKILL_HEADBUTT"},
    {SKILL_RESCUE, "SKILL_RESCUE"},
    {SKILL_BLACKSMITHING, "SKILL_BLACKSMITHING"},
    {SKILL_DISARM, "SKILL_DISARM"},
    {SKILL_PARRY_WARRIOR, "SKILL_PARRY_WARRIOR"},
    {SKILL_BERSERK, "SKILL_BERSERK"},
    {SKILL_SWITCH_OPP, "SKILL_SWITCH_OPP"},
    {SKILL_BODYSLAM, "SKILL_BODYSLAM"},
    {SKILL_SPIN, "SKILL_SPIN"},
    {SKILL_KNEESTRIKE, "SKILL_KNEESTRIKE"},
    {SKILL_SHOVE, "SKILL_SHOVE"},
    {SKILL_RETREAT, "SKILL_RETREAT"},
    {SKILL_GRAPPLE, "SKILL_GRAPPLE"},
    {SKILL_STOMP, "SKILL_STOMP"},
    {SKILL_BRAWL_AVOIDANCE, "SKILL_BRAWL_AVOIDANCE"},
    {SKILL_DOORBASH, "SKILL_DOORBASH"},
    {SKILL_DEATHSTROKE, "SKILL_DEATHSTROKE"},
    {SKILL_FOCUS_ATTACK, "SKILL_FOCUS_ATTACK"},
    {SKILL_DUAL_WIELD, "SKILL_DUAL_WIELD"},
    {SKILL_POWERMOVE, "SKILL_POWERMOVE"},
    {SKILL_2H_SPEC, "SKILL_2H_SPEC"},
    {SKILL_TRANCE_OF_BLADES, "SKILL_TRANCE_OF_BLADES"},
    {SKILL_WEAPON_RETENTION, "SKILL_WEAPON_RETENTION"},
    {SKILL_BLACKSMITHING_ADVANCED, "SKILL_BLACKSMITHING_ADVANCED"},
    {SKILL_CLOSE_QUARTERS_FIGHTING, "SKILL_CLOSE_QUARTERS_FIGHTING"},
    {SKILL_RIPOSTE, "SKILL_RIPOSTE"},
    {SKILL_TAUNT, "SKILL_TAUNT"},
    {SKILL_SCRIBE, "SKILL_SCRIBE"},
    {SKILL_REPAIR_MAGE, "SKILL_REPAIR_MAGE"},
    {SKILL_REPAIR_CLERIC, "SKILL_REPAIR_CLERIC"},
    {SKILL_TRANSFORM_LIMB, "SKILL_TRANSFORM_LIMB"},
    {SKILL_BEAST_SOOTHER, "SKILL_BEAST_SOOTHER"},
    {SKILL_BEFRIEND_BEAST, "SKILL_BEFRIEND_BEAST"},
    {SKILL_TRANSFIX, "SKILL_TRANSFIX"},
    {SKILL_BUTCHER, "SKILL_BUTCHER"},
    {SKILL_BEAST_SUMMON, "SKILL_BEAST_SUMMON"},
    {SKILL_BARKSKIN, "SKILL_BARKSKIN"},
    {SKILL_BEAST_CHARM, "SKILL_BEAST_CHARM"},
    {SKILL_APPLY_HERBS, "SKILL_APPLY_HERBS"},
    {SKILL_CHIVALRY, "SKILL_CHIVALRY"},
    {SKILL_BASH_DEIKHAN, "SKILL_BASH_DEIKHAN"},
    {SKILL_RESCUE_DEIKHAN, "SKILL_RESCUE_DEIKHAN"},
    {SKILL_DIVINE_GRACE, "SKILL_DIVINE_GRACE"},
    {SKILL_DIVINE_RESCUE, "SKILL_DIVINE_RESCUE"},
    {SKILL_GUARDIANS_LIGHT, "SKILL_GUARDIANS_LIGHT"},
    {SKILL_SMITE, "SKILL_SMITE"},
    {SKILL_CHARGE, "SKILL_CHARGE"},
    {SKILL_DISARM_DEIKHAN, "SKILL_DISARM_DEIKHAN"},
    {SKILL_SWITCH_DEIKHAN, "SKILL_SWITCH_DEIKHAN"},
    {SKILL_RETREAT_DEIKHAN, "SKILL_RETREAT_DEIKHAN"},
    {SKILL_SHOVE_DEIKHAN, "SKILL_SHOVE_DEIKHAN"},
    {SKILL_2H_SPEC_DEIKHAN, "SKILL_2H_SPEC_DEIKHAN"},
    {SKILL_SHOCK_CAVALRY, "SKILL_SHOCK_CAVALRY"},
    {SKILL_ORIENT, "SKILL_ORIENT"},
    {SKILL_RIDE, "SKILL_RIDE"},
    {SKILL_CALM_MOUNT, "SKILL_CALM_MOUNT"},
    {SKILL_TRAIN_MOUNT, "SKILL_TRAIN_MOUNT"},
    {SKILL_ADVANCED_RIDING, "SKILL_ADVANCED_RIDING"},
    {SKILL_RIDE_DOMESTIC, "SKILL_RIDE_DOMESTIC"},
    {SKILL_RIDE_NONDOMESTIC, "SKILL_RIDE_NONDOMESTIC"},
    {SKILL_RIDE_WINGED, "SKILL_RIDE_WINGED"},
    {SKILL_RIDE_EXOTIC, "SKILL_RIDE_EXOTIC"},
    {SKILL_LAY_HANDS, "SKILL_LAY_HANDS"},
    {SKILL_REPAIR_DEIKHAN, "SKILL_REPAIR_DEIKHAN"},
    {SKILL_AURA_MIGHT, "SKILL_AURA_MIGHT"},
    {SPELL_AURA_MIGHT, "SPELL_AURA_MIGHT"},
    {SKILL_AURA_REGENERATION, "SKILL_AURA_REGENERATION"},
    {SPELL_AURA_REGENERATION, "SPELL_AURA_REGENERATION"},
    {SKILL_AURA_GUARDIAN, "SKILL_AURA_GUARDIAN"},
    {SPELL_AURA_GUARDIAN, "SPELL_AURA_GUARDIAN"},
    {SKILL_AURA_VENGEANCE, "SKILL_AURA_VENGEANCE"},
    {SPELL_AURA_VENGEANCE, "SPELL_AURA_VENGEANCE"},
    {SKILL_AURA_ABSOLUTION, "SKILL_AURA_ABSOLUTION"},
    {SPELL_AURA_ABSOLUTION, "SPELL_AURA_ABSOLUTION"},
    {SKILL_YOGINSA, "SKILL_YOGINSA"},
    {SKILL_CINTAI, "SKILL_CINTAI"},
    {SKILL_OOMLAT, "SKILL_OOMLAT"},
    {SKILL_KICK_MONK, "SKILL_KICK_MONK"},
    {SKILL_ADVANCED_KICKING, "SKILL_ADVANCED_KICKING"},
    {SKILL_DISARM_MONK, "SKILL_DISARM_MONK"},
    {SKILL_GROUNDFIGHTING, "SKILL_GROUNDFIGHTING"},
    {SKILL_CHOP, "SKILL_CHOP"},
    {SKILL_SPRINGLEAP, "SKILL_SPRINGLEAP"},
    {SKILL_DUFALI, "SKILL_DUFALI"},
    {SKILL_RETREAT_MONK, "SKILL_RETREAT_MONK"},
    {SKILL_SNOFALTE, "SKILL_SNOFALTE"},
    {SKILL_COUNTER_MOVE, "SKILL_COUNTER_MOVE"},
    {SKILL_SWITCH_MONK, "SKILL_SWITCH_MONK"},
    {SKILL_JIRIN, "SKILL_JIRIN"},
    {SKILL_KUBO, "SKILL_KUBO"},
    {SKILL_CATFALL, "SKILL_CATFALL"},
    {SKILL_WOHLIN, "SKILL_WOHLIN"},
    {SKILL_VOPLAT, "SKILL_VOPLAT"},
    {SKILL_BLINDFIGHTING, "SKILL_BLINDFIGHTING"},
    {SKILL_CHI, "SKILL_CHI"},
    {SKILL_QUIV_PALM, "SKILL_QUIV_PALM"},
    {SKILL_CRIT_HIT, "SKILL_CRIT_HIT"},
    {SKILL_FEIGN_DEATH, "SKILL_FEIGN_DEATH"},
    {SKILL_BLUR, "SKILL_BLUR"},
    {SKILL_HURL, "SKILL_HURL"},
    {SKILL_SHOULDER_THROW, "SKILL_SHOULDER_THROW"},
    {SKILL_IRON_FIST, "SKILL_IRON_FIST"},
    {SKILL_IRON_FLESH, "SKILL_IRON_FLESH"},
    {SKILL_IRON_SKIN, "SKILL_IRON_SKIN"},
    {SKILL_IRON_BONES, "SKILL_IRON_BONES"},
    {SKILL_IRON_MUSCLES, "SKILL_IRON_MUSCLES"},
    {SKILL_IRON_LEGS, "SKILL_IRON_LEGS"},
    {SKILL_IRON_WILL, "SKILL_IRON_WILL"},
    {SKILL_REPAIR_MONK, "SKILL_REPAIR_MONK"},
    {SKILL_CHAIN_ATTACK, "SKILL_CHAIN_ATTACK"},
    {SKILL_CATLEAP, "SKILL_CATLEAP"},
    {SKILL_DEFENESTRATE, "SKILL_DEFENESTRATE"},
    {SKILL_BONEBREAK, "SKILL_BONEBREAK"},
    {SKILL_SWINDLE, "SKILL_SWINDLE"},
    {SKILL_SNEAK, "SKILL_SNEAK"},
    {SKILL_STABBING, "SKILL_STABBING"},
    {SKILL_RETREAT_THIEF, "SKILL_RETREAT_THIEF"},
    {SKILL_KICK, "SKILL_KICK"},
    {SKILL_PICK_LOCK, "SKILL_PICK_LOCK"},
    {SKILL_BACKSTAB, "SKILL_BACKSTAB"},
    {SKILL_THROATSLIT, "SKILL_THROATSLIT"},
    {SKILL_SEARCH, "SKILL_SEARCH"},
    {SKILL_SPY, "SKILL_SPY"},
    {SKILL_SWITCH_THIEF, "SKILL_SWITCH_THIEF"},
    {SKILL_STEAL, "SKILL_STEAL"},
    {SKILL_DETECT_TRAP, "SKILL_DETECT_TRAP"},
    {SKILL_SUBTERFUGE, "SKILL_SUBTERFUGE"},
    {SKILL_DISARM_TRAP, "SKILL_DISARM_TRAP"},
    {SKILL_CUDGEL, "SKILL_CUDGEL"},
    {SKILL_HIDE, "SKILL_HIDE"},
    {SKILL_POISON_WEAPON, "SKILL_POISON_WEAPON"},
    {SKILL_DISGUISE, "SKILL_DISGUISE"},
    {SKILL_DODGE_THIEF, "SKILL_DODGE_THIEF"},
    {SKILL_GARROTTE, "SKILL_GARROTTE"},
    {SKILL_SET_TRAP_CONT, "SKILL_SET_TRAP_CONT"},
    {SKILL_SET_TRAP_DOOR, "SKILL_SET_TRAP_DOOR"},
    {SKILL_SET_TRAP_MINE, "SKILL_SET_TRAP_MINE"},
    {SKILL_SET_TRAP_GREN, "SKILL_SET_TRAP_GREN"},
    {SKILL_SET_TRAP_ARROW, "SKILL_SET_TRAP_ARROW"},
    {SKILL_DUAL_WIELD_THIEF, "SKILL_DUAL_WIELD_THIEF"},
    {SKILL_DISARM_THIEF, "SKILL_DISARM_THIEF"},
    {SKILL_COUNTER_STEAL, "SKILL_COUNTER_STEAL"},
    {SKILL_REPAIR_THIEF, "SKILL_REPAIR_THIEF"},
    {SKILL_PLANT, "SKILL_PLANT"},
    {SKILL_CONCEALMENT, "SKILL_CONCEALMENT"},
    {SKILL_TRACK, "SKILL_TRACK"},
    {SKILL_RITUALISM, "SKILL_RITUALISM"},
    {SKILL_SACRIFICE, "SKILL_SACRIFICE"},
    {SKILL_BREW, "SKILL_BREW"},
    {SKILL_TURN, "SKILL_TURN"},
    {SKILL_REPAIR_SHAMAN, "SKILL_REPAIR_SHAMAN"},
    {SKILL_PSITELEPATHY, "SKILL_PSITELEPATHY"},
    {SKILL_TELE_SIGHT, "SKILL_TELE_SIGHT"},
    {SKILL_TELE_VISION, "SKILL_TELE_VISION"},
    {SKILL_MIND_FOCUS, "SKILL_MIND_FOCUS"},
    {SKILL_PSI_BLAST, "SKILL_PSI_BLAST"},
    {SKILL_MIND_THRUST, "SKILL_MIND_THRUST"},
    {SKILL_PSYCHIC_CRUSH, "SKILL_PSYCHIC_CRUSH"},
    {SKILL_KINETIC_WAVE, "SKILL_KINETIC_WAVE"},
    {SKILL_MIND_PRESERVATION, "SKILL_MIND_PRESERVATION"},
    {SKILL_TELEKINESIS, "SKILL_TELEKINESIS"},
    {SKILL_PSIDRAIN, "SKILL_PSIDRAIN"},
    {SKILL_DIMENSIONAL_FOLD, "SKILL_DIMENSIONAL_FOLD"},
    {SKILL_SIGN, "SKILL_SIGN"},
    {SKILL_SWIM, "SKILL_SWIM"},
    {SKILL_CONS_UNDEAD, "SKILL_CONS_UNDEAD"},
    {SKILL_CONS_VEGGIE, "SKILL_CONS_VEGGIE"},
    {SKILL_CONS_DEMON, "SKILL_CONS_DEMON"},
    {SKILL_CONS_ANIMAL, "SKILL_CONS_ANIMAL"},
    {SKILL_CONS_REPTILE, "SKILL_CONS_REPTILE"},
    {SKILL_CONS_PEOPLE, "SKILL_CONS_PEOPLE"},
    {SKILL_CONS_GIANT, "SKILL_CONS_GIANT"},
    {SKILL_CONS_OTHER, "SKILL_CONS_OTHER"},
    {SKILL_READ_MAGIC, "SKILL_READ_MAGIC"},
    {SKILL_BANDAGE, "SKILL_BANDAGE"},
    {SKILL_CLIMB, "SKILL_CLIMB"},
    {SKILL_FAST_HEAL, "SKILL_FAST_HEAL"},
    {SKILL_EVALUATE, "SKILL_EVALUATE"},
    {SKILL_TACTICS, "SKILL_TACTICS"},
    {SKILL_DISSECT, "SKILL_DISSECT"},
    {SKILL_DEFENSE, "SKILL_DEFENSE"},
    {SKILL_OFFENSE, "SKILL_OFFENSE"},
    {SKILL_WHITTLE, "SKILL_WHITTLE"},
    {SKILL_WIZARDRY, "SKILL_WIZARDRY"},
    {SKILL_MEDITATE, "SKILL_MEDITATE"},
    {SKILL_DEVOTION, "SKILL_DEVOTION"},
    {SKILL_PENANCE, "SKILL_PENANCE"},
    {SKILL_SLASH_PROF, "SKILL_SLASH_PROF"},
    {SKILL_PIERCE_PROF, "SKILL_PIERCE_PROF"},
    {SKILL_BLUNT_PROF, "SKILL_BLUNT_PROF"},
    {SKILL_BAREHAND_PROF, "SKILL_BAREHAND_PROF"},
    {SKILL_SLASH_SPEC, "SKILL_SLASH_SPEC"},
    {SKILL_BLUNT_SPEC, "SKILL_BLUNT_SPEC"},
    {SKILL_PIERCE_SPEC, "SKILL_PIERCE_SPEC"},
    {SKILL_BAREHAND_SPEC, "SKILL_BAREHAND_SPEC"},
    {SKILL_RANGED_SPEC, "SKILL_RANGED_SPEC"},
    {SKILL_RANGED_PROF, "SKILL_RANGED_PROF"},
    {SKILL_FAST_LOAD, "SKILL_FAST_LOAD"},
    {SKILL_SHARPEN, "SKILL_SHARPEN"},
    {SKILL_DULL, "SKILL_DULL"},
    {SKILL_ATTUNE, "SKILL_ATTUNE"},
    {SKILL_STAVECHARGE, "SKILL_STAVECHARGE"},
    {SKILL_ALCOHOLISM, "SKILL_ALCOHOLISM"},
    {SKILL_FISHING, "SKILL_FISHING"},
    {SKILL_LOGGING, "SKILL_LOGGING"},
    {SKILL_ADVANCED_DEFENSE, "SKILL_ADVANCED_DEFENSE"},
    {SKILL_FOCUSED_AVOIDANCE, "SKILL_FOCUSED_AVOIDANCE"},
    {SKILL_TOUGHNESS, "SKILL_TOUGHNESS"},
    {SKILL_MANA, "SKILL_MANA"},
    {SKILL_MEND, "SKILL_MEND"},
    {SKILL_FORAGE, "SKILL_FORAGE"},
    {SKILL_SEEKWATER, "SKILL_SEEKWATER"},
    {SKILL_SKIN, "SKILL_SKIN"},
    {SKILL_DIVINATION, "SKILL_DIVINATION"},
    {SKILL_ENCAMP, "SKILL_ENCAMP"},
    {SKILL_HIKING, "SKILL_HIKING"},
    {SKILL_FISHLORE, "SKILL_FISHLORE"},
    {SKILL_GUTTER_CANT, "SKILL_GUTTER_CANT"},
    {SKILL_GNOLL_JARGON, "SKILL_GNOLL_JARGON"},
    {SKILL_TROGLODYTE_PIDGIN, "SKILL_TROGLODYTE_PIDGIN"},
    {SKILL_TROLLISH, "SKILL_TROLLISH"},
    {SKILL_BULLYWUGCROAK, "SKILL_BULLYWUGCROAK"},
    {SKILL_AVIAN, "SKILL_AVIAN"},
    {SKILL_FISHBURBLE, "SKILL_FISHBURBLE"},
    {SKILL_ADVANCED_OFFENSE, "SKILL_ADVANCED_OFFENSE"},
    {SKILL_INEVITABILITY, "SKILL_INEVITABILITY"},
    {SKILL_COMMON, "SKILL_COMMON"},
    {MAX_SKILL, "MAX_SKILL"},
    {TYPE_HIT, "TYPE_HIT"},
    {TYPE_BLUDGEON, "TYPE_BLUDGEON"},
    {TYPE_WHIP, "TYPE_WHIP"},
    {TYPE_CRUSH, "TYPE_CRUSH"},
    {TYPE_SMASH, "TYPE_SMASH"},
    {TYPE_SMITE, "TYPE_SMITE"},
    {TYPE_PUMMEL, "TYPE_PUMMEL"},
    {TYPE_FLAIL, "TYPE_FLAIL"},
    {TYPE_BEAT, "TYPE_BEAT"},
    {TYPE_THRASH, "TYPE_THRASH"},
    {TYPE_THUMP, "TYPE_THUMP"},
    {TYPE_WALLOP, "TYPE_WALLOP"},
    {TYPE_BATTER, "TYPE_BATTER"},
    {TYPE_STRIKE, "TYPE_STRIKE"},
    {TYPE_CLUB, "TYPE_CLUB"},
    {TYPE_POUND, "TYPE_POUND"},
    {TYPE_PIERCE, "TYPE_PIERCE"},
    {TYPE_BITE, "TYPE_BITE"},
    {TYPE_STING, "TYPE_STING"},
    {TYPE_STAB, "TYPE_STAB"},
    {TYPE_THRUST, "TYPE_THRUST"},
    {TYPE_SPEAR, "TYPE_SPEAR"},
    {TYPE_BEAK, "TYPE_BEAK"},
    {TYPE_SLASH, "TYPE_SLASH"},
    {TYPE_CLAW, "TYPE_CLAW"},
    {TYPE_CLEAVE, "TYPE_CLEAVE"},
    {TYPE_SLICE, "TYPE_SLICE"},
    {TYPE_AIR, "TYPE_AIR"},
    {TYPE_EARTH, "TYPE_EARTH"},
    {TYPE_FIRE, "TYPE_FIRE"},
    {TYPE_WATER, "TYPE_WATER"},
    {TYPE_BEAR_CLAW, "TYPE_BEAR_CLAW"},
    {TYPE_KICK, "TYPE_KICK"},
    {TYPE_MAUL, "TYPE_MAUL"},
    {TYPE_SHOOT, "TYPE_SHOOT"},
    {TYPE_CANNON, "TYPE_CANNON"},
    {TYPE_SHRED, "TYPE_SHRED"},
    {TYPE_MAX_HIT, "TYPE_MAX_HIT"},
    {AFFECT_TRANSFORMED_HANDS, "AFFECT_TRANSFORMED_HANDS"},
    {AFFECT_TRANSFORMED_ARMS, "AFFECT_TRANSFORMED_ARMS"},
    {AFFECT_TRANSFORMED_LEGS, "AFFECT_TRANSFORMED_LEGS"},
    {AFFECT_TRANSFORMED_HEAD, "AFFECT_TRANSFORMED_HEAD"},
    {AFFECT_TRANSFORMED_NECK, "AFFECT_TRANSFORMED_NECK"},
    {LAST_TRANSFORMED_LIMB, "LAST_TRANSFORMED_LIMB"},
    {SPELL_FIRE_BREATH, "SPELL_FIRE_BREATH"},
    {SPELL_CHLORINE_BREATH, "SPELL_CHLORINE_BREATH"},
    {SPELL_FROST_BREATH, "SPELL_FROST_BREATH"},
    {SPELL_ACID_BREATH, "SPELL_ACID_BREATH"},
    {SPELL_LIGHTNING_BREATH, "SPELL_LIGHTNING_BREATH"},
    {SPELL_DUST_BREATH, "SPELL_DUST_BREATH"},
    {LAST_BREATH_WEAPON, "LAST_BREATH_WEAPON"},
    {AFFECT_DUMMY, "AFFECT_DUMMY"},
    {AFFECT_DRUNK, "AFFECT_DRUNK"},
    {AFFECT_NEWBIE, "AFFECT_NEWBIE"},
    {AFFECT_SKILL_ATTEMPT, "AFFECT_SKILL_ATTEMPT"},
    {AFFECT_FREE_DEATHS, "AFFECT_FREE_DEATHS"},
    {AFFECT_TEST_FIGHT_MOB, "AFFECT_TEST_FIGHT_MOB"},
    {AFFECT_DRUG, "AFFECT_DRUG"},
    {AFFECT_PET, "AFFECT_PET"},
    {AFFECT_CHARM, "AFFECT_CHARM"},
    {AFFECT_THRALL, "AFFECT_THRALL"},
    {AFFECT_ORPHAN_PET, "AFFECT_ORPHAN_PET"},
    {AFFECT_DISEASE, "AFFECT_DISEASE"},
    {AFFECT_COMBAT, "AFFECT_COMBAT"},
    {AFFECT_PLAYERKILL, "AFFECT_PLAYERKILL"},
    {AFFECT_PLAYERLOOT, "AFFECT_PLAYERLOOT"},
    {AFFECT_HORSEOWNED, "AFFECT_HORSEOWNED"},
    {AFFECT_GROWTH_POTION, "AFFECT_GROWTH_POTION"},
    {AFFECT_WARY, "AFFECT_WARY"},
    {AFFECT_DEFECTED, "AFFECT_DEFECTED"},
    {AFFECT_OFFER, "AFFECT_OFFER"},
    {AFFECT_OBJECT_USED, "AFFECT_OBJECT_USED"},
    {AFFECT_WAS_INDOORS, "AFFECT_WAS_INDOORS"},
    {AFFECT_FORTIFY, "AFFECT_FORTIFY"},
    {AFFECT_BITTEN_BY_VAMPIRE, "AFFECT_BITTEN_BY_VAMPIRE"},
    {AFFECT_IMMORTAL_BLESSING, "AFFECT_IMMORTAL_BLESSING"},
    {AFFECT_AION_BLESSING, "AFFECT_AION_BLESSING"},
    {AFFECT_SIDARTHA_BLESSING, "AFFECT_SIDARTHA_BLESSING"},
    {AFFECT_DAMESCENA_BLESSING, "AFFECT_DAMESCENA_BLESSING"},
    {AFFECT_BOZ_BLESSING, "AFFECT_BOZ_BLESSING"},
    {AFFECT_VASCO_BLESSING, "AFFECT_VASCO_BLESSING"},
    {AFFECT_CORAL_BLESSING, "AFFECT_CORAL_BLESSING"},
    {AFFECT_BUMP_BLESSING, "AFFECT_BUMP_BLESSING"},
    {AFFECT_ONSLAUGHT_BLESSING, "AFFECT_ONSLAUGHT_BLESSING"},
    {AFFECT_DASH_BLESSING, "AFFECT_DASH_BLESSING"},
    {AFFECT_DEIRDRE_BLESSING, "AFFECT_DEIRDRE_BLESSING"},
    {AFFECT_GARTHAGK_BLESSING, "AFFECT_GARTHAGK_BLESSING"},
    {AFFECT_MERCURY_BLESSING, "AFFECT_MERCURY_BLESSING"},
    {AFFECT_METROHEP_BLESSING, "AFFECT_METROHEP_BLESSING"},
    {AFFECT_MAGDALENA_BLESSING, "AFFECT_MAGDALENA_BLESSING"},
    {AFFECT_MACROSS_BLESSING, "AFFECT_MACROSS_BLESSING"},
    {AFFECT_PAPPY_BLESSING, "AFFECT_PAPPY_BLESSING"},
    {AFFECT_STAFFA_BLESSING, "AFFECT_STAFFA_BLESSING"},
    {AFFECT_UNHOLY_WRATH, "AFFECT_UNHOLY_WRATH"},
    {AFFECT_HOLY_WRATH, "AFFECT_HOLY_WRATH"},
    {AFFECT_HOLY_BEAM, "AFFECT_HOLY_BEAM"},
    {AFFECT_GUARDIANS_LIGHT, "AFFECT_GUARDIANS_LIGHT"},
    {AFFECT_PREENED, "AFFECT_PREENED"},
    {AFFECT_WET, "AFFECT_WET"},
    {LAST_ODDBALL_AFFECT, "LAST_ODDBALL_AFFECT"},
    {ABSOLUTE_MAX_SKILL, "ABSOLUTE_MAX_SKILL"},
  };
}

// returns DELETE_THIS
int TBeing::doDiscipline(spellNumT which, const sstring& n1) {
  TObj* o = NULL;
  TThing* t = NULL;
  TBeing* ch = NULL;
  int rc = 0;
  char arg[256];
  const char* n = n1.c_str();

  auto it = spellNumToName.find(which);
  if (it == spellNumToName.end()) {
    vlogf(LOG_BUG, format("doDiscipline called with bad spellNumT %d") % which);
    return FALSE;
  }

  if (n1.empty())
    vlogf(LOG_SILENT, format("doDiscipline: %s (%s): %s") % name % number %
                        spellNumToName[which]);
  else
    vlogf(LOG_SILENT, format("doDiscipline: %s (%s): %s on %s") % name %
                        number % spellNumToName[which] % n1);

  if (!discArray[which]) {
    vlogf(LOG_BUG,
      format("doDiscipline called with null discArray[] (%d) (%s)") % which %
        getName());
    return FALSE;
  }
  if (which <= TYPE_UNDEFINED)
    return FALSE;

  if (!preDiscCheck(which))
    return FALSE;

  strncpy(arg, n, cElements(arg) - 1);
  arg[cElements(arg) - 1] = '\0';
  if (!parseTarget(which, arg, &t))
    return FALSE;

  ch = dynamic_cast<TBeing*>(t);
  o = dynamic_cast<TObj*>(t);

#if 0
// COSMO CASTING MARKER
// check if this is right
  if (isPc() && canSpeak()) {
    if (discArray[which]->minMana &&
        (getWizardryLevel() < WIZ_LEV_NO_MANTRA))
      saySpell(which);
  }
  if (isPc() && canSpeak()) {
    if (discArray[which]->minLifeforce &&
        (getRitualismLevel() < RIT_LEV_NO_MANTRA))
      saySpell(which);
  }
#endif
  if (isPc() && canSpeak()) {
    if (discArray[which]->holyStrength &&
        IS_SET(discArray[which]->comp_types, SPELL_TASKED_EVERY) &&
        (getDevotionLevel() < DEV_LEV_NO_MANTRA))
      saySpell(which);
  }
  // We can call the spell now, switch to see which to call!
  //    sendTo("Ok.\n\r");

  // COSMO MARKER: Mana..Piety checked here ...have to change useMana too
  if (isPc()) {
    if (discArray[which]->minMana) {
      if (!reconcileMana(which, TRUE))
        return FALSE;
    } else if (discArray[which]->minLifeforce) {
      if (!reconcileLifeforce(which, TRUE))
        return FALSE;
    } else {
      if (!reconcilePiety(which, TRUE))
        return FALSE;
    }
  }
  your_deity_val = which;
  inPraying = TRUE;

  switch (which) {
    case SPELL_GUST:
      rc = gust(this, ch);
      break;
    case SPELL_IMMOBILIZE:
      rc = immobilize(this, ch);
      break;
    case SPELL_SUFFOCATE:
      rc = suffocate(this, ch);
      break;
    case SPELL_DUST_STORM:
      rc = dustStorm(this);
      break;
    case SPELL_TORNADO:
      tornado(this);
      break;
    case SPELL_CONJURE_AIR:
      rc = conjureElemAir(this);
      break;
    case SPELL_ENTHRALL_SPECTRE:
      rc = enthrallSpectre(this);
      break;
    case SPELL_ENTHRALL_GHAST:
      rc = enthrallGhast(this);
      break;
    case SPELL_ENTHRALL_GHOUL:
      rc = enthrallGhoul(this);
      break;
    case SPELL_ENTHRALL_DEMON:
      rc = enthrallDemon(this);
      break;
    case SPELL_CREATE_WOOD_GOLEM:
      rc = createWoodGolem(this);
      break;
    case SPELL_CREATE_ROCK_GOLEM:
      rc = createRockGolem(this);
      break;
    case SPELL_CREATE_IRON_GOLEM:
      rc = createIronGolem(this);
      break;
    case SPELL_CREATE_DIAMOND_GOLEM:
      rc = createDiamondGolem(this);
      break;
    case SPELL_FEATHERY_DESCENT:
      rc = featheryDescent(this, ch);
      break;
    case SPELL_FLY:
      rc = fly(this, ch);
      break;
    case SPELL_ANTIGRAVITY:
      rc = antigravity(this);
      break;
    case SPELL_FALCON_WINGS:
      rc = falconWings(this, ch);
      break;
    case SPELL_LEVITATE:
      levitate(this, ch);
      break;
    case SPELL_PROTECTION_FROM_AIR:
      rc = protectionFromAir(this);
      break;
    case SPELL_DJALLA:
      rc = djallasProtection(this, ch);
      break;
    case SPELL_LEGBA:
      rc = legbasGuidance(this, ch);
      break;
    case SPELL_IDENTIFY:
      if (!o) {
        rc = identify(this, ch);
      } else
        rc = identify(this, o);
      break;
    case SPELL_DIVINATION:
      if (!o) {
        rc = divinationBeing(this, ch);
      } else
        rc = divinationObj(this, o);
      break;
    case SPELL_EYES_OF_FERTUMAN:
      rc = eyesOfFertuman(this, n);
      break;
    case SPELL_POWERSTONE:
      rc = powerstone(this, o);
      break;
    case SPELL_SHATTER:
      rc = shatter(this, ch);
      break;
    case SPELL_FARLOOK:
      rc = farlook(this, ch);
      break;
    case SPELL_ILLUMINATE:
      rc = illuminate(this, o);
      break;
    case SPELL_DETECT_MAGIC:
      rc = detectMagic(this, ch);
      break;
    case SPELL_DISPEL_MAGIC:
      if (!o) {
        rc = dispelMagic(this, ch);
      } else
        rc = dispelMagic(this, o);
      break;
    case SPELL_COPY:
      rc = copy(this, o);
      break;
    case SPELL_ENHANCE_WEAPON:
      rc = enhanceWeapon(this, o);
      break;
    case SPELL_MATERIALIZE:
      materialize(this, n);
      break;
    case SPELL_SPONTANEOUS_GENERATION:
      spontaneousGeneration(this, n);
      break;
    case SPELL_ETHER_GATE:
      etherealGate(this, n);
      break;
    case SPELL_GALVANIZE:
      rc = galvanize(this, o);
      break;
    case SPELL_SLING_SHOT:
      rc = slingShot(this, ch);
      break;
    case SPELL_GRANITE_FISTS:
      rc = graniteFists(this, ch);
      break;
    case SPELL_METEOR_SWARM:
      rc = meteorSwarm(this, ch);
      break;
    case SPELL_PEBBLE_SPRAY:
      rc = pebbleSpray(this);
      break;
    case SPELL_SAND_BLAST:
      rc = sandBlast(this);
      break;
    case SPELL_LAVA_STREAM:
      rc = lavaStream(this);
      break;
    case SPELL_STONE_SKIN:
      rc = stoneSkin(this, ch);
      break;
    case SPELL_TRAIL_SEEK:
      rc = trailSeek(this, ch);
      break;
    case SPELL_CONJURE_EARTH:
      conjureElemEarth(this);
      break;
    case SPELL_PROTECTION_FROM_EARTH:
      protectionFromEarth(this);
      break;
    case SPELL_HANDS_OF_FLAME:
      rc = handsOfFlame(this, ch);
      break;
    case SPELL_FAERIE_FIRE:
      faerieFire(this, ch);
      break;
    case SPELL_FLAMING_SWORD:
      rc = flamingSword(this, ch);
      break;
    case SPELL_LAVA_LANCE:
      rc = lavaLance(this, ch);
      break;
    case SPELL_INFERNO:
      rc = inferno(this, ch);
      break;
    case SPELL_FIREBALL:
      rc = fireball(this);
      break;
    case SPELL_HELLFIRE:
      rc = hellfire(this);
      break;
    case SPELL_FLAMING_FLESH:
      rc = flamingFlesh(this, ch);
      break;
    case SPELL_CONJURE_FIRE:
      rc = conjureElemFire(this);
      break;
    case SPELL_STUPIDITY:
      stupidity(this, ch);
      break;
    case SPELL_CHASE_SPIRIT:
      if (!o) {
        rc = chaseSpirits(this, ch);
      } else
        rc = chaseSpirits(this, o);
      break;
    case SPELL_FLARE:
      rc = flare(this);
      break;
    case SPELL_INFRAVISION:
      infravision(this, ch);
      break;
    case SPELL_PROTECTION_FROM_FIRE:
      protectionFromFire(this);
      break;
    case SPELL_MYSTIC_DARTS:
      rc = mysticDarts(this, ch);
      break;
    case SPELL_STUNNING_ARROW:
      rc = stunningArrow(this, ch);
      break;
    case SPELL_STICKS_TO_SNAKES:
      sticksToSnakes(this, ch);
      break;
    case SPELL_DISTORT:
      rc = distort(this, ch);
      break;
    case SPELL_DEATHWAVE:
      rc = deathWave(this, ch);
      break;
    case SPELL_SOUL_TWIST:
      rc = soulTwist(this, ch);
      break;
    case SPELL_BLOOD_BOIL:
      rc = bloodBoil(this, ch);
      break;
    case SPELL_FLATULENCE:
      rc = flatulence(this);
      break;
    case SPELL_BLAST_OF_FURY:
      rc = blastOfFury(this, ch);
      break;
    case SPELL_ENERGY_DRAIN:
      rc = energyDrain(this, ch);
      break;
    case SPELL_ATOMIZE:
      rc = atomize(this, ch);
      break;
    case SPELL_COLOR_SPRAY:
      rc = colorSpray(this);
      break;
    case SPELL_ACID_BLAST:
      rc = acidBlast(this);
      break;
#if 0
    case SPELL_CHAIN_LIGHTNING:
      rc = chainLightning(this);
      break;
#endif
    case SPELL_ANIMATE:
      animate(this);
      break;
    case SPELL_SORCERERS_GLOBE:
      sorcerersGlobe(this, ch);
      break;
    case SPELL_BIND:
      bind(this, ch);
      break;
    case SPELL_TELEPORT:
      rc = teleport(this, ch);
      break;
    case SPELL_KNOT:
      rc = knot(this, ch);
      break;
    case SPELL_PROTECTION_FROM_ENERGY:
      protectionFromEnergy(this);
      break;
    case SPELL_SENSE_LIFE:
      senseLife(this, ch);
      break;
    case SPELL_SENSE_LIFE_SHAMAN:
      senseLifeShaman(this, ch);
      break;
    case SPELL_SILENCE:
      silence(this, ch);
      break;
    case SPELL_STEALTH:
      stealth(this, ch);
      break;
    case SPELL_CALM:
      calm(this, ch);
      break;
    case SPELL_SQUISH:
      rc = squish(this, ch);
      break;
    case SPELL_ENSORCER:
      rc = ensorcer(this, ch);
      break;
    case SPELL_FEAR:
      rc = fear(this, ch);
      break;
    case SPELL_INTIMIDATE:
      rc = intimidate(this, ch);
      break;
    case SPELL_INVISIBILITY:
      if (!o) {
        invisibility(this, ch);
      } else
        invisibility(this, o);
      break;
    case SPELL_CLOUD_OF_CONCEALMENT:
      rc = cloudOfConcealment(this);
      break;
    case SPELL_DETECT_INVISIBLE:
      detectInvisibility(this, ch);
      break;
    case SPELL_DETECT_SHADOW:
      detectShadow(this, ch);
      break;
    case SPELL_DISPEL_INVISIBLE:
      if (!o)
        dispelInvisible(this, ch);
      else
        dispelInvisible(this, o);
      break;
    case SPELL_TELEPATHY:
      telepathy(this, n);
      break;
    case SPELL_ROMBLER:
      rombler(this, n);
      break;
    case SPELL_RAZE:
      rc = raze(this, ch);
      break;
    case SPELL_TRUE_SIGHT:
      trueSight(this, ch);
      break;
    case SPELL_POLYMORPH:
      polymorph(this, n);
      break;
    case SPELL_ACCELERATE:
      accelerate(this, ch);
      break;
    case SPELL_HASTE:
      haste(this, ch);
      break;
    case SPELL_SLUMBER:
      rc = slumber(this, ch);
      break;
    case SPELL_FUMBLE:
      rc = fumble(this, ch);
      break;
    case SPELL_GUSHER:
      rc = gusher(this, ch);
      break;
    case SPELL_AQUATIC_BLAST:
      rc = aquaticBlast(this, ch);
      break;
    case SPELL_CARDIAC_STRESS:
      rc = cardiacStress(this, ch);
      break;
    case SPELL_FAERIE_FOG:
      faerieFog(this);
      break;
    case SPELL_ICY_GRIP:
      rc = icyGrip(this, ch);
      break;
    case SPELL_WATERY_GRAVE:
      rc = wateryGrave(this, ch);
      break;
    case SPELL_ARCTIC_BLAST:
      rc = arcticBlast(this);
      break;
    case SPELL_ICE_STORM:
      rc = iceStorm(this);
      break;
    case SPELL_TSUNAMI:
      rc = tsunami(this);
      break;
    case SPELL_CONJURE_WATER:
      conjureElemWater(this);
      break;
    case SPELL_GILLS_OF_FLESH:
      gillsOfFlesh(this, ch);
      break;
    case SPELL_AQUALUNG:
      aqualung(this, ch);
      break;
    case SPELL_BREATH_OF_SARAHAGE:
      rc = breathOfSarahage(this);
      break;
    case SPELL_PROTECTION_FROM_WATER:
      protectionFromWater(this);
      break;
    case SPELL_PLASMA_MIRROR:
      rc = plasmaMirror(this);
      break;
    case SPELL_GARMULS_TAIL:
      rc = garmulsTail(this, ch);
      break;
    case SPELL_FLAMESTRIKE:
      rc = flamestrike(this, ch);
      break;
    case SPELL_CURSE:
    case SPELL_CURSE_DEIKHAN:
      if (!o)
        curse(this, ch);
      else
        curse(this, o);
      break;
    case SPELL_EARTHQUAKE:
      rc = earthquake(this);
      break;
    case SPELL_CALL_LIGHTNING:
      rc = callLightning(this, ch);
      break;
    case SPELL_PILLAR_SALT:
      rc = pillarOfSalt(this, ch);
      break;
    case SPELL_SPONTANEOUS_COMBUST:
      rc = spontaneousCombust(this, ch);
      break;
    case SPELL_PLAGUE_LOCUSTS:
      rc = plagueOfLocusts(this, ch);
      break;
    case SPELL_RAIN_BRIMSTONE:
      rc = rainBrimstone(this, ch);
      break;

    case SPELL_HARM_LIGHT:
    case SPELL_HARM_LIGHT_DEIKHAN:
      rc = harmLight(this, ch);
      break;
    case SPELL_HARM_SERIOUS:
    case SPELL_HARM_SERIOUS_DEIKHAN:
      rc = harmSerious(this, ch);
      break;
    case SPELL_HARM_CRITICAL:
    case SPELL_HARM_CRITICAL_DEIKHAN:
      rc = harmCritical(this, ch);
      break;
    case SPELL_HARM:
    case SPELL_HARM_DEIKHAN:
      rc = harm(this, ch);
      break;
    case SPELL_POISON:
    case SPELL_POISON_DEIKHAN:
      if (!o)
        poison(this, ch);
      else
        poison(this, o);
      break;
    case SPELL_PARALYZE_LIMB:
      rc = paralyzeLimb(this, ch);
      break;
    case SPELL_WITHER_LIMB:
      rc = witherLimb(this, ch);
      break;
    case SPELL_BONE_BREAKER:
      rc = boneBreaker(this, ch);
      break;
    case SPELL_PARALYZE:
      paralyze(this, ch);
      break;
    case SPELL_BLEED:
      rc = bleed(this, ch);
      break;
    case SPELL_INFECT:
    case SPELL_INFECT_DEIKHAN:
      infect(this, ch);
      break;
    case SPELL_DISEASE:
      disease(this, ch);
      break;
    case SPELL_BLINDNESS:
      blindness(this, ch);
      break;
    case SPELL_NUMB:
    case SPELL_NUMB_DEIKHAN:
      rc = numb(this, ch);
      break;
    case SPELL_HEAL_LIGHT:
    case SPELL_HEAL_LIGHT_DEIKHAN:
      healLight(this, ch);
      break;
    case SPELL_HEAL_SERIOUS:
    case SPELL_HEAL_SERIOUS_DEIKHAN:
      healSerious(this, ch);
      break;
    case SPELL_HEAL_CRITICAL:
    case SPELL_HEAL_CRITICAL_DEIKHAN:
      healCritical(this, ch);
      break;
    case SPELL_HEAL:
      heal(this, ch);
      break;
    case SPELL_HEAL_FULL:
      healFull(this, ch);
      break;
    case SPELL_HEAL_CRITICAL_SPRAY:
      healCritSpray(this);
      break;
    case SPELL_HEAL_SPRAY:
      healSpray(this);
      break;
    case SPELL_HEAL_FULL_SPRAY:
      healFullSpray(this);
      break;
    case SPELL_SALVE:
    case SPELL_SALVE_DEIKHAN:
      salve(this, ch);
      break;
    case SPELL_RESTORE_LIMB:
      restoreLimb(this, ch);
      break;
    case SPELL_KNIT_BONE:
      knitBone(this, ch);
      break;
    case SPELL_CLOT:
    case SPELL_CLOT_DEIKHAN:
      clot(this, ch);
      break;
    case SPELL_STERILIZE:
    case SPELL_STERILIZE_DEIKHAN:
      sterilize(this, ch);
      break;
    case SPELL_EXPEL:
    case SPELL_EXPEL_DEIKHAN:
      expel(this, ch);
      break;
    case SPELL_HEALING_GRASP:
      healingGrasp(this, ch);
      break;
    case SPELL_CONTROL_UNDEAD:
      controlUndead(this, ch);
      break;
    case SPELL_CLARITY:
      clarity(this, ch);
      break;
    case SPELL_CLEANSE:
      cleanse(this, ch);
      break;
    case SPELL_LICH_TOUCH:
      rc = lichTouch(this, ch);
      break;
    case SPELL_CHEVAL:
      cheval(this, ch);
      break;
    case SPELL_CELERITE:
      celerite(this, ch);
      break;
    case SPELL_VAMPIRIC_TOUCH:
      rc = vampiricTouch(this, ch);
      break;
    case SPELL_LIFE_LEECH:
      rc = lifeLeech(this, ch);
      break;
    case SPELL_HYPNOSIS:
      rc = hypnosis(this, ch);
      break;
    case SPELL_THORNFLESH:
      rc = thornflesh(this);
      break;
    case SPELL_DEATH_MIST:
      rc = deathMist(this);
      break;
    case SPELL_SHIELD_OF_MISTS:
      shieldOfMists(this, ch);
      break;
    case SPELL_SHADOW_WALK:
      rc = shadowWalk(this, ch);
      break;
    case SPELL_ENLIVEN:
      enliven(this, ch);
      break;
    case SPELL_CHRISM:
      chrism(this, n);
      break;
    case SPELL_VOODOO:
      rc = voodoo(this, o);
      break;
    case SPELL_DANCING_BONES:
      rc = dancingBones(this, o);
      break;
    case SPELL_RESURRECTION:
      rc = resurrection(this, o);
      break;

    case SPELL_PORTAL:
      portal(this, n);
      break;
    case SPELL_CREATE_FOOD:
    case SPELL_CREATE_FOOD_DEIKHAN:
      createFood(this);
      break;
    case SPELL_CREATE_WATER:
    case SPELL_CREATE_WATER_DEIKHAN:
      createWater(this, o);
      break;
    case SPELL_HEROES_FEAST:
    case SPELL_HEROES_FEAST_DEIKHAN:
      heroesFeast(this);
      break;
    case SPELL_ASTRAL_WALK:
      rc = astralWalk(this, ch);
      break;
    case SPELL_WORD_OF_RECALL:
      wordOfRecall(this, ch);
      break;
    case SPELL_SUMMON:
      rc = summon(this, ch);
      break;
    case SPELL_SYNOSTODWEOMER:
      rc = synostodweomer(this, ch);
      break;
#if 0
      case SKILL_BARKSKIN:
          rc = barkskin(this, ch);
        break;
#endif
    case SPELL_ROOT_CONTROL:
      rootControl(this, ch);
      break;
    case SPELL_LIVING_VINES:
      livingVines(this, ch);
      break;
    case SPELL_STORMY_SKIES:
      rc = stormySkies(this, ch);
      break;
#if 0
      case SKILL_TRANSFORM_LIMB:
          transformLimb(this, n);
        break;
#endif
    case SPELL_TREE_WALK:
      rc = treeWalk(this, n);
      break;

#if 0
      case SKILL_BEAST_SOOTHER:
          rc = beastSoother(this, ch);
        break;
      case SKILL_BEAST_SUMMON:
          rc = beastSummon(this, n);
        break;
      case SKILL_TRANSFIX:
          rc = transfix(this, ch);
        break;

#endif
    case SPELL_BLESS_DEIKHAN:
    case SPELL_BLESS:
      if (!o)
        bless(this, ch);
      else
        bless(this, o);
      break;
    case SPELL_ARMOR_DEIKHAN:
    case SPELL_ARMOR:
      armor(this, ch);
      break;
    case SPELL_SANCTUARY:
      sanctuary(this, ch);
      break;
    case SPELL_CRUSADE:
      crusade(this);
      break;
    case SPELL_RELIVE:
      relive(this, ch);
      break;
    case SPELL_CURE_POISON:
    case SPELL_CURE_POISON_DEIKHAN:
      curePoison(this, ch);
      break;
    case SPELL_CURE_PARALYSIS:
      cureParalysis(this, ch);
      break;
    case SPELL_CURE_BLINDNESS:
      cureBlindness(this, ch);
      break;
    case SPELL_CURE_DISEASE:
    case SPELL_CURE_DISEASE_DEIKHAN:
      cureDisease(this, ch);
      break;
    case SPELL_REFRESH:
    case SPELL_REFRESH_DEIKHAN:
      refresh(this, ch);
      break;
    case SPELL_SECOND_WIND:
      secondWind(this, ch);
      break;
    case SPELL_SHAPESHIFT:
      shapeShift(this, n);
      break;
    case SPELL_REMOVE_CURSE:
    case SPELL_REMOVE_CURSE_DEIKHAN:
      if (!o)
        removeCurseBeing(ch);
      else
        removeCurseObj(o);
      break;
    case SPELL_EMBALM:
      embalm(this, o);
      break;

    default:
      sendTo("Spell or discipline not yet implemented!\n\r");
      return FALSE;
  }

  // COSMO MARKER: Mana..Piety taken through here ...have to change useMana too
  if (inPraying)
    inPraying = 0;

  if (isPc()) {
    if (discArray[which]->minMana) {
      reconcileMana(which, FALSE);
    } else if (discArray[which]->minLifeforce) {
      reconcileLifeforce(which, FALSE);
    } else {
      reconcilePiety(which, FALSE);
    }
  }
  // COSMO MARKER: Spell lag added here
  // Need to modify to seperate casting and lag spells
  // one combat round is 2 seconds
  if (!IS_SET(discArray[which]->comp_types, SPELL_TASKED))
    addSkillLag(which, rc);

#if 0
  if (cast)
    learnFromDoingUnusual(LEARN_UNUSUAL_NORM_LEARN, SKILL_CASTING, 20);
  else
    learnFromDoingUnusual(LEARN_UNUSUAL_NORM_LEARN, SKILL_PRAYING, 20);
#endif

  if (IS_SET_DELETE(rc, DELETE_VICT) && ch != this) {
    delete ch;
    ch = NULL;
  }
  if (o && IS_SET_DELETE(rc, DELETE_ITEM)) {
    delete o;
    o = NULL;
  }
  if ((IS_SET_DELETE(rc, DELETE_VICT) && ch == this) ||
      IS_SET_DELETE(rc, DELETE_THIS))
    return DELETE_THIS;

  // this handles the "offensive" spells
  if (discArray[which]->targets & TAR_VIOLENT) {
    if (ch)
      reconcileDamage(ch, 0, which);

    if (discArray[which]->targets & TAR_AREA) {
      bool found = false;
      for (StuffIter it = roomp->stuff.begin();
           it != roomp->stuff.end() && (t = *it); ++it) {
        TBeing* victim = dynamic_cast<TBeing*>(t);
        if (!victim)
          continue;
        if (!inGroup(*victim) && !victim->isImmortal()) {
          reconcileDamage(victim, 0, which);
          if (!found) {
            if (discArray[which]->minMana) {
              act("Your enemies sense your offensive magic.", FALSE, this, NULL,
                NULL, TO_CHAR);
              act("You sense offensive magic emanating from $n.", FALSE, this,
                NULL, NULL, TO_ROOM);
            } else if (discArray[which]->minLifeforce) {
              act(
                "Your enemies sense your harmful invokation is directed toward "
                "them.",
                FALSE, this, NULL, NULL, TO_CHAR);
              act("You sense evil from $n.", FALSE, this, NULL, NULL, TO_ROOM);
            } else {
              act("Your enemies sense your offensive praying.", FALSE, this,
                NULL, NULL, TO_CHAR);
              act("You sense offensive praying emanating from $n.", FALSE, this,
                NULL, NULL, TO_ROOM);
            }
          }
          found = true;
        }
      }
    }
  }
  updatePos();
  // this SHOULD return TRUE - in theory
  // hope I'm not fucking everything up by making it 'correct'
  return TRUE;
  //  return FALSE;
}

void buildWeatherDamMap() {
  int i;
  i = SPELL_GUST;
  discArray[i]->weatherData[Weather::RAINY] = -20;
  //    discArray[i]->weatherData[Weather::WINDY] = 20;
  i = SPELL_HANDS_OF_FLAME;
  discArray[i]->weatherData[Weather::RAINY] = 20;
  discArray[i]->weatherData[Weather::SNOWY] = 20;
}

void buildTerrainDamMap() {
  int i;
  i = SPELL_GUST;
  discArray[i]->sectorData[SECT_TEMPERATE_ATMOSPHERE] = 20;
  discArray[i]->sectorData[SECT_TEMPERATE_OCEAN] = -20;
  discArray[i]->sectorData[SECT_TEMPERATE_ROAD] = 5;
  i = SPELL_HANDS_OF_FLAME;
  discArray[i]->sectorData[SECT_TEMPERATE_ATMOSPHERE] = 10;
  discArray[i]->sectorData[SECT_TEMPERATE_OCEAN] = -50;
  discArray[i]->sectorData[SECT_TEMPERATE_ROAD] = -5;
}
