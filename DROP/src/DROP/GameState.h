#pragma once

// can we do a struct without names and only using types?
// We could, given a type, get the type Id and then use it inside a map
// and return the pointer, but can be kinda slow, compared to a specified type
struct SingletonComponents
{

};

// Holding Game data for an easier access, not because i am lazy
// use it for singleton components, like in the Ovewatch ECS.
struct GameState
{
	SingletonComponents* singletonComponents = nullptr;

};