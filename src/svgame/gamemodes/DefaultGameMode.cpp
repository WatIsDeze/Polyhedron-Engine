/*
// LICENSE HERE.

//
// DefaultGameMode.cpp
//
//
*/
#include "../g_local.h"          // SVGame.

// Server Game Base Entity.
#include "../entities/base/SVGBaseEntity.h"

// Game Mode.
#include "DefaultGameMode.h"

//
// Constructor/Deconstructor.
//
DefaultGameMode::DefaultGameMode() {

}
DefaultGameMode::~DefaultGameMode() {

}



//
// Interface functions. 
//
//
//===============
// DefaultGameMode::OnSameTeam
//
//===============
//
qboolean DefaultGameMode::OnSameTeam(SVGBaseEntity* ent1, SVGBaseEntity* ent2) {
    char    ent1Team[512];
    char    ent2Team[512];

    if (!((int)(dmflags->value) & (GameModeFlags::ModelTeams | GameModeFlags::SkinTeams)))
        return false;

    //strcpy(ent1Team, ClientTeam(ent1));
    //strcpy(ent2Team, ClientTeam(ent2));

    if (strcmp(ent1Team, ent2Team) == 0)
        return true;
    return false;
}

//
//===============
// DefaultGameMode::OnSameTeam
//
//===============
//
qboolean DefaultGameMode::CanDamage(SVGBaseEntity* targ, SVGBaseEntity* inflictor) {
	
    vec3_t  dest;
    SVGTrace trace;

    // WID: Admer, why the fuck did they rush hour these comments all the time?
    // bmodels need special checking because their origin is 0,0,0 <-- is bad.
    //
    // Solid entities need a special check, as their origin is usually 0,0,0
    // Exception to the above: the solid entity moves or has an origin brush
    if (targ->GetMoveType() == MoveType::Push) {
        // Calculate destination.
        dest = vec3_scale(targ->GetAbsoluteMin() + targ->GetAbsoluteMax(), 0.5f);
        trace = SVG_Trace(inflictor->GetOrigin(), vec3_origin, vec3_origin, dest, inflictor, CONTENTS_MASK_SOLID);
        if (trace.fraction == 1.0)
            return true;
        if (trace.ent == targ)
            return true;
        return false;
    }

    // From here on we start tracing in various directions. Look at the code yourself to figure that one out...
    trace = SVG_Trace(inflictor->GetOrigin(), vec3_origin, vec3_origin, targ->GetOrigin(), inflictor, CONTENTS_MASK_SOLID);
    if (trace.fraction == 1.0)
        return true;

    VectorCopy(targ->GetOrigin(), dest);
    dest[0] += 15.0;
    dest[1] += 15.0;
    trace = SVG_Trace(inflictor->GetOrigin(), vec3_origin, vec3_origin, dest, inflictor, CONTENTS_MASK_SOLID);
    if (trace.fraction == 1.0)
        return true;

    VectorCopy(targ->GetOrigin(), dest);
    dest[0] += 15.0;
    dest[1] -= 15.0;
    trace = SVG_Trace(inflictor->GetOrigin(), vec3_origin, vec3_origin, dest, inflictor, CONTENTS_MASK_SOLID);
    if (trace.fraction == 1.0)
        return true;

    VectorCopy(targ->GetOrigin(), dest);
    dest[0] -= 15.0;
    dest[1] += 15.0;
    trace = SVG_Trace(inflictor->GetOrigin(), vec3_origin, vec3_origin, dest, inflictor, CONTENTS_MASK_SOLID);
    if (trace.fraction == 1.0)
        return true;

    VectorCopy(targ->GetOrigin(), dest);
    dest[0] -= 15.0;
    dest[1] -= 15.0;
    trace = SVG_Trace(inflictor->GetOrigin(), vec3_origin, vec3_origin, dest, inflictor, CONTENTS_MASK_SOLID);
    if (trace.fraction == 1.0)
        return true;

    // If we reached this point... Well, it is false :)
    return false;
}