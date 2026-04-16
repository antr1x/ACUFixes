#pragma once

#include "basic_types.h"
#include "SmallArray.h"

class AtomCondition;

class AtomConditionExpression
{
public:
	// @members
	uint64 ExpressionID; //0x0000
	SmallArray<uint32> referencedGraphVarsIndices; //0x0008
	SmallArray<AtomCondition> Conditions; //0x0014
	char pad_0020[8]; //0x0020

	// @helper_functions
	// This probably deserves another look later, but for now it seems to follow the same +0x2010 shift as the rest of this Atom graph TypeInfo block.
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439DA5C0; }
}; //Size: 0x0028
assert_sizeof(AtomConditionExpression, 0x28);
