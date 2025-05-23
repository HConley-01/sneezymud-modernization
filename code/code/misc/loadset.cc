//////////////////////////////////////////////////////////////////////////
//
//      SneezyMUD++ - All rights reserved, SneezyMUD Coding Team
//
//      "loadset.cc" - Functions related to loading a set of EQ
//
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include <cmath>
#include "room.h"
#include "being.h"
#include "configuration.h"
#include "monster.h"
#include "extern.h"
#include "materials.h"
#include "loadset.h"
#include "obj_armor.h"
#include "obj_base_clothing.h"
#include "statistics.h"

extern int GetItemClassRestrictions(const TObj*);
extern bool IsRestricted(unsigned short int, unsigned short int);

extern void tallyObjLoadPotential(const int obj_num);
extern int getObjLoadPotential(const int obj_num);

loadSetClass suitSets;

const char* suitFilePath = "objdata/suitsets";

const char* suitPieceNames[] = {"head", "neck", "body", "arm", "hand", "waist",
  "wrist", "leg", "foot", "finger", "shield", "back", "\n"};

const char* suitTypeRaces[] = {"Unknown", "human", "elf", "dwarf", "hobbit",
  "gnome", "ogre", "Unknown", "\n"};

wearSlotT getSlotFromLST(loadSetTypeT tPiece, TBeing* ch, bool isFirst) {
  if (tPiece == LST_HELM)
    return WEAR_HEAD;
  if (tPiece == LST_COLLAR)
    return WEAR_NECK;
  if (tPiece == LST_JACKET)
    return WEAR_BODY;
  if (tPiece == LST_SLEEVE)
    return (isFirst ? WEAR_ARM_L : WEAR_ARM_R);
  if (tPiece == LST_GLOVE)
    return (isFirst ? WEAR_HAND_L : WEAR_HAND_R);
  if (tPiece == LST_BELT)
    return WEAR_WAIST;
  if (tPiece == LST_BRACELET)
    return (isFirst ? WEAR_WRIST_L : WEAR_WRIST_R);
  if (tPiece == LST_LEGGING)
    return (isFirst ? WEAR_LEG_L : WEAR_LEG_R);
  if (tPiece == LST_BOOT)
    return (isFirst ? WEAR_FOOT_L : WEAR_FOOT_R);
  if (tPiece == LST_RING)
    return (isFirst ? WEAR_FINGER_L : WEAR_FINGER_R);
  if (tPiece == LST_SHIELD)
    return ch->getSecondaryHold();
  if (tPiece == LST_CLOAK)
    return WEAR_BACK;

  return WEAR_NOWHERE;
}

bool loadSetClass::suitLoad(const char* argument, TBeing* ch,
  loadSetTypeT tPiece, int tChance, int sCount, bool findLoadPotential) {
  // soft 101 -1

  // Explicitly initialize tChance and sCount to safe values if not provided
  if (tChance < 0 || tChance > 100) tChance = 0;
  if (sCount < 0) sCount = 0;

  if (sCount <= 0 && (!argument || !*argument))
    return false;

  for (unsigned short int suitIndex = 0; suitIndex < suits.size(); suitIndex++)
    if ((suits[suitIndex].name && isname(argument, suits[suitIndex].name)) ||
        sCount-- == 1) {
      if (tPiece == LST_ALL) {
        for (int pieceIndex = 0; pieceIndex < LST_MAX; pieceIndex++) {
          // This is so hobbits will not load feet where in suits meant
          // for Gnome or Hobbit&Gnome.  Safty catch is all.
          if (ch && ch->getRace() == RACE_HOBBIT && pieceIndex == LST_BOOT)
            continue;

          if (findLoadPotential) {
            tallyObjLoadPotential(suits[suitIndex].equipment[pieceIndex]);
          } else {
            loadsetCheck(ch, suits[suitIndex].equipment[pieceIndex], tChance,
              getSlotFromLST(loadSetTypeT(pieceIndex), ch, false),
              suitPieceNames[pieceIndex]);
          }

          if (pieceIndex == LST_SLEEVE || pieceIndex == LST_GLOVE ||
              pieceIndex == LST_BRACELET || pieceIndex == LST_LEGGING ||
              pieceIndex == LST_BOOT || pieceIndex == LST_RING) {
            if (findLoadPotential) {
              tallyObjLoadPotential(suits[suitIndex].equipment[pieceIndex]);
            } else {
              loadsetCheck(ch, suits[suitIndex].equipment[pieceIndex], tChance,
                getSlotFromLST(loadSetTypeT(pieceIndex), ch, true),
                suitPieceNames[pieceIndex]);
            }
          }
        }
      } else if (findLoadPotential) {
        tallyObjLoadPotential(suits[suitIndex].equipment[tPiece]);
      } else {
        loadsetCheck(ch, suits[suitIndex].equipment[tPiece], tChance,
          getSlotFromLST(loadSetTypeT(tPiece), ch, false),
          suitPieceNames[tPiece]);
      }

      return true;
    }

  ch->sendTo(
    "That suit of armor is not presently supported for loading as a set.\n\r");
  return false;
}

// Chance comes in as 101 if this function is called via the imm loadset command
bool loadsetCheck(TBeing* ch, int vnum, int chance, wearSlotT slot,
  const sstring& slotname, resetFlag flags) {
  const bool isImmLoad = chance == 101;
  const bool isPropLoad = IS_SET(flags, resetFlagPropLoad);

  if (vnum < 0) {
    if (isImmLoad)
      ch->sendTo(format("No %s exists in that set.\n\r") % slotname);
    return false;
  }

  if (IS_SET(flags, resetFlagFindLoadPotential)) {
    tallyObjLoadPotential(vnum);
    return false;
  }

  // No reason to proceed if ch already has something equipped in this slot
  if (!isImmLoad && ch->equipment[slot])
    return false;

  objIndexData* objInfo = nullptr;
  const int index = real_object(vnum);

  try {
    objInfo = &obj_index.at(index);
    if (!objInfo)
      return false;
  } catch (const std::out_of_range&) {
    return false;
  }

  // Prevent items with low remaining max_exist space from being loaded as
  // props, so that rare items can't inadvertently be blocked from loading.
  // To use artifacts as props, copies of the items should be made with high
  // max_exist, to only be loaded via I/J commands.
  if (isPropLoad && (objInfo->max_exist - objInfo->getNumber() < 5)) {
    vlogf(LOG_LOW, format("Prevented prop load of object %d - %s. (max_exist - "
                          "current_exist) < 5") %
                     vnum % objInfo->name);
  }

  if (objInfo->getNumber() >= objInfo->max_exist) {
    if (!isImmLoad)
      return false;

    ch->sendTo(format("The %s in that suit is over max.\n\r") % slotname);

    // let L58+ gods load full set regardless
    if (!ch->hasWizPower(POWER_LOAD_LIMITED))
      return false;
  }

  // Scale load chance from zonefile by loadrate tweak value from db.
  const auto realChance = static_cast<int>(
    min(max(chance * tweakInfo[TWEAK_LOADRATE]->current, 0.0), 100.0));

  // Debug logging for load chance
  printf("DEBUG: loadsetCheck incoming chance=%d, tweak=%.3f, realChance=%d\n", chance, tweakInfo[TWEAK_LOADRATE]->current, realChance);

  bool passed = percentChance(realChance);
  vlogf(LOG_BUG, format("loadsetCheck: vnum=%d slot=%s percentChance result=%s") % vnum % slotname % (passed ? "PASS" : "FAIL"));
  if (!passed)
    return false;

  TObj* obj = isImmLoad || isPropLoad ? read_object(index, REAL)
                                      : read_object_buy_build(ch, index, REAL);

  if (!obj) {
    if (isImmLoad)
      ch->sendTo(format("The %s was listed but not found, not in db yet?\n\r") %
                 slotname);
    return false;
  }

  ch->logItem(obj, CMD_LOAD);

  if (!isImmLoad)
    log_object(obj);

  if (obj->isPaired() && slot == WEAR_LEG_L) {
    delete obj;  // avoid double loads of pants
    return false;
  }

  if (isPropLoad)
    markProp(*obj);

  if (isImmLoad || (Config::LoadOnDeath() && !isPropLoad))
    *ch += *obj;
  else
    ch->equipChar(obj, slot);

  // Most likely an immortal loading through loadset. If they cannot load with
  // no proto then they cannot load set with no proto.
  if (isImmLoad && !ch->hasWizPower(POWER_LOAD_NOPROTOS))
    obj->addObjStat(ITEM_PROTOTYPE);

  return true;
}

bool is_floatVal(const char* str) {
  if (!*str)
    return false;

  for (; *str; str++)
    if ((*str < '0' || *str > '9') && *str != '.')
      return false;

  return true;
}

void TBeing::loadSetEquipment(int num, char* arg, int tChance,
  bool findLoadPotential) {
  char tString[1024] = "\0", suitClasses[256];
  const char* tArg = NULL;
  loadSetTypeT tPiece = LST_ALL;
  sstring StString("");
  int tCount = 0;

  // Explicitly initialize tChance to a safe value if not provided
  if (tChance < 0 || tChance > 100) tChance = 0;

  if (num < 0 && (!arg || !*arg) && tChance != 101)
    return;

  if (arg && *arg) {
    for (; isspace(*arg); arg++)
      ;

    tArg = arg;
    tArg = one_argument(tArg, tString, cElements(tString));
  }

  if (tChance == 101) {
    if (*arg && !is_abbrev(tString, "information")) {
      const char* pString;
      char zString[256] = "\0", dString[256] = "\0";

      pString = tArg;
      for (; isspace(*pString); pString++)
        ;
      tArg = one_argument(pString, zString, cElements(zString));

      if (*zString) {
        int tPart;

        bisect_arg(zString, &tPart, dString, suitPieceNames);
        tPart--;

        if (tPart < 0 || tPart >= LST_MAX) {
          StString += "Illegal part.  Valids are:\n\r";

          for (int indexPart = 0; indexPart < LST_MAX; indexPart++) {
            snprintf(dString, sizeof(dString), "    %s\n\r", suitPieceNames[indexPart]);
            StString += dString;
          }

          if (isPc() && isImmortal() && desc)
            desc->page_string(StString);

          return;
        }

        tPiece = loadSetTypeT(tPart);
      }
    } else if (*arg) {
      const char* cArg = arg;
      char tSuit[256] = "\0";
      bool listMissing = false, hasMatch = false;
      double tLevelMin = -1, tLevelMax = -1;
      int suitRaces[7] = {0, 0, 0, 0, 0, 0, 0}, tRace = RACE_NORACE,
          tClass = -1;
      unsigned short int tTotal = 0, tPosbl = 0;

      cArg = one_argument(cArg, tString, cElements(tString));
      for (tArg = tString; isspace(*tArg); tArg++)
        ;

      if (*tArg)
        for (;; cArg = one_argument(cArg, tString, cElements(tString))) {
          for (tArg = tString; isspace(*tArg); tArg++)
            ;

          if (!*tArg)
            break;

          if (is_abbrev(tArg, "missing"))
            listMissing = true;
          else if (is_floatVal(tArg)) {
            if (tLevelMin == -1) {
              tLevelMin = convertTo<float>(tArg);

              if (tLevelMin < 0 || tLevelMin > 50) {
                sendTo("Invalid Min-Level.\n\r");
                return;
              }
            } else {
              tLevelMax = convertTo<float>(tArg);

              if (tLevelMax < 0 || tLevelMax > 50 || tLevelMax <= tLevelMin) {
                sendTo("Invalid Max-Level.\n\r");
                return;
              }
            }
          } else {
            hasMatch = false;

            for (int raceIndex = 1; raceIndex < 7; raceIndex++)
              if (is_abbrev(tArg, suitTypeRaces[raceIndex])) {
                tRace = raceIndex;
                hasMatch = true;
                break;
              }

            if (!hasMatch) {
              if (!is_abbrev(tArg, "information")) {
                strncpy(tSuit, tArg, sizeof(tSuit) - 1);
                tSuit[sizeof(tSuit) - 1] = '\0';
              }
            }
          }

          if (!*cArg)
            break;
        }

      StString += "Extended SuitSet Information:\n\r[ ";

      if (listMissing)
        StString += "[Missing] ";

      if (tRace != RACE_NORACE) {
        StString += "[";
        StString += suitTypeRaces[max(0, min(7, tRace))];
        StString += "] ";
      }

      if (tClass != -1) {
        StString += "[";

        if (tClass & (1 << 31))
          StString += classInfo[(tClass & ~(1 << 31))].name.cap().c_str();
        else
          StString += classInfo[tClass].name.c_str();

        StString += "] ";
      }

      if (*tSuit) {
        StString += "[";
        StString += tSuit;
        StString += "] ";
      }

      if (tLevelMin > -1) {
        snprintf(tString, sizeof(tString), "[%sLevel:%6.2f] ", (tLevelMax > -1 ? "Min-" : ""),
          tLevelMin);
        StString += tString;
      }

      if (tLevelMax > -1) {
        snprintf(tString, sizeof(tString), "[Max-Level:%6.2f] ", tLevelMax);
        StString += tString;
      }

      if (tLevelMax == -1 && tLevelMin > -1) {
        tLevelMin = max(0.0, (tLevelMin - 2));
        tLevelMax = max(0.0, (tLevelMin + 2));
      }

      StString += "]\n\r";

      for (unsigned short int suitIndex = 0; suitIndex < suitSets.suits.size();
           suitIndex++)
        if ((tRace == RACE_NORACE ||
              suitSets.suits[suitIndex].suitRace == race_t(tRace)) &&
            (tClass == -1 ||
              ((tClass & (1 << 31))
                  ? (suitSets.suits[suitIndex].suitClassTotal &
                      (1 << (tClass & ~(1 << 31))))
                  : (suitSets.suits[suitIndex].suitClassPossible &
                      (1 << tClass)))) &&
            (tLevelMin == -1 ||
              (suitSets.suits[suitIndex].suitLevel >= tLevelMin &&
                suitSets.suits[suitIndex].suitLevel <= tLevelMax)) &&
            (!*tSuit || (suitSets.suits[suitIndex].name &&
                          isname(tSuit, suitSets.suits[suitIndex].name)))) {
          if (suitSets.suits[suitIndex].suitRace > 0 &&
              suitSets.suits[suitIndex].suitRace < 7)
            suitRaces[suitSets.suits[suitIndex].suitRace - 1]++;
          else
            suitRaces[6]++;

          tTotal = suitSets.suits[suitIndex].suitClassTotal;
          tPosbl = suitSets.suits[suitIndex].suitClassPossible;

          snprintf(suitClasses, sizeof(suitClasses), "[%c%c%c%c%c%c%c%c]",
            (tTotal & CLASS_MAGE ? 'M' : (tPosbl & CLASS_MAGE ? 'm' : ' ')),
            (tTotal & CLASS_CLERIC ? 'C' : (tPosbl & CLASS_CLERIC ? 'c' : ' ')),
            (tTotal & CLASS_WARRIOR ? 'W'
                                    : (tPosbl & CLASS_WARRIOR ? 'w' : ' ')),
            (tTotal & CLASS_THIEF ? 'T' : (tPosbl & CLASS_THIEF ? 't' : ' ')),
            (tTotal & CLASS_SHAMAN ? 'A' : (tPosbl & CLASS_SHAMAN ? 'a' : ' ')),
            (tTotal & CLASS_DEIKHAN ? 'D'
                                    : (tPosbl & CLASS_DEIKHAN ? 'd' : ' ')),
            (tTotal & CLASS_MONK ? 'K' : (tPosbl & CLASS_MONK ? 'k' : ' ')),
            (tTotal & CLASS_RANGER ? 'R'
                                   : (tPosbl & CLASS_RANGER ? 'r' : ' ')));

          char suitNameShort[256];
          const char* tSuitShortMark;

          tSuitShortMark = suitSets.suits[suitIndex].name;
          one_argument(tSuitShortMark, suitNameShort, cElements(suitNameShort));
          snprintf(tString, sizeof(tString), "[%3d] %-15s   [%7s]   Lvl:%6.2f %s\n\r",
            (suitIndex + 1), suitNameShort,
            suitTypeRaces[max(0,
              min(7, (int)suitSets.suits[suitIndex].suitRace))],
            suitSets.suits[suitIndex].suitLevel, suitClasses);
          StString += tString;

          bool hasShownOne = false;

          if (!*tSuit && !listMissing)
            continue;

          for (int pieceIndex = 0; pieceIndex < LST_MAX; pieceIndex++)
            if (!*tSuit && listMissing) {
              if (suitSets.suits[suitIndex].equipment[pieceIndex] == -1) {
                snprintf(tString, sizeof(tString), "%s%s", (hasShownOne ? ", " : "      "),
                  suitPieceNames[pieceIndex]);
                StString += tString;
                hasShownOne = true;
              }
            } else {
              snprintf(suitClasses, sizeof(suitClasses), "[%c%c%c%c%c%c%c%c]",
                ((suitSets.suits[suitIndex].suitClass[pieceIndex] & CLASS_MAGE)
                    ? 'M'
                    : ' '),
                ((suitSets.suits[suitIndex].suitClass[pieceIndex] &
                   CLASS_CLERIC)
                    ? 'C'
                    : ' '),
                ((suitSets.suits[suitIndex].suitClass[pieceIndex] &
                   CLASS_WARRIOR)
                    ? 'W'
                    : ' '),
                ((suitSets.suits[suitIndex].suitClass[pieceIndex] &
                   CLASS_THIEF)
                    ? 'T'
                    : ' '),
                ((suitSets.suits[suitIndex].suitClass[pieceIndex] &
                   CLASS_SHAMAN)
                    ? 'A'
                    : ' '),
                ((suitSets.suits[suitIndex].suitClass[pieceIndex] &
                   CLASS_DEIKHAN)
                    ? 'D'
                    : ' '),
                ((suitSets.suits[suitIndex].suitClass[pieceIndex] &
                   CLASS_MONK)
                    ? 'K'
                    : ' '),
                ((suitSets.suits[suitIndex].suitClass[pieceIndex] &
                   CLASS_RANGER)
                    ? 'R'
                    : ' '));

              snprintf(tString, sizeof(tString), "  %-7s: %6d",
                sstring(suitPieceNames[pieceIndex]).cap().c_str(),
                suitSets.suits[suitIndex].equipment[pieceIndex]);
              StString += tString;

              if (suitSets.suits[suitIndex].equipment[pieceIndex] > -1) {
                TThing* tThing = NULL;
                TBaseClothing* tBCloth = NULL;

                int rNum =
                  real_object(suitSets.suits[suitIndex].equipment[pieceIndex]);
                if (rNum < 0 || rNum >= (signed)obj_index.size() ||
                    !(tThing = read_object(rNum, REAL)))
                  StString += " ***Not Found***\n\r";
                else {
                  if ((tBCloth = dynamic_cast<TBaseClothing*>(tThing)) &&
                      tBCloth->armorLevel(ARMOR_LEV_REAL))
                    snprintf(tString, sizeof(tString), " [R:%6.2f] %s [%s] [%s]\n\r",
                      tBCloth->armorLevel(ARMOR_LEV_REAL), suitClasses,
                      tBCloth->getName().c_str(),
                      material_nums[tBCloth->getMaterial()].mat_name);
                  else
                    snprintf(tString, sizeof(tString), " [R:      ] %s [%s] [%s]\n\r",
                      suitClasses, tThing->getName().c_str(),
                      material_nums[tThing->getMaterial()].mat_name);

                  StString += tString;
                  delete tThing;
                  tThing = NULL;
                }
              } else
                StString += "\n\r";
            }

          if (hasShownOne)
            StString += "\n\r";
        }

      for (int raceIndex = 0; raceIndex < 6; raceIndex++)
        if (suitRaces[raceIndex] > 0)
          tCount += suitRaces[raceIndex];

      for (int raceIndex = 0; raceIndex < 6; raceIndex++)
        if (suitRaces[raceIndex] > 0) {
          tCount = max(1, tCount);
          float tUsedPerc =
            (((float)suitRaces[raceIndex] / (float)tCount) * 100);

          snprintf(tString, sizeof(tString), " [%3.0f%%][%3d] %s\n\r", tUsedPerc,
            suitRaces[raceIndex],
            sstring(suitTypeRaces[raceIndex + 1]).cap().c_str());

          StString += tString;
        }

      if (suitRaces[6] > 0) {
        snprintf(tString, sizeof(tString), " Unknown[%d]\n\r", suitRaces[6]);
        StString += tString;
      } else
        StString += "\n\r";

      if (isPc() && isImmortal() && desc)
        desc->page_string(StString);

      return;
    } else {
      StString += "Basic SuitSet Information:\n\r";

      int tCount = 0, suitRaces[7] = {0, 0, 0, 0, 0, 0, 0};

      for (unsigned short int suitIndex = 0; suitIndex < suitSets.suits.size();
           suitIndex++) {
        char suitNameShort[256];
        const char* tSuitShortMark;

        tSuitShortMark = suitSets.suits[suitIndex].name;
        one_argument(tSuitShortMark, suitNameShort, cElements(suitNameShort));

        snprintf(tString, sizeof(tString), "     [%3d] %-15s", ++tCount, suitNameShort);
        StString += tString;

        if ((tCount % 3) == 0)
          StString += "\n\r";

        if (suitSets.suits[suitIndex].suitRace > 0 &&
            suitSets.suits[suitIndex].suitRace < 7)
          suitRaces[suitSets.suits[suitIndex].suitRace - 1]++;
        else
          suitRaces[6]++;
      }

      if ((tCount % 3) != 0) {
        StString += "\n\r";
        StString += "\n\r";
      }
      snprintf(tString, sizeof(tString),
        "Total Suits: Ogre[%d] Human[%d] Elf[%d] Dwarf[%d] Gnome[%d] "
        "Hobbit[%d]",
        suitRaces[5], suitRaces[0], suitRaces[1], suitRaces[2], suitRaces[4],
        suitRaces[3]);
      StString += tString;

      if (suitRaces[6] > 0) {
        snprintf(tString, sizeof(tString), " **Unknown[%d]**\n\r", suitRaces[6]);
        StString += tString;
      } else
        StString += "\n\r";

      if (isPc() && isImmortal() && desc)
        desc->page_string(StString);

      return;
    }
  }
  if (num < 0 && !*tString)
    return;

  if (*tString && is_number(tString)) {
    num = convertTo<int>(tString);
    tString[0] = '\0';
  }

  if (!suitSets.suitLoad(tString, this, tPiece, tChance, num,
        findLoadPotential))
    return;

  if (tChance == 101) {
    if (tPiece == LST_ALL) {
      sendTo("You load a set of armor.\n\r");
      act("$n loads a set of armor.", TRUE, this, 0, 0, TO_ROOM);
    } else {
      sendTo(format("You load a %s piece from a suit of armor.\n\r") %
             suitPieceNames[tPiece]);
      act("$n loads a piece of armor.", TRUE, this, 0, 0, TO_ROOM);
    }
  }
}

void loadSetClass::SetupLoadSetSuits() {
  FILE* suitFile;
  char tString[256], tBuffer[256], tName[256] = "";
  int equipment[12] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, tPiece;
  race_t tRace = RACE_NORACE;
  bool hasSuit = false;

  if (!(suitFile = fopen(suitFilePath, "r"))) {
    vlogf(LOG_FILE, format("Unable to open '%s' for reading.") % suitFilePath);
    return;
  }

  while (1) {
    if (!fgets(tString, sizeof(tString), suitFile))
      vlogf(LOG_FILE, format("Unexpected read error on '%s'") % suitFilePath);

    if (tString[0] == ':')
      continue;

    if (tString[0] == '#' || !strncmp("!*EOF*", tString, 6)) {
      if (hasSuit || !strncmp("!*EOF*", tString, 6)) {
        suitAdd(tName, equipment[0], equipment[1], equipment[2], equipment[3],
          equipment[4], equipment[5], equipment[6], equipment[7], equipment[8],
          equipment[9], equipment[10], equipment[11], tRace);
        hasSuit = false;
      }

      if (!strncmp("!*EOF*", tString, 6)) {
        fclose(suitFile);
        return;
      }

      strncpy(tName, tString + 1, sizeof(tName) - 1);
      tName[sizeof(tName) - 1] = '\0';

      if (strlen(tName) > 0) {
        hasSuit = true;
        equipment[0] = equipment[1] = equipment[2] = equipment[3] =
          equipment[4] = equipment[5] = equipment[6] = equipment[7] =
            equipment[8] = equipment[9] = equipment[10] = equipment[11] = -1;
        tRace = RACE_NORACE;
      }
    } else if (tString[0] == '$') {
      strncpy(tBuffer, tString + 1, sizeof(tBuffer) - 1);
      tBuffer[sizeof(tBuffer) - 1] = '\0';

      if (*tBuffer)
        tBuffer[max(0, (int)(strlen(tBuffer) - 1))] = '\0';

      bisect_arg(tBuffer, &tPiece, tString, suitTypeRaces);
      tPiece--;

      if (tPiece >= RACE_HUMAN && tPiece <= RACE_OGRE)
        tRace = race_t(tPiece);
    } else {
      const char* tArg = tString;
      tPiece = 0;

      for (; isspace(*tArg); tArg++)
        ;
      tArg = one_argument(tArg, tBuffer, cElements(tBuffer));  // get limb name.

      bisect_arg(tBuffer, &tPiece, tString, suitPieceNames);
      tPiece--;

      for (; isspace(*tArg); tArg++)
        ;
      tArg = one_argument(tArg, tBuffer,
        cElements(tBuffer));  // get rid of the = in the line.
      for (; isspace(*tArg); tArg++)
        ;
      tArg = one_argument(tArg, tBuffer,
        cElements(tBuffer));  // grab the <vnum> of the item.

      if (tPiece >= 0 && tPiece < LST_MAX && is_number(tBuffer))
        equipment[tPiece] = convertTo<int>(tBuffer);
    }

    if (feof(suitFile)) {
      fclose(suitFile);
      return;
    }
  }

  fclose(suitFile);
}

void loadSetClass::suitAdd(const char* tName, int tHelm, int tCollar,
  int tJacket, int tSleeve, int tGlove, int tBelt, int tBracelet, int tLegging,
  int tBoot, int tRing, int tShield, int tCloak, race_t tRace) {
  loadSetStruct newSuitStruct;

  newSuitStruct.equipment[0] = tHelm;
  newSuitStruct.equipment[1] = tCollar;
  newSuitStruct.equipment[2] = tJacket;
  newSuitStruct.equipment[3] = tSleeve;
  newSuitStruct.equipment[4] = tGlove;
  newSuitStruct.equipment[5] = tBelt;
  newSuitStruct.equipment[6] = tBracelet;
  newSuitStruct.equipment[7] = tLegging;
  newSuitStruct.equipment[8] = tBoot;
  newSuitStruct.equipment[9] = tRing;
  newSuitStruct.equipment[10] = tShield;
  newSuitStruct.equipment[11] = tCloak;

  int suitCount = 0;

  newSuitStruct.name = mud_str_dup(tName);
  newSuitStruct.suitRace = tRace;
  newSuitStruct.suitLevel = 0.0;
  newSuitStruct.suitClassTotal = 0;
  newSuitStruct.suitClassPossible = 0;

  // Set all classes as true, we remove them one by one below.
  for (int classIndex = MIN_CLASS_IND; classIndex < MAX_CLASSES; classIndex++)
    newSuitStruct.suitClassTotal |= (1 << classIndex);

  for (int suitIndex = 0; suitIndex < 12; suitIndex++) {
    int realVNum;

    if (newSuitStruct.equipment[suitIndex] < 0 ||
        ((realVNum = real_object(newSuitStruct.equipment[suitIndex])) >=
          (signed)obj_index.size()) ||
        realVNum < 0)
      continue;

    TObj* tObj = read_object(realVNum, REAL);
    TBaseClothing* tClothing;

    if (!tObj)
      newSuitStruct.equipment[suitIndex] = -1;
    else {
      if ((tClothing = dynamic_cast<TBaseClothing*>(tObj)) &&
          tClothing->armorLevel(ARMOR_LEV_REAL)) {
        newSuitStruct.suitLevel += tClothing->armorLevel(ARMOR_LEV_REAL);
        suitCount++;
      }

      for (int classIndex = MIN_CLASS_IND; classIndex < MAX_CLASSES;
           classIndex++) {
        unsigned short int classValue = (1 << classIndex);
        bool otherClass = false;

        if (classValue == CLASS_MONK && dynamic_cast<TArmor*>(tObj))
          otherClass = true;

        /*
     if (classValue == CLASS_MONK &&
         !tObj->canWear(ITEM_WEAR_FINGERS) &&
         ((tObj->getMaterial() >= 100 && tObj->getMaterial() != MAT_BONE) ||
          tObj->getMaterial() == MAT_IRON ||
          dynamic_cast<TArmor *>(tObj)))
       otherClass = true;
        */

        if (classValue == CLASS_RANGER && tObj->isMetal() &&
            dynamic_cast<TArmor*>(tObj) && !tObj->canWear(ITEM_WEAR_FINGERS))
          otherClass = true;

        if (IsRestricted(GetItemClassRestrictions(tObj), classValue) ||
            otherClass)
          newSuitStruct.suitClassTotal &= ~classValue;
        else {
          newSuitStruct.suitClass[suitIndex] |= classValue;
          newSuitStruct.suitClassPossible |= classValue;
        }
      }

      delete tObj;
      tObj = NULL;
    }
  }

  newSuitStruct.suitLevel /= suitCount;

  for (int suitIndex = suits.size(); suitIndex > 0; suitIndex--)
    suits[suitIndex] = suits[suitIndex - 1];

  suits[0] = newSuitStruct;
}

loadSetStruct& loadSetStruct::operator=(const loadSetStruct& a) {
  if (this == &a)
    return *this;

  delete[] name;
  name = mud_str_dup(a.name);
  suitRace = a.suitRace;
  suitLevel = a.suitLevel;
  suitClassTotal = a.suitClassTotal;
  suitClassPossible = a.suitClassPossible;

  for (int pieceIndex = 0; pieceIndex < LST_MAX; pieceIndex++) {
    equipment[pieceIndex] = a.equipment[pieceIndex];
    suitClass[pieceIndex] = a.suitClass[pieceIndex];
  }

  return *this;
}

loadSetStruct::loadSetStruct() :
  name(NULL),
  suitRace(RACE_NORACE),
  suitLevel(0),
  suitClassTotal(0),
  suitClassPossible(0) {
  memset(equipment, 0, sizeof(equipment));
  memset(suitClass, 0, sizeof(suitClass));
}

loadSetStruct::~loadSetStruct() {
  delete[] name;
  name = NULL;
}

int weightedRandomizer::add(int v, int w, int p) {
  weightedBucket item;
  item.value = v;
  item.weight = w;
  item.percent = p;
  m_items.push_back(item);
  return 1;
}

int weightedRandomizer::getRandomIndex(int weightMax) {
  for (unsigned int c = 0; c < iterMax; c++) {
    int i = ::number(0, m_items.size());
    if (weightMax > 0 && m_items[i].weight > 0 && m_items[i].weight > weightMax)
      continue;
    if (::number(0, 100) > m_items[i].percent)
      continue;
    return i;
  }
  return -1;
}
