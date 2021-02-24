// LICENSE HERE.

//
// g_pmai.c
//
//
// Works the Player Move AI system logic. Does the thinking for them ;-)
//
#include "g_local.h"
#include "g_pmai.h"

//
//==========================================================================
//
// UTILITY FUNCTIONS
//
//==========================================================================
//

//
// This is a straight copy from PM_Trace.
// Eventually it needs to be moved into its own file obviously.
//
edict_t* pmai_passent;
trace_t	PMAI_Trace(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end)
{
	if (pmai_passent->health > 0)
		return gi.trace(start, mins, maxs, end, pmai_passent, MASK_PLAYERSOLID);
	else
		return gi.trace(start, mins, maxs, end, pmai_passent, MASK_DEADSOLID);
}

//
//===============
// PMAI_SetSightClient
// 
// Called once each frame to set level.sight_client to one of the clients
// that is in sight.
// 
// If all clients are either dead or in notarget, sight_client
// will be null.
// 
// In coop games, sight_client will cycle between the clients.
//===============
//
void PMAI_SetSightClient(void)
{
	// Stores the actual entity that's found.
	edict_t* ent;

	// Counter variables.
	int	start, index;

	// Start all over if we don't have any client in sight.
	if (level.sight_client == NULL)
		start = 1;
	else
		start = level.sight_client - g_edicts;

	// Start a loop 
	index = start;
	while (1)
	{
		// Increment.
		index++;

		// If we've exceeded maxclients, reset the check counter.
		if (index > game.maxclients)
			index = 1;

		// Fetch the entity.
		ent = &g_edicts[index];

		// Ensure it is in use, has health, and isn't a no target entity or
		// a disguised entity.
		if (ent->inuse
			&& ent->health > 0
			&& !(ent->flags & (FL_NOTARGET | FL_DISGUISED)))
		{
			// If player is using func_monitor, make
			// the sight_client = the fake player at the
			// monitor currently taking the player's place.
			// Do NOT do this for players using a
			// target_monitor, though... in this case
			// both player and fake player are ignored.
			if (ent->client && ent->client->camplayer)
			{
				if (ent->client->spycam)
				{
					level.sight_client = ent->client->camplayer;
					return;
				}
			}
			else
			{
				// We've found an entity, store it.
				level.sight_client = ent;
				return;
			}
		}

		// We've checked each client for this frame. Time to move on since
		// none is in sight.
		if (index == start)
		{
			level.sight_client = NULL;
			return;		// nobody to see
		}
	}
}

//
//===============
// PMAI_EntityRange
// 
// Returns the range(distance in units) between the two entities.
//===============
//
float PMAI_EntityRange(edict_t* self, edict_t* other)
{
	vec3_t	v;
	float	len;

	VectorSubtract(self->s.origin, other->s.origin, v);
	len = VectorLength(v);

	return len;
}

//
//===============
// PMAI_EntityVisible
// 
// Returns true if an entity is visible, even if it is NOT IN FRONT of the
// other entity.
//===============
//
qboolean PMAI_EntityIsVisible(edict_t* self, edict_t* other)
{
	vec3_t	spot1;
	vec3_t	spot2;
	trace_t	trace;

	// Ensure we aren't having NULL pointers here.
	if (!self || !other)
		return false;

	// Calculate the starting vector for tracing. Starting at 'self' its
	// origin + viewheight.
	VectorCopy(self->s.origin, spot1);
	spot1[2] += self->pmai.settings.view.height;

	// Calculate the end vector. Take note that this should trace to the 
	// origin + viewheight of the 'other' AI entity.
	VectorCopy(other->s.origin, spot2);
	spot2[2] += other->pmai.settings.view.height;
	
	// Execute the trace.
	trace = gi.trace(spot1, vec3_origin, vec3_origin, spot2, self, MASK_OPAQUE);

	// Ensure the trace was valid.
	if ((trace.fraction == 1.0) || (trace.ent == other)) {
		return true;
	}

	// If we've reached this point, we return false because no entity was found
	return false;
}

//
//===============
// PMAI_EntityIsInFront
// 
// Returns true in case the other entity is in front of self, and within
// range of the calculated dot product.
//
// Note that it does not mean that the AI can actually see the entity, it
// might be hidden behind a brush etc. Use PMAI_EntityIsVisible to test if
// they can actually see each other.
//===============
//
qboolean PMAI_EntityIsInFront(edict_t* self, edict_t* other, float min_dot)
{
	vec3_t	vec;
	float	dot;
	vec3_t	forward;

	// Ensure we aren't having NULL pointers here.
	if (!self || !other)
		return false;

	// Calculate the forward vector based on the angles of 'self'.
	AngleVectors(self->s.angles, forward, NULL, NULL);

	// Subtract the other origin, from self, and normalize the resulting vec.
	VectorSubtract(other->s.origin, self->s.origin, vec);
	VectorNormalize(vec);

	// Do a dot product on vec and forward to calculate whether it is actually
	// visible or not.
	dot = DotProduct(vec, forward);

	// In case the dot product is higher than the min_dot, it is visible.
	if (dot > min_dot)
		return true;

	// Nothing visible if we've reached this point.
	return false;
}

//
//===============
// PMAI_BrushInFront
//
// Will return true in case a brush has been traced from the given viewheight.
// This can be used to check for jumping, or crouching.
//===============
//
int PMAI_BrushInFront(edict_t* self, float viewheight)
{
	// Vectors.
	vec3_t dir;				// Direction.
	vec3_t forward, right;	// Forward, and right vector.
	vec3_t start, end;		// Start and end point of our traces.
	vec3_t offset;			// The offset to start tracing from, IN that direction.
	vec3_t top;

	// The actual trace results.
	trace_t trace_head;
	trace_t trace_torso;
	trace_t trace_feet;

	// Get current direction
	VectorCopy(self->s.angles, dir);
	AngleVectors(dir, forward, right, NULL);

	//
	// We will calculate the base values for start and end positions to use for
	// tracing.
	// 
	// This will be a scan starting 18 units in front of the player, and with a
	// length of 18 units.
	// Calculate the start vector, with a distance in the direction.
	VectorSet(offset, 18, 0, 0);
	gi.dprintf("------------------------------------------------\n");
	gi.dprintf("origin = %s\n", vtos(self->s.origin));
	G_ProjectSource(self->s.origin, offset, forward, right, start);

//	// Calculate the end vector, with a distance in the direction.
	offset[0] += 18;
	G_ProjectSource(self->s.origin, offset, forward, right, end);
	gi.dprintf("base_start = %s		base_end = %s\n", vtos(start), vtos(end));
//
	//
	// We start off by tracing the head, torso and feet.
	//
	// Trace the feet.
	start[2] += 18;
	end[2] += 18;
	trace_feet = gi.trace(start, self->mins, self->maxs, end, self, MASK_MONSTERSOLID);
	gi.dprintf("trace_feet = start(%s) end(%s)\n", vtos(start), vtos(end));

	// Trace the torso.
	start[2] += 8;
	end[2] += 8;
	trace_torso = gi.trace(start, self->mins, self->maxs, end, self, MASK_MONSTERSOLID);
	gi.dprintf("trace_torso = start(%s) end(%s)\n", vtos(start), vtos(end));

	// Trace the head.
	start[2] += 12;
	end[2] += 12;
	trace_head = gi.trace(start, self->mins, self->maxs, end, self, MASK_MONSTERSOLID);
	gi.dprintf("trace_head = start(%s) end(%s)\n", vtos(start), vtos(end));

	// There is an object in front of our feet, but not in front of our torso.
	// This could insist we jump.
	if (trace_feet.allsolid && !trace_torso.allsolid && !trace_head.allsolid) {
		gi.dprintf("All Solid\n");

		return 1;
	}

	return -1;
//
//	if (tr.allsolid)
//	{
//		//return 1;
//
//		// Check for crouching
//		start[2] -= 14;
//		end[2] -= 14;
//
//		// Set up for crouching check
//		VectorCopy(self->maxs, top);
//		top[2] = 24.0; // crouching height
//		tr = gi.trace(start, self->mins, top, end, self, MASK_PLAYERSOLID);
//
//		// Crouch
//		if (!tr.allsolid)
//		{
////			ucmd->forwardmove = 400;
////			ucmd->upmove = -400;
//			return 1;
//		}
//
//		// Check for high jump
//		start[2] += 16;
//		end[2] += 16;
//		tr = gi.trace(start, self->mins, self->maxs, end, self, MASK_MONSTERSOLID);
//
//		if (!tr.allsolid)
//		{
//			//			ucmd->forwardmove = 400;
//			//			ucmd->upmove = 400;
//			return 2;
//		}
//
//		// Check for high jump
//		start[2] += 16;
//		end[2] += 16;
//		tr = gi.trace(start, self->mins, self->maxs, end, self, MASK_MONSTERSOLID);
//
//		if (!tr.allsolid)
//		{
////			ucmd->forwardmove = 400;
////			ucmd->upmove = 400;
//			return 3;
//		}
//	}
//
//	return 0;
}

//
//==========================================================================
//
// SETTINGS
//
//==========================================================================
//

//
//===============
// PMAI_Initialize
// 
// Initializes an entity with the default AI settings. Call this before
// tweaking them on your own.
//===============
//
void PMAI_Initialize(edict_t* self) {
	// Make sure it's valid.
	if (!self)
		return;

	// Setup the player move parameters.
	PmoveInit(&self->pmai.pmp);

	// Setup view.
	self->pmai.settings.view.height = 25;

	// Setup ranges.
	self->pmai.settings.ranges.melee = 80;
	self->pmai.settings.ranges.hostility = 500;
	
	self->pmai.settings.ranges.max_hearing = 1024;
	self->pmai.settings.ranges.max_sight = 1024;

	self->pmai.settings.ranges.min_dot = 0.3;

	// Setup the pmove trace and point contents function pointers.
	self->pmai.pmove.trace = PMAI_Trace;				// Adds default parms
	self->pmai.pmove.pointcontents = gi.pointcontents;

	// Setup the pmove bounding box.
	VectorSet(self->pmai.pmove.mins, -16, -16, -24);
	VectorSet(self->pmai.pmove.maxs, 16, 16, 32);

	// Setup the pmove state flags.
	self->pmai.pmove.s.pm_flags &= ~PMF_NO_PREDICTION;	// We don't want it to use prediction, there is no client.
	self->pmai.pmove.s.gravity = sv_gravity->value;		// Default gravity.
	self->pmai.pmove.s.pm_type = PM_NORMAL;				// Defualt Player Movement.
	self->pmai.pmove.s.pm_time = 1;						// 1ms = 8 units

	// Copy over the entities origin into the player move for its spawn point.
	VectorCopy(self->s.origin, self->pmai.pmove.s.origin);

}

//
//==========================================================================
//
// TARGET SEEKING ETC.
//
//==========================================================================