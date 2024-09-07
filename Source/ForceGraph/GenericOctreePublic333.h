// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	GenericOctreePublic.h: Generic octree definition.
=============================================================================*/

#pragma once

#include "CoreMinimal.h"

/** 
 *	An identifier for an element in the octree. 
 */
class FOctreeElementId3
{
public:
	template <typename, typename>
	friend class TOctree3;

	/** Default constructor. */
	FOctreeElementId3()
		: Node(NULL)
		  , ElementIndex(INDEX_NONE)
	{
	}

	/** @return a boolean value representing whether the id is NULL. */
	bool IsValidId() const
	{
		return Node != NULL;
	}

private:
	/** The node the element is in. */
	const void* Node;

	/** The index of the element in the node's element array. */
	int32 ElementIndex;

	/** Initialization constructor. */
	FOctreeElementId3(const void* InNode, int32 InElementIndex)
		: Node(InNode)
		  , ElementIndex(InElementIndex)
	{
	}

	/** Implicit conversion to the element index. */
	operator int32() const
	{
		return ElementIndex;
	}
};
