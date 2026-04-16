#pragma once

#include "AtomAnimationDataBaseNode.h"

class AtomAnimationDataNode : public AtomAnimationDataBaseNode
{
public:
	// @members

	// @helper_functions
	// This probably deserves another look later, but for now it seems to follow the same +0x2010 shift as the rest of this Atom graph TypeInfo block.
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439D9670; }
}; //Size: 0x0070
assert_sizeof(AtomAnimationDataNode, 0x70);
