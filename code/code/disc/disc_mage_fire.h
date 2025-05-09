#pragma once

#include "discipline.h"
#include "skills.h"

class CDFire : public CDiscipline {
  public:
    CSkill skLavaLance;
    CSkill skInferno;
    CSkill skHellFire;
    CSkill skFlamingFlesh;
    CSkill skProtectionFromFire;

    virtual CDFire* cloneMe() { return new CDFire(*this); }

  private:
};

int castHandsOfFlame(TBeing*, TBeing*);
int handsOfFlame(TBeing*, TBeing*);
int handsOfFlame(TBeing*, TBeing*, TMagicItem*);
int handsOfFlame(TBeing*, TBeing*, int, short, int*, int);

void faerieFire(TBeing*, TBeing*);
void faerieFire(TBeing*, TBeing*, TMagicItem*);
int castFaerieFire(TBeing*, TBeing*);
int faerieFire(TBeing*, TBeing*, int, short);

int castFlamingSword(TBeing*, TBeing*);
int flamingSword(TBeing*, TBeing*);
int flamingSword(TBeing*, TBeing*, TMagicItem*);
int flamingSword(TBeing*, TBeing*, int, short, int);

int castLavaLance(TBeing*, TBeing*);
int lavaLance(TBeing*, TBeing*);
int lavaLance(TBeing*, TBeing*, int, short, int);
int lavaLance(TBeing*, TBeing*, TMagicItem*);

int castInferno(TBeing*, TBeing*);
int inferno(TBeing*, TBeing*);
int inferno(TBeing*, TBeing*, int, short, int);
int inferno(TBeing*, TBeing*, TMagicItem*);

int castHellfire(TBeing*);
int hellfire(TBeing*);
int hellfire(TBeing*, TMagicItem*);
int hellfire(TBeing*, int, short, int);

int castFireball(TBeing*);
int fireball(TBeing*);
int fireball(TBeing*, TMagicItem*);
int fireball(TBeing*, int, short, int);

int flamingFlesh(TBeing*, TBeing*);
void flamingFlesh(TBeing*, TBeing*, TMagicItem*);
int castFlamingFlesh(TBeing*, TBeing*);
int flamingFlesh(TBeing*, TBeing*, int, short);

int castConjureElemFire(TBeing*);
int conjureElemFire(TBeing*);
int conjureElemFire(TBeing*, int, short);

int castFlare(TBeing*);
int flare(TBeing*);
int flare(TBeing*, TMagicItem*);
int flare(TBeing*, int, short);

void infravision(TBeing*, TBeing*);
void infravision(TBeing*, TBeing*, TMagicItem*);
int infravision(TBeing*, TBeing*, int, short);
int castInfravision(TBeing*, TBeing*);

int protectionFromFire(TBeing*);
void protectionFromFire(TBeing*, TMagicItem*);
int protectionFromFire(TBeing*, int, short);
