#include "g_local.h"

extern void DeathmatchScoreboard (edict_t *ent);


// snap - new chasecam mode...
void UpdateChaseCam(edict_t *ent)
{
	static vec3_t	mins = {-10, -10, -10}, maxs = {10, 10, 10};
	vec3_t o, ownerv, goal;
	edict_t *targ;
	vec3_t forward, right, up;
	trace_t trace;
	int i;
	vec3_t angles;

	// is our chase target gone?
	if (!ent->client->chase_target->inuse) {
		ent->client->chase_target = NULL;
		ent->client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
		return;
	}

	targ = ent->client->chase_target;

	VectorCopy(targ->s.origin, ownerv);

	ownerv[2] += targ->viewheight;

	// normal locked chase mode...
	if(ent->client->chasemode == LOCKED_CHASE){  
		VectorCopy(targ->client->v_angle, angles);
		if (angles[PITCH] > 56)
			angles[PITCH] = 56;
		AngleVectors (angles, forward, right, up);
		VectorNormalize(forward);
		VectorMA(ownerv, -150, forward, o);
		VectorMA(o, 24, up, o);

		if (o[2] < targ->s.origin[2] + 20)
			o[2] = targ->s.origin[2] + 20;

		// jump animation lifts
		if (!targ->groundentity)
			o[2] += 16;

		trace = gi.trace(ownerv, mins, maxs, o, targ, MASK_SOLID);

		VectorCopy(trace.endpos, goal);

		ent->client->ps.pmove.pm_type = PM_FREEZE;

		VectorCopy(goal, ent->s.origin);
		for (i=0 ; i<3 ; i++)
			ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(targ->client->v_angle[i] - ent->client->resp.cmd_angles[i]);

		VectorCopy(targ->client->v_angle, ent->client->ps.viewangles);
		VectorCopy(targ->client->v_angle, ent->client->v_angle);
		// Ridah, angle down a bit since we moved it up
		ent->client->v_angle[2] += 35;
	}

	// free to spin around the target...
	else if(ent->client->chasemode == FREE_CHASE){
		VectorCopy(ent->client->v_angle, angles);
        AngleVectors (angles, forward, right, NULL);
        VectorNormalize(forward);
        VectorMA(ownerv, -150, forward, o);

        // jump animation lifts
        if (!targ->groundentity)
	        o[2] += 16;

        trace = gi.trace(ownerv, mins, maxs, o, targ, MASK_SOLID);        

        VectorCopy(trace.endpos, goal);
		VectorCopy(goal, ent->s.origin);

        ent->client->ps.pmove.pm_type = PM_FREEZE;              
 
        for (i=0 ; i<3 ; i++)
		    ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(ent->client->v_angle[i] - ent->client->resp.cmd_angles[i]);

	}

	ent->viewheight = 0;
	ent->client->ps.pmove.pm_flags |= PMF_NO_PREDICTION;
	gi.linkentity(ent);
}


void ChaseNext(edict_t *ent)
{
	int i;
	edict_t *e;

	if (!ent->client->chase_target)
	{
		// select the first entity, then go from there
		ent->client->chase_target = &g_edicts[1];
	}

	i = ent->client->chase_target - g_edicts;
	do {
		i++;
		if (i > maxclients->value)
			i = 1;
		e = g_edicts + i;
		if (!e->inuse)
			continue;
//		if (!e->playing_ingame)
//			continue;
		if (e->solid != SOLID_NOT)
			break;
	} while (e != ent->client->chase_target);

	if (e == ent || !e->inuse || e->solid==SOLID_NOT)
	{
		ent->client->chase_target = NULL;
		ent->client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
		return;
	}

	ent->client->chase_target = e;
	ent->client->update_chase = true;
	
	DeathmatchScoreboard(ent);
}

void ChasePrev(edict_t *ent)
{
	int i;
	edict_t *e;

	if (!ent->client->chase_target)
	{
		// select the first entity, then go from there
		ent->client->chase_target = &g_edicts[1];
	}

	i = ent->client->chase_target - g_edicts;
	do {
		i--;
		if (i < 1)
			i = maxclients->value;
		e = g_edicts + i;
		if (!e->inuse) 
			continue;
//		if (!e->playing_ingame)
//			continue;
		if (e->solid != SOLID_NOT)
			break;
	} while (e != ent->client->chase_target);

	if (e == ent || !e->inuse || e->solid==SOLID_NOT)
	{
		ent->client->chase_target = NULL;
		ent->client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
		return;
	}

	ent->client->chase_target = e;
	ent->client->update_chase = true;
	
	DeathmatchScoreboard(ent);
}
