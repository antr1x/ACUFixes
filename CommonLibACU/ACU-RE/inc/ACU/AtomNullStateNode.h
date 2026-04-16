#pragma once

#include "AtomStateNode.h"

class AtomNullStateNode : public AtomStateNode
{
public:
	// @members

	// @helper_functions
	// This probably deserves another look later, but for now it seems to follow the same +0x2010 shift as the rest of this Atom graph TypeInfo block.
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439DFA00; }
}; //Size: 0x0080
assert_sizeof(AtomNullStateNode, 0x80);
