// LICENSE HERE.

//
// svgame/entities/func_door.c
//
//
// func_door entity implementation.
//

#include "../../g_local.h"      // Include SVGame funcs.
#include "sharedgame/sharedgame.h"
#include "../../utils.h"        // Include Util funcs.
#include "../../brushfuncs.h"   // Include Brush funcs.
#include "../../effects.h"

#include "func_door.h"          // Include func_door entity header.

//=====================================================
/*QUAKED func_door (0 .5 .8) ? START_OPEN x CRUSHER NOMONSTER ANIMATED TOGGLE ANIMATED_FAST
TOGGLE      wait in both the start and end states for a trigger event.
START_OPEN  the door to moves to its destination when spawned, and operate in reverse.  It is used to temporarily or permanently close off an area when triggered (not useful for touch or takeDamage doors).
NOMONSTER   monsters will not trigger this door

"message"   is printed when the door is touched if it is a trigger door and it hasn't been fired yet
"angle"     determines the opening direction
"targetName" if set, no touch field will be spawned and a remote button or trigger field activates the door.
"health"    if set, door must be shot open
"speed"     movement speed (100 default)
"wait"      wait before returning (3 default, -1 = never return)
"lip"       lip remaining at end of move (8 default)
"damage"       damage to inflict when Blocked (2 default)
"sounds"
1)  silent
2)  light
3)  medium
4)  heavy
*/

void door_use_areaportals(Entity* self, qboolean open)
{
    Entity* t = NULL;

    if (!self->target)
        return;

    while ((t = SVG_Find(t, FOFS(targetName), self->target))) {
        if (Q_stricmp(t->className, "func_areaportal") == 0) {
            gi.SetAreaPortalState(t->style, open);
        }
    }
}

void door_go_down(Entity* self);

void door_hit_top(Entity* self)
{
    if (!(self->flags & EntityFlags::TeamSlave)) {
        if (self->moveInfo.endSoundIndex)
            gi.Sound(self, CHAN_NO_PHS_ADD + CHAN_VOICE, self->moveInfo.endSoundIndex, 1, ATTN_STATIC, 0);
        self->state.sound = 0;
    }
    self->moveInfo.state = STATE_TOP;
    if (self->spawnFlags & DOOR_TOGGLE)
        return;
    if (self->moveInfo.wait >= 0) {
        self->Think = door_go_down;
        self->nextThinkTime = level.time + self->moveInfo.wait;
    }
}

void door_hit_bottom(Entity* self)
{
    if (!(self->flags & EntityFlags::TeamSlave)) {
        if (self->moveInfo.endSoundIndex)
            gi.Sound(self, CHAN_NO_PHS_ADD + CHAN_VOICE, self->moveInfo.endSoundIndex, 1, ATTN_STATIC, 0);
        self->state.sound = 0;
    }
    self->moveInfo.state = STATE_BOTTOM;
    door_use_areaportals(self, false);
}

void door_go_down(Entity* self)
{
    if (!(self->flags & EntityFlags::TeamSlave)) {
        if (self->moveInfo.startSoundIndex)
            gi.Sound(self, CHAN_NO_PHS_ADD + CHAN_VOICE, self->moveInfo.startSoundIndex, 1, ATTN_STATIC, 0);
        self->state.sound = self->moveInfo.middleSoundIndex;
    }
    if (self->maxHealth) {
        self->takeDamage = TakeDamage::Yes;
        self->health = self->maxHealth;
    }

    self->moveInfo.state = STATE_DOWN;
    if (strcmp(self->className, "func_door") == 0)
        Brush_Move_Calc(self, self->moveInfo.startOrigin, door_hit_bottom);
    else if (strcmp(self->className, "func_door_rotating") == 0)
        Brush_AngleMove_Calc(self, door_hit_bottom);
}

void door_go_up(Entity* self, Entity* activator)
{
    if (self->moveInfo.state == STATE_UP)
        return;     // already going up

    if (self->moveInfo.state == STATE_TOP) {
        // reset top wait time
        if (self->moveInfo.wait >= 0)
            self->nextThinkTime = level.time + self->moveInfo.wait;
        return;
    }

    if (!(self->flags & EntityFlags::TeamSlave)) {
        if (self->moveInfo.startSoundIndex)
            gi.Sound(self, CHAN_NO_PHS_ADD + CHAN_VOICE, self->moveInfo.startSoundIndex, 1, ATTN_STATIC, 0);
        self->state.sound = self->moveInfo.middleSoundIndex;
    }
    self->moveInfo.state = STATE_UP;
    if (strcmp(self->className, "func_door") == 0)
        Brush_Move_Calc(self, self->moveInfo.endOrigin, door_hit_top);
    else if (strcmp(self->className, "func_door_rotating") == 0)
        Brush_AngleMove_Calc(self, door_hit_top);

    UTIL_UseTargets(self, activator);
    door_use_areaportals(self, true);
}

void door_use(Entity* self, Entity* other, Entity* activator)
{
    Entity* ent;

    if (self->flags & EntityFlags::TeamSlave)
        return;

    if (self->spawnFlags & DOOR_TOGGLE) {
        if (self->moveInfo.state == STATE_UP || self->moveInfo.state == STATE_TOP) {
            // trigger all paired doors
            for (ent = self; ent; ent = ent->teamChainPtr) {
                ent->message = NULL;
                ent->Touch = NULL;
                door_go_down(ent);
            }
            return;
        }
    }

    // trigger all paired doors
    for (ent = self; ent; ent = ent->teamChainPtr) {
        ent->message = NULL;
        ent->Touch = NULL;
        door_go_up(ent, activator);
    }
}

void Touch_DoorTrigger(Entity* self, Entity* other, cplane_t* plane, csurface_t* surf)
{
    if (other->health <= 0)
        return;

    if (!(other->serverFlags & EntityServerFlags::Monster) && (!other->client))
        return;

    if ((self->owner->spawnFlags & DOOR_NOMONSTER) && (other->serverFlags & EntityServerFlags::Monster))
        return;

    if (level.time < self->debounceTouchTime)
        return;
    self->debounceTouchTime = level.time + 1.0;

    door_use(self->owner, other, other);
}

void Think_CalcMoveSpeed(Entity* self)
{
    Entity* ent;
    float   min;
    float   time;
    float   newspeed;
    float   ratio;
    float   dist;

    if (self->flags & EntityFlags::TeamSlave)
        return;     // only the team master does this

    // find the smallest distance any member of the team will be moving
    min = fabs(self->moveInfo.distance);
    for (ent = self->teamChainPtr; ent; ent = ent->teamChainPtr) {
        dist = fabs(ent->moveInfo.distance);
        if (dist < min)
            min = dist;
    }

    time = min / self->moveInfo.speed;

    // adjust speeds so they will all complete at the same time
    for (ent = self; ent; ent = ent->teamChainPtr) {
        newspeed = fabs(ent->moveInfo.distance) / time;
        ratio = newspeed / ent->moveInfo.speed;
        if (ent->moveInfo.acceleration == ent->moveInfo.speed)
            ent->moveInfo.acceleration = newspeed;
        else
            ent->moveInfo.acceleration *= ratio;
        if (ent->moveInfo.deceleration == ent->moveInfo.speed)
            ent->moveInfo.deceleration = newspeed;
        else
            ent->moveInfo.deceleration *= ratio;
        ent->moveInfo.speed = newspeed;
    }
}

void Think_SpawnDoorTrigger(Entity* ent)
{
    Entity* other;
    vec3_t      mins, maxs;

    if (ent->flags & EntityFlags::TeamSlave)
        return;     // only the team leader spawns a trigger

    VectorCopy(ent->absMin, mins);
    VectorCopy(ent->absMax, maxs);

    for (other = ent->teamChainPtr; other; other = other->teamChainPtr) {
        AddPointToBounds(other->absMin, mins, maxs);
        AddPointToBounds(other->absMax, mins, maxs);
    }

    // expand
    mins[0] -= 60;
    mins[1] -= 60;
    maxs[0] += 60;
    maxs[1] += 60;

    other = SVG_Spawn();
    VectorCopy(mins, other->mins);
    VectorCopy(maxs, other->maxs);
    other->owner = ent;
    other->solid = Solid::Trigger;
    other->moveType = MoveType::None;
    other->Touch = Touch_DoorTrigger;
    gi.LinkEntity(other);

    if (ent->spawnFlags & DOOR_START_OPEN)
        door_use_areaportals(ent, true);

    Think_CalcMoveSpeed(ent);
}

void door_blocked(Entity* self, Entity* other)
{
    Entity* ent;

    if (!(other->serverFlags & EntityServerFlags::Monster) && (!other->client)) {
        // give it a chance to go away on it's own terms (like gibs)
        SVG_InflictDamage(other, self, self, vec3_zero(), other->state.origin, vec3_zero(), 100000, 1, 0, MeansOfDeath::Crush);
        // if it's still there, nuke it
        if (other)
            SVG_BecomeExplosion1(other);
        return;
    }

    SVG_InflictDamage(other, self, self, vec3_zero(), other->state.origin, vec3_zero(), self->damage, 1, 0, MeansOfDeath::Crush);

    if (self->spawnFlags & DOOR_CRUSHER)
        return;


    // if a door has a negative wait, it would never come back if Blocked,
    // so let it just squash the object to death real fast
    if (self->moveInfo.wait >= 0) {
        if (self->moveInfo.state == STATE_DOWN) {
            for (ent = self->teamMasterPtr; ent; ent = ent->teamChainPtr)
                door_go_up(ent, ent->activator);
        }
        else {
            for (ent = self->teamMasterPtr; ent; ent = ent->teamChainPtr)
                door_go_down(ent);
        }
    }
}

void door_killed(Entity* self, Entity* inflictor, Entity* attacker, int damage, const vec3_t &point)
{
    Entity* ent;

    for (ent = self->teamMasterPtr; ent; ent = ent->teamChainPtr) {
        ent->health = ent->maxHealth;
        ent->takeDamage = TakeDamage::No;
    }
    door_use(self->teamMasterPtr, attacker, attacker);
}

void door_touch(Entity* self, Entity* other, cplane_t* plane, csurface_t* surf)
{
    if (!other->client)
        return;

    if (level.time < self->debounceTouchTime)
        return;
    self->debounceTouchTime = level.time + 5.0;

    gi.CenterPrintf(other, "%s", self->message);
    gi.Sound(other, CHAN_AUTO, gi.SoundIndex("misc/talk1.wav"), 1, ATTN_NORM, 0);
}

void SP_func_door(Entity* ent)
{
    vec3_t  abs_movedir;

    if (ent->sounds != 1) {
        ent->moveInfo.startSoundIndex = gi.SoundIndex("doors/dr1_strt.wav");
        ent->moveInfo.middleSoundIndex = gi.SoundIndex("doors/dr1_mid.wav");
        ent->moveInfo.endSoundIndex = gi.SoundIndex("doors/dr1_end.wav");
    }

    UTIL_SetMoveDir(ent->state.angles, ent->moveDirection);
    ent->moveType = MoveType::Push;
    ent->solid = Solid::BSP;
    gi.SetModel(ent, ent->model);

    ent->Blocked = door_blocked;
    ent->Use = door_use;

    if (!ent->speed)
        ent->speed = 100;
    if (deathmatch->value)
        ent->speed *= 2;

    if (!ent->acceleration)
        ent->acceleration = ent->speed;
    if (!ent->deceleration)
        ent->deceleration = ent->speed;

    if (!ent->wait)
        ent->wait = 3;
    if (!st.lip)
        st.lip = 8;
    if (!ent->damage)
        ent->damage = 2;

    // calculate second position
    VectorCopy(ent->state.origin, ent->position1);
    abs_movedir[0] = fabs(ent->moveDirection[0]);
    abs_movedir[1] = fabs(ent->moveDirection[1]);
    abs_movedir[2] = fabs(ent->moveDirection[2]);
    ent->moveInfo.distance = abs_movedir[0] * ent->size[0] + abs_movedir[1] * ent->size[1] + abs_movedir[2] * ent->size[2] - st.lip;
    VectorMA(ent->position1, ent->moveInfo.distance, ent->moveDirection, ent->position2);

    // if it starts open, switch the positions
    if (ent->spawnFlags & DOOR_START_OPEN) {
        VectorCopy(ent->position2, ent->state.origin);
        VectorCopy(ent->position1, ent->position2);
        VectorCopy(ent->state.origin, ent->position1);
    }

    ent->moveInfo.state = STATE_BOTTOM;

    if (ent->health) {
        ent->takeDamage = TakeDamage::Yes;
        ent->Die = door_killed;
        ent->maxHealth = ent->health;
    }
    else if (ent->targetName && ent->message) {
        gi.SoundIndex("misc/talk.wav");
        ent->Touch = door_touch;
    }

    ent->moveInfo.speed = ent->speed;
    ent->moveInfo.acceleration = ent->acceleration;
    ent->moveInfo.deceleration = ent->deceleration;
    ent->moveInfo.wait = ent->wait;
    VectorCopy(ent->position1, ent->moveInfo.startOrigin);
    VectorCopy(ent->state.angles, ent->moveInfo.startAngles);
    VectorCopy(ent->position2, ent->moveInfo.endOrigin);
    VectorCopy(ent->state.angles, ent->moveInfo.endAngles);

    if (ent->spawnFlags & 16)
        ent->state.effects |= EntityEffectType::AnimCycleAll2hz;
    if (ent->spawnFlags & 64)
        ent->state.effects |= EntityEffectType::AnimCycleAll30hz;

    // to simplify logic elsewhere, make non-teamed doors into a team of one
    if (!ent->team)
        ent->teamMasterPtr = ent;

    gi.LinkEntity(ent);

    ent->nextThinkTime = level.time + FRAMETIME;
    if (ent->health || ent->targetName)
        ent->Think = Think_CalcMoveSpeed;
    else
        ent->Think = Think_SpawnDoorTrigger;
}