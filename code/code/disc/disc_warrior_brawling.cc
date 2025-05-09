#include "handler.h"
#include "extern.h"
#include "obj_base_weapon.h"
#include "room.h"
#include "being.h"
#include "disc_warrior_brawling.h"
#include "obj_base_clothing.h"

using std::max;
using std::min;

int TBeing::doBerserk() {
  int rc;

  if (!doesKnowSkill(SKILL_BERSERK)) {
    sendTo("You lack the bloodlust.\n\r");
    return FALSE;
  }
  if (checkBusy()) {
    return FALSE;
  }
  if (affectedBySpell(SKILL_BERSERK)) {
    sendTo("You are unable to work up the bloodlust at this time.\n\r");
    return FALSE;
  }
  if (affectedBySpell(SKILL_DISGUISE)) {
    sendTo(
      "You can't work up the bloodlust while pretending to be someone "
      "else.\n\r");
    return FALSE;
  }

  rc = berserk(this);
  if (IS_SET_DELETE(rc, DELETE_THIS))
    return DELETE_THIS;
  return FALSE;
}

int berserk(TBeing* caster) {
  int level;
  affectedData af;

  if (caster->riding) {
    act("Not while riding.", TRUE, caster, 0, 0, TO_CHAR);
    return FALSE;
  }
  if (caster->getCombatMode() == ATTACK_BERSERK) {
    act("You are already berserking!", TRUE, caster, 0, 0, TO_CHAR);
    return FALSE;
  }

  if (caster->checkPeaceful("This room is too tranquil to go berserk in.\n\r"))
    return FALSE;

  if (!caster->isPc())
    return FALSE;

  level = caster->getSkillLevel(SKILL_BERSERK);
  int bKnown = caster->getSkillValue(SKILL_BERSERK);
  if (caster->bSuccess(bKnown, SKILL_BERSERK)) {
    caster->setCombatMode(ATTACK_BERSERK);
    act("You go berserk!", TRUE, caster, 0, 0, TO_CHAR);
    act("$n goes berserk!", TRUE, caster, 0, 0, TO_ROOM);

    if (caster->getHit() > (caster->hitLimit() / 2)) {
      af.type = SKILL_BERSERK;
      af.modifier = ::number(caster->getSkillValue(SKILL_BERSERK),
        caster->getSkillValue(SKILL_BERSERK) * 2);
      af.level = level;
      //      af.duration = caster->getSkillValue(SKILL_BERSERK);
      af.duration = PERMANENT_DURATION;
      af.location = APPLY_HIT;
      af.bitvector = 0;
      caster->affectTo(&af, -1);

      af.location = APPLY_CURRENT_HIT;
      caster->affectTo(&af, -1);

      caster->sendTo(
        "Berserking increases your ability to withstand damage!\n\r");
    }

    if (!caster->fight())
      caster->goBerserk(NULL);
  } else {
    act("You try to go berserk and bite yourself in the tongue!", TRUE, caster,
      0, 0, TO_CHAR);
    act("$n bites $mself in the tongue while trying to go berserk!", TRUE,
      caster, 0, 0, TO_ROOM);
    if (caster->reconcileDamage(caster, 1, SKILL_BERSERK) == -1)
      return DELETE_THIS;

    af.type = SKILL_BERSERK;
    af.level = level;
    af.duration = Pulse::UPDATES_PER_MUDHOUR / 2;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = 0;
    caster->affectTo(&af, -1);
  }

  return TRUE;
}

int TBeing::doTaunt(const sstring& arg) {
  TBeing* victim;
  char name_buf[256];

  strcpy(name_buf, arg.c_str());

  if (!(victim = get_char_room_vis(this, name_buf))) {
    if (!(victim = fight())) {
      sendTo("Taunt whom?\n\r");
      return FALSE;
    }
  }
  if (!sameRoom(*victim)) {
    sendTo("That person isn't around.\n\r");
    return FALSE;
  }

  if (!doesKnowSkill(SKILL_TAUNT) ||

      (roomp && roomp->isRoomFlag(ROOM_PEACEFUL)) || victim == this ||
      noHarmCheck(victim) || checkBusy() || victim->isDumbAnimal() ||
      !canSpeak() || victim != fight()) {
    return doAction(arg, CMD_TAUNT);
  }

  if (bSuccess(SKILL_TAUNT)) {
    act("You taunt $N ruthlessly, drawing their ire.", FALSE, this, 0, victim,
      TO_CHAR);
    act("$n taunts you ruthlessly, drawing your ire.", FALSE, this, 0, victim,
      TO_VICT);
    act("$n taunts $N ruthlessly, drawing their ire.", FALSE, this, 0, victim,
      TO_NOTVICT);

    affectedData af;

    af.type = SKILL_TAUNT;
    af.level = getSkillValue(SKILL_TAUNT);
    af.duration = durationModify(SKILL_TAUNT,
      Pulse::COMBAT * ::number(1, (int)(af.level / 10)));
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = 0;
    victim->affectFrom(SKILL_TAUNT);
    victim->affectTo(&af, -1);

  } else {
    act("You taunt yourself ruthlessly, confusing yourself.", FALSE, this, 0,
      this, TO_CHAR);
    act("$n taunts $mself ruthlessly, confusing $mself.", FALSE, this, 0, this,
      TO_NOTVICT);
  }

  return TRUE;
}

namespace {
  class AdvancedBerserkSkill {
    public:
      spellNumT skillNum{MAX_SKILL};
      int weight{0};
      std::function<bool(const TBeing*)> canUseSkill{
        [](const TBeing*) { return true; }};

      double calcRealChance(const TBeing* ch) const;
  };

  bool isWieldingWeapon(const TBeing* ch) {
    return dynamic_cast<TBaseWeapon*>(ch->heldInPrimHand()) != nullptr;
  };

  bool victimIsStanding(const TBeing* ch) {
    return ch->fight() && ch->fight()->getPosition() >= POSITION_STANDING;
  }

  bool canHeadbutt(const TBeing* ch) {
    return ch->fight() && ch->canHeadbutt(ch->fight(), SILENT_YES);
  }

  const std::vector<AdvancedBerserkSkill> advancedBerserkSkills = {
    {SKILL_SLAM, 3, isWieldingWeapon},
    {SKILL_BASH, 1, victimIsStanding},
    {SKILL_FOCUS_ATTACK, 2, isWieldingWeapon},
    {SKILL_HEADBUTT, 3, canHeadbutt},
    {SKILL_KNEESTRIKE, 3, victimIsStanding},
    {SKILL_BODYSLAM, 1, victimIsStanding},
    {SKILL_SPIN, 1, victimIsStanding},
    {SKILL_STOMP, 3},
    {SKILL_DEATHSTROKE, 1, isWieldingWeapon},
  };

  const int roll_range = []() {
    int total = 0;
    for (const auto& skill : advancedBerserkSkills)
      total += skill.weight;
    return total;
  }();

  const AdvancedBerserkSkill* whichSkill(const int roll) {
    int total = 0;
    for (const auto& skill : advancedBerserkSkills) {
      total += skill.weight;
      if (roll <= total)
        return &skill;
    }
    return nullptr;
  };

  const AdvancedBerserkSkill* findValidSkill(const TBeing* ch, const int roll) {
    const auto* skill = whichSkill(roll);
    if (!skill || !ch->doesKnowSkill(skill->skillNum))
      return nullptr;
    return skill->canUseSkill(ch) ? skill
                                  : findValidSkill(ch, ::number(1, roll_range));
  }
}  // namespace

int TBeing::doAdvancedBerserk(TBeing* target) {
  static const sstring toChar = "You're overcome by your <R>berserker rage<z>!";
  static const sstring toRoom = "$n is overcome by <R>berserker rage<z>!";

  // Pulse::COMBAT is divided into 12 mini-rounds in perform_violence, so
  // percent chances in here need to be divided by that number to be accurate
  // for a single round of combat
  static constexpr double mini_pulses_per_round = 12.0;

  // Chance per combat round to gain a stack of bloodlust buff
  static constexpr double bloodlust_chance_per_round =
    15.0 / mini_pulses_per_round;

  // Chance per combat round for advanced berserk to execute a random skill from
  // advancedBerserkSkills
  static constexpr double skill_chance_per_round = 50.0 / mini_pulses_per_round;

  if (doesKnowSkill(SKILL_BLOODLUST) &&
      percentChance(bloodlust_chance_per_round))
    doBloodlust();

  if (!percentChance(skill_chance_per_round))
    return false;

  const AdvancedBerserkSkill* which =
    findValidSkill(this, ::number(1, roll_range));

  if (!which)
    return false;

  act(toChar, false, this, nullptr, nullptr, TO_CHAR);
  act(toRoom, false, this, nullptr, nullptr, TO_ROOM);

  const bool success = bSuccess(which->skillNum);

  switch (which->skillNum) {
    case SKILL_SLAM:
      return success ? slamSuccess(target) : slamFail(target);
    case SKILL_BASH: {
      auto* item = heldInSecHand();

      return success ? bashSuccess(target, SKILL_BASH, item && item->isShield(),
                         dynamic_cast<TObj*>(item))
                     : bashFail(target, SKILL_BASH, false, false, false);
    }
    case SKILL_FOCUS_ATTACK:
      return success ? focusAttackSuccess(target) : focusAttackFail(target);
    case SKILL_HEADBUTT:
      return success ? headbuttHit(target) : headbuttMiss(target);
    case SKILL_KNEESTRIKE:
      return success ? kneestrikeHit(target) : kneestrikeMiss(target, 0);
    case SKILL_BODYSLAM:
      return success ? bodyslamHit(target) : bodyslamMiss(target, TYPE_DEX);
    case SKILL_SPIN:
      return success ? spinHit(target) : spinMiss(target, TYPE_DEX);
    case SKILL_STOMP:
      return success ? stompHit(target) : stompMiss(target);
    case SKILL_DEATHSTROKE:
      return success ? deathstrokeSuccess(target) : deathstrokeFail(target);
    default:
      return false;
  }
}
