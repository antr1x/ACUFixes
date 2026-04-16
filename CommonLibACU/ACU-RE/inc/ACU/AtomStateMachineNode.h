#pragma once

#include "AtomStateNode.h"
#include "AtomNodeID.h"

class AtomConditionExpression;

class AtomInitialState
{
public:
	// @members
	AtomConditionExpression* conditionExpression; //0x0000
	uint16 TargetStateIndex; //0x0008
	AtomNodeID nodeID; //0x000A
	char pad_001A[6]; //0x001A

	// @helper_functions
	// This probably deserves another look later, but for now it seems to follow the same +0x2010 shift as the rest of this Atom graph TypeInfo block.
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439DF740; }
}; //Size: 0x0020
assert_sizeof(AtomInitialState, 0x20);

class AtomStateMachineNode : public AtomStateNode
{
public:
	// @members
	SmallArray<AtomStateNode*> States; //0x0080
	SmallArray<AtomInitialState> InitialStates; //0x008C
	SmallArray<AtomStateTransitionTarget*> GlobalTransitions; //0x0098
	char pad_00A4[4]; //0x00A4

	// @helper_functions
	// This probably deserves another look later, but for now it seems to follow the same +0x2010 shift as the rest of this Atom graph TypeInfo block.
	static TypeInfo& GetTI() { return *(TypeInfo*)0x1439DF8A0; }
}; //Size: 0x00A8
assert_sizeof(AtomStateMachineNode, 0xA8);
