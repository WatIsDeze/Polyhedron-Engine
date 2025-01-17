// LICENSE HERE.

//
// svgame/entities/func_water.c
//
//
// func_water entity implementation.
//

#include "../../g_local.h"      // SVGame funcs.
#include "../../utils.h"        // Util funcs.
#include "../../brushfuncs.h"   // Brush funcs.

// Include door header.
#include "func_door.h"          // func_door entity.

//=====================================================
/*QUAKED func_water (0 .5 .8) ? START_OPEN
func_water is a moveable water brush.  It must be targeted to operate.  Use a non-water texture at your own risk.

START_OPEN causes the water to move to its destination when spawned and operate in reverse.

"angle"     determines the opening direction (up or down only)
"speed"     movement speed (25 default)
"wait"      wait before returning (-1 default, -1 = TOGGLE)
"lip"       lip remaining at end of move (0 default)
"sounds"    (yes, these need to be changed)
0)  no sound
1)  water
2)  lava
*/

void SP_func_water(Entity* self)
{
    vec3_t  abs_movedir;

    UTIL_SetMoveDir(self->state.angles, self->moveDirection);
    self->moveType = MoveType::Push;
    self->solid = Solid::BSP;
    gi.SetModel(self, self->model);

    switch (self->sounds) {
    default:
        break;

    case 1: // water
        self->moveInfo.startSoundIndex = gi.SoundIndex("world/mov_watr.wav");
        self->moveInfo.endSoundIndex = gi.SoundIndex("world/stp_watr.wav");
        break;

    case 2: // lava
        self->moveInfo.startSoundIndex = gi.SoundIndex("world/mov_watr.wav");
        self->moveInfo.endSoundIndex = gi.SoundIndex("world/stp_watr.wav");
        break;
    }

    // calculate second position
    VectorCopy(self->state.origin, self->position1);
    abs_movedir[0] = fabs(self->moveDirection[0]);
    abs_movedir[1] = fabs(self->moveDirection[1]);
    abs_movedir[2] = fabs(self->moveDirection[2]);
    self->moveInfo.distance = abs_movedir[0] * self->size[0] + abs_movedir[1] * self->size[1] + abs_movedir[2] * self->size[2] - st.lip;
    VectorMA(self->position1, self->moveInfo.distance, self->moveDirection, self->position2);

    // if it starts open, switch the positions
    if (self->spawnFlags & DOOR_START_OPEN) {
        VectorCopy(self->position2, self->state.origin);
        VectorCopy(self->position1, self->position2);
        VectorCopy(self->state.origin, self->position1);
    }

    VectorCopy(self->position1, self->moveInfo.startOrigin);
    VectorCopy(self->state.angles, self->moveInfo.startAngles);
    VectorCopy(self->position2, self->moveInfo.endOrigin);
    VectorCopy(self->state.angles, self->moveInfo.endAngles);

    self->moveInfo.state = STATE_BOTTOM;

    if (!self->speed)
        self->speed = 25;
    self->moveInfo.acceleration = self->moveInfo.deceleration = self->moveInfo.speed = self->speed;

    if (!self->wait)
        self->wait = -1;
    self->moveInfo.wait = self->wait;

    self->Use = door_use;

    if (self->wait == -1)
        self->spawnFlags |= DOOR_TOGGLE;

    self->className = "func_door";

    gi.LinkEntity(self);
}