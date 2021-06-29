/*
// LICENSE HERE.

// FuncButton.cpp
*/

#include "../../g_local.h"
#include "../../effects.h"
#include "../../entities.h"
#include "../../utils.h"
#include "../../physics/stepmove.h"
#include "../../brushfuncs.h"

#include "../base/SVGBaseEntity.h"
#include "../base/SVGBaseTrigger.h"
#include "../base/SVGBaseMover.h"

#include "FuncButton.h"

FuncButton::FuncButton( Entity* svEntity )
	: SVGBaseMover( svEntity )
{

}

void FuncButton::Precache()
{
	SVGBaseMover::Precache();
}

void FuncButton::Spawn()
{
	SVGBaseMover::Spawn();

	vec3_t absoluteMovedir;
	float distance;

	vec3_t angles = GetAngles();
	UTIL_SetMoveDir( angles, serverEntity->moveDirection );
	
	// mappers set angles to determine the movement direction of the button,
	// so we gotta set the entity's angles to zero
	SetAngles( vec3_zero() );

	SetModel( GetModel() );
	SetMoveType( MoveType::Stop );
	SetSolid( Solid::BSP );

	// If the mapper didn't specify a sound
	if ( GetSound() != 1 )
	{
		serverEntity->moveInfo.startSoundIndex = gi.SoundIndex( "switches/butn2.wav" );
	}
	// If the mapper didn't specify speed, set it to 40 u/s
	if ( !serverEntity->speed )
	{
		serverEntity->speed = 40.0f;
	}
	// If the mapper didn't specify acceleration & deceleration, set the default
	if ( !serverEntity->acceleration )
	{
		serverEntity->acceleration = serverEntity->speed;
	}
	if ( !serverEntity->deceleration )
	{
		serverEntity->deceleration = serverEntity->speed;
	}

	if ( !waitTime )
	{
		SetWaitTime( 3.0f );
	}

	// Lip: how much to subtract from the door's travel distance
	if ( !lip )
	{
		lip = 4.0f;
	}

	// Set up the trajectory
	serverEntity->position1 = GetOrigin();

	absoluteMovedir.x = fabsf( serverEntity->moveDirection.x );
	absoluteMovedir.y = fabsf( serverEntity->moveDirection.y );
	absoluteMovedir.z = fabsf( serverEntity->moveDirection.z );
	distance = (absoluteMovedir.x * serverEntity->size.x) + (absoluteMovedir.y * serverEntity->size.y) + (absoluteMovedir.z * serverEntity->size.z) - lip;
	
	serverEntity->position2 = vec3_fmaf( serverEntity->position1, distance, serverEntity->moveDirection );

	SetEffects( EntityEffectType::AnimCycleFrames01hz2 );

	if ( GetHealth() )
	{
		SetMaxHealth( GetHealth() );
		SetDieCallback( &FuncButton::ButtonDie );
		SetTakeDamage( TakeDamage::Yes );
	}
	else if ( nullptr == serverEntity->targetName )
	{
		SetTouchCallback( &FuncButton::ButtonTouch );
	}

	SetUseCallback( &FuncButton::ButtonUse );

	// Set up moveInfo stuff
	// Button starts off
	serverEntity->moveInfo.state = MoverState::Bottom;

	serverEntity->moveInfo.speed = serverEntity->speed;
	serverEntity->moveInfo.acceleration = serverEntity->acceleration;
	serverEntity->moveInfo.deceleration = serverEntity->deceleration;
	serverEntity->moveInfo.wait = waitTime;

	serverEntity->moveInfo.startOrigin = serverEntity->position1;
	serverEntity->moveInfo.startAngles = GetAngles();
	serverEntity->moveInfo.endOrigin = serverEntity->position2;
	serverEntity->moveInfo.endAngles = GetAngles();

	LinkEntity();
}

void FuncButton::SpawnKey( const std::string& key, const std::string& value )
{
	// I think serverEntity variables should just be set in SVGBaseEntity::SpawnKey
	// It doesn't make sense to set them only here, if these variables are available to every entity
	if ( key == "speed" )
	{
		ParseFloatKeyValue( key, value, serverEntity->speed );
	}
	else if ( key == "lip" )
	{
		ParseFloatKeyValue( key, value, lip );
	}
	else
	{
		return SVGBaseMover::SpawnKey( key, value );
	}
}

void FuncButton::OnButtonDone( Entity* self )
{
	FuncButton* button = static_cast<FuncButton*>( self->classEntity );
	button->ButtonDone();
}

void FuncButton::ButtonDone()
{
	serverEntity->moveInfo.state = MoverState::Bottom;
	serverEntity->state.effects &= ~(EntityEffectType::AnimCycleFrames23hz2);
	serverEntity->state.effects |= EntityEffectType::AnimCycleFrames01hz2;
}

void FuncButton::ButtonReturn()
{
	serverEntity->moveInfo.state = MoverState::Down;
	BrushMoveCalc( serverEntity->moveInfo.startOrigin, OnButtonDone );
	SetFrame( 0 );
	
	if ( GetHealth() )
	{
		SetTakeDamage( TakeDamage::Yes );
	}
}

void FuncButton::OnButtonWait( Entity* self )
{
	FuncButton* button = static_cast<FuncButton*>(self->classEntity);
	button->ButtonWait();
}

void FuncButton::ButtonWait()
{
	serverEntity->moveInfo.state = MoverState::Top;
	serverEntity->state.effects &= ~(EntityEffectType::AnimCycleFrames01hz2);
	serverEntity->state.effects |= EntityEffectType::AnimCycleFrames23hz2;
	SetFrame( 1 );

	UseTargets(GetActivator());

	if ( serverEntity->moveInfo.wait >= 0.0f )
	{
		SetNextThinkTime( level.time + serverEntity->moveInfo.wait );
		SetThinkCallback( &FuncButton::ButtonReturn );
	}
}

void FuncButton::ButtonFire()
{
	if ( serverEntity->moveInfo.state == MoverState::Up || serverEntity->moveInfo.state == MoverState::Top )
	{
		return;
	}

	serverEntity->moveInfo.state = MoverState::Up;
	if ( serverEntity->moveInfo.startSoundIndex && !(flags & EntityFlags::TeamSlave) )
	{
		gi.Sound( serverEntity, CHAN_NO_PHS_ADD + CHAN_VOICE, serverEntity->moveInfo.startSoundIndex, 1, ATTN_STATIC, 0 );
	}
	
	BrushMoveCalc( serverEntity->moveInfo.endOrigin, OnButtonWait );
}

void FuncButton::ButtonUse( SVGBaseEntity* other, SVGBaseEntity* activator )
{
	this->activator = activator;
	ButtonFire();
}

void FuncButton::ButtonTouch( SVGBaseEntity* self, SVGBaseEntity* other, cplane_t* plane, csurface_t* surf )
{
	if ( !other->GetClient() || other->GetHealth() <= 0 )
	{
		return;
	}

	activator = other;
	ButtonFire();
}

void FuncButton::ButtonDie( SVGBaseEntity* inflictor, SVGBaseEntity* attacker, int damage, const vec3_t& point )
{
	activator = attacker;
	SetHealth( GetMaxHealth() );
	SetTakeDamage( TakeDamage::No );
}

// =========================
// Brush move methods
// 
// Will be moved to either SVGBaseEntity or 
// some sorta SVGBaseMover when we make one
// =========================
void FuncButton::BrushMoveDone()
{
	SetVelocity( vec3_zero() );
	serverEntity->moveInfo.OnEndFunction( serverEntity );
}

void FuncButton::BrushMoveFinal()
{
	// We've traveled our world, time to rest
	if ( serverEntity->moveInfo.remainingDistance == 0.0f )
	{
		BrushMoveDone();
		return;
	}

	// Move only as far as to clear the remaining distance
	SetVelocity( vec3_scale( serverEntity->moveInfo.dir, serverEntity->moveInfo.remainingDistance / FRAMETIME ) );

	SetThinkCallback( &FuncButton::BrushMoveDone );
	SetNextThinkTime( level.time + FRAMETIME );
}

void FuncButton::BrushMoveBegin()
{
	float frames;

	// It's time to stop
	if ( (serverEntity->moveInfo.speed * FRAMETIME) >= serverEntity->moveInfo.remainingDistance )
	{
		BrushMoveFinal();
		return;
	}

	SetVelocity( vec3_scale( serverEntity->moveInfo.dir, serverEntity->moveInfo.speed ) );

	frames = floor( (serverEntity->moveInfo.remainingDistance / serverEntity->moveInfo.speed) / FRAMETIME );
	serverEntity->moveInfo.remainingDistance -= frames * serverEntity->moveInfo.speed * FRAMETIME;

	SetThinkCallback( &FuncButton::BrushMoveFinal );
	SetNextThinkTime( level.time + (frames * FRAMETIME) );
}

void FuncButton::BrushMoveCalc( const vec3_t& destination, PushMoveEndFunction* function )
{
	PushMoveInfo& mi = serverEntity->moveInfo;

	SetVelocity( vec3_zero() );
	mi.dir = destination - GetOrigin();
	mi.remainingDistance = VectorNormalize( serverEntity->moveInfo.dir );
	mi.OnEndFunction = function;

	if ( mi.speed == mi.acceleration && mi.speed == mi.deceleration )
	{
		if ( level.currentEntity == ((GetFlags() & EntityFlags::TeamSlave) ? GetTeamMasterEntity() : this) )
		{
			BrushMoveBegin();
		}
		else
		{
			SetThinkCallback( &FuncButton::BrushMoveBegin );
			SetNextThinkTime( level.time + FRAMETIME );
		}
	}
	else
	{
		// accelerative
		mi.currentSpeed = 0;

		// Implement Think_AccelMove first
		//SetThinkCallback( &FuncButton::BrushAccelMove );
		SetNextThinkTime( level.time + FRAMETIME );
	}
}
