#pragma once
#include "VNR-Core/Core/CoreDefines.h"

namespace VNR
{
	enum class ENetworkProtocol : uint8
	{
		UDP = 0,
		TCP
	};

	ENUM_CLASS_CONVERSIONS(ENetworkProtocol)
}