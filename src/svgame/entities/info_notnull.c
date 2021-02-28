// LICENSE HERE.

//
// svgame/entities/info_notnull.c
//
//
// info_notnull entity implementation.
//

// Include local game header.
#include "../g_local.h"

//=====================================================
/*QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for lightning.
*/
void SP_info_notnull(edict_t* self)
{
    VectorCopy(self->s.origin, self->absmin);
    VectorCopy(self->s.origin, self->absmax);
}