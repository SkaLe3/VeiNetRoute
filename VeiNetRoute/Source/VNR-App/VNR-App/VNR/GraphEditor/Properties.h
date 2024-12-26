#pragma once
#include "VNR-Core/Core/CoreDefines.h"

namespace VNR
{
	struct NetworkNode;

	DECLARE_DELEGATE_ONE_PARAM(DeleteNodeDelegate, NetworkNode*)

	class Properties
	{
	public:
		Properties();

		void SelectNode(NetworkNode* node);
		void DeselectNode();
		void Draw();

	private:
		void RoutingTableSection();

	public:
		DeleteNodeDelegate OnDeleteNode;

	private:
		NetworkNode* m_SelectedNode;
		bool m_bOpenRoutingHeader = false;
	};
}