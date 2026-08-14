#pragma once

/*
==================
CInputAction::CInputAction
==================
*/
template<uint32 maxKeys /* = 2 */>
FORCEINLINE CInputAction<maxKeys>::CInputAction()
	: state( BUTTON_STATE_FLAG_NONE )
{
	for ( uint32 index = 0; index < maxKeys; ++index )
	{
		keys[index] = BUTTON_CODE_NONE;
	}
}

/*
==================
CInputAction::Press
==================
*/
template<uint32 maxKeys>
FORCEINLINE void CInputAction<maxKeys>::Press( buttonCode_t key )
{
	// If the press isn't keyless, remember the key
	if ( key != BUTTON_CODE_NONE )
	{
		// Do nothing if the key already holds it (auto-repeat)
		if ( GetKeySlot( key ) != INVALID_INDEX )
		{
			return;
		}

		// Get free slot and do nothing if all slots are busy
		uint32 freeSlot = GetKeySlot( BUTTON_CODE_NONE );
		if ( freeSlot == INVALID_INDEX )
		{
			return;
		}
		keys[freeSlot] = key;
	}

	// Set flags if this is the first keypress
	if ( !IsDown() )
	{
		state |= BUTTON_STATE_FLAG_DOWN | BUTTON_STATE_FLAG_PRESSED;
	}
}

/*
==================
CInputAction::Press
==================
*/
template<uint32 maxKeys /* = 2 */>
FORCEINLINE void CInputAction<maxKeys>::Release( buttonCode_t key )
{
	// If the release isn't keyless, remove the key
	if ( key != BUTTON_CODE_NONE )
	{
		// Remove the key if it was holding the action
		uint32 slot = GetKeySlot( key );
		if ( slot == INVALID_INDEX )
		{
			return;
		}
		keys[slot] = BUTTON_CODE_NONE;
	}

	// Do nothing while a key holds it
	for ( uint32 index = 0; index < maxKeys; ++index )
	{
		if ( keys[index] != BUTTON_CODE_NONE )
		{
			return;
		}
	}

	// Set flags
	state = ( state & ~BUTTON_STATE_FLAG_DOWN ) | BUTTON_STATE_FLAG_RELEASED;
}

/*
==================
CInputAction::ClearEdges
==================
*/
template<uint32 maxKeys /* = 2 */>
FORCEINLINE void CInputAction<maxKeys>::ClearEdges()
{
	// Drop the edges, keep the down bit
	state &= BUTTON_STATE_FLAG_DOWN;
}

/*
==================
CInputAction::IsDown
==================
*/
template<uint32 maxKeys /* = 2 */>
FORCEINLINE bool CInputAction<maxKeys>::IsDown() const
{
	return ( state & BUTTON_STATE_FLAG_DOWN ) != 0;
}

/*
==================
CInputAction::WasPressed
==================
*/
template<uint32 maxKeys /* = 2 */>
FORCEINLINE bool CInputAction<maxKeys>::WasPressed() const
{
	return ( state & BUTTON_STATE_FLAG_PRESSED ) != 0;
}

/*
==================
CInputAction::WasReleased
==================
*/
template<uint32 maxKeys /* = 2 */>
FORCEINLINE bool CInputAction<maxKeys>::WasReleased() const
{
	return ( state & BUTTON_STATE_FLAG_RELEASED ) != 0;
}

/*
==================
CInputAction::GetState

Fraction of the frame the action was held, in [0,1].
A mid-frame press/release is estimated, since exact
sub-frame timing isn't tracked:
  1.00	held the entire frame
  0.75	released and re-pressed within the frame (held most of it)
  0.50	pressed mid-frame and still held (held about half)
  0.25	pressed and released within the frame (a quick tap)
  0.00	not held (up all frame, or released mid-frame)
==================
*/
template<uint32 maxKeys /* = 2 */>
FORCEINLINE float CInputAction<maxKeys>::GetState() const
{
	bool bPressed  = WasPressed();
	bool bReleased = WasReleased();
	bool bDown	   = IsDown();
	if ( !bPressed && !bReleased )
	{
		return bDown ? 1.f : 0.f;
	}
	if ( bPressed && bReleased )
	{
		return bDown ? 0.75f : 0.25f;
	}
	return bDown ? 0.5f : 0.f;
}

/*
==================
CInputAction::GetKeySlot
==================
*/
template<uint32 maxKeys /* = 2 */>
FORCEINLINE uint32 CInputAction<maxKeys>::GetKeySlot( buttonCode_t key ) const
{
	for ( uint32 index = 0; index < maxKeys; ++index )
	{
		if ( keys[index] == key )
		{
			return index;
		}
	}
	return INVALID_INDEX;
}
