#include "Simulation.h"

#include <imgui.h>
#include <glm/glm.hpp>

#include <random>
#include <cstdio>


namespace VNR
{

	Simulation::Simulation()
		: m_Nodes(nullptr), m_Channels(nullptr)
	{

	}

	void Simulation::ProvideNetworkData(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels)
	{
		m_Nodes = &nodes;
		m_Channels = &channels;
	}

	void Simulation::Simulate(PacketTransmissionSettings settings)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		if (settings.bRandomMessageCount)
		{
			std::uniform_int_distribution<> dis(1, 10);
			settings.MessageCount = dis(gen);
		}

		if (settings.bRandomMTU)
		{

			std::uniform_int_distribution<> dis(500, 9000);
			settings.MTU = dis(gen);
		}

		for (size_t i = 0; i < settings.MessageCount; i++)
		{
			PacketTransmissionSpecifications specs(settings, m_Nodes);
			if (specs.SourceNode == specs.DestinationNode)
				continue;

			switch (specs.Protocol)
			{
			case ENetworkProtocol::TCP:
				specs.ServiceSize = 60;
				break;
			case ENetworkProtocol::UDP:
				specs.ServiceSize = 28;
				break;
			}
			specs.PayloadSize = specs.MTU - specs.ServiceSize;

			bool status = SendMessage(specs);
			if (!status)
			{
				PacketTransmissionResult result;
				result.Protocol = specs.Protocol;
				result.Success = false;
				m_Result.push_back(result);
				VNR_WARN("TCP Connection lost");
			}

		}

	}

	bool Simulation::SendMessage(PacketTransmissionSpecifications& specs)
	{
		PacketTransmissionResult res;
		res.Protocol = specs.Protocol;
		res.SourceNode = specs.SourceNode;
		res.DestinationNode = specs.DestinationNode;
		res.MessageSize = specs.MessageSize;
		res.ServiceSize = specs.ServiceSize;


	

		double time = 0;
		bool status = false;
		int32 retransmissionTryLimit = 15;
		int32 retransmitTries = 0;
		if (specs.Protocol == ENetworkProtocol::TCP)
		{
			Packet packetPrototype = { ENetworkProtocol::TCP, specs.SourceNode, specs.DestinationNode, specs.ServiceSize, specs.PayloadSize };
			/* Begin Handshake*/
			Packet syn = packetPrototype;
			syn.ServiceSize = 40;
			syn.PayloadSize = 0;
			Packet syn_ack = syn;
			syn_ack.ServiceSize = 60;
			syn_ack.SourceNode = specs.DestinationNode;
			syn_ack.DestinationNode = specs.SourceNode;
			Packet ack_handshake = syn;
			ack_handshake.ServiceSize = 20;

			double time = 0;
			NetworkNode* sourceNode = FindNode(specs.SourceNode);
			NetworkNode* destinationNode = FindNode(specs.DestinationNode);

			while (!status && retransmitTries < retransmissionTryLimit)
			{
				status = TransmitPacket(syn, sourceNode, time);
				res.PacketsSent++;
				if (status)
				{
					res.PacketsReceived++;
					res.ServiceSize+=syn.ServiceSize;
					status = TransmitPacket(syn_ack, destinationNode, time);
					res.PacketsSent++;
					if (status)
					{
						res.PacketsReceived++;
						res.ServiceSize += syn_ack.ServiceSize;
					}
				}

				if (!status)
				{
					retransmitTries++;
					time += 0.2f; // wait time before retransmission
				}
			}
			res.RetransmissionCount += retransmitTries;
			retransmitTries = 0;
			if (!status)
				return false;  // return if failed to establish connection
			status = false;

			TransmitPacket(ack_handshake, sourceNode, time, 0);
			res.ServiceSize += ack_handshake.ServiceSize;
			res.PacketsSent++;
			res.PacketsReceived++;

			/* End Handshake */

			/* Begin Data Transmission */
			Packet dataPacket = packetPrototype;
			Packet ack = packetPrototype;
			ack.ServiceSize = 20;
			ack.PayloadSize = 0;
			ack.DestinationNode = specs.SourceNode;
			ack.SourceNode = specs.DestinationNode;

			int32 dataLeft = res.MessageSize;

			while (dataLeft > 0)
			{

				while (!status && retransmitTries < retransmissionTryLimit)
				{
					dataPacket.PayloadSize = std::min(dataPacket.PayloadSize, dataLeft);
					status = TransmitPacket(dataPacket, sourceNode, time);
					res.PacketsSent++;
					if (status)
					{
						res.PacketsReceived++;
						res.ServiceSize += dataPacket.ServiceSize;
						status = TransmitPacket(ack, destinationNode, time);
						res.PacketsSent++;
						if (status)
						{
							res.PacketsReceived++;
							res.ServiceSize += ack.ServiceSize;
							dataLeft -= dataPacket.PayloadSize;
						}
					}

					if (!status)
					{
						retransmitTries++;
						time += 0.2f;
					}
				}

				res.RetransmissionCount += retransmitTries;
				retransmitTries = 0;
				if (!status)
					return false;  // return if connection lost
				status = false;
			}

			/* End Data Transmission */

			/* Begin Finish Handshake*/
			Packet fin = packetPrototype;
			fin.ServiceSize = 20;
			fin.PayloadSize = 0;
			Packet ack_finish = fin;
			ack_finish.ServiceSize = 20;
			ack_finish.SourceNode = specs.DestinationNode;
			ack_finish.DestinationNode = specs.SourceNode;

			// Sender fin
			while (!status && retransmitTries < retransmissionTryLimit)
			{
				status = TransmitPacket(fin, sourceNode, time);
				res.PacketsSent++;
				if (status)
				{
					res.PacketsReceived++;
					res.ServiceSize += fin.ServiceSize;
					status = TransmitPacket(ack_finish, destinationNode, time);
					res.PacketsSent++;
					if (status)
					{
						res.PacketsReceived++;
						res.ServiceSize += ack_finish.ServiceSize;
					}
				}

				if (!status)
				{
					retransmitTries++;
					time += 0.2f; // wait time before retransmission
				}
			}
			res.RetransmissionCount += retransmitTries;
			retransmitTries = 0;
			if (!status)
				return false;
			status = false;


			fin.SourceNode = specs.DestinationNode;
			fin.DestinationNode = specs.SourceNode;
			ack_finish.SourceNode = specs.SourceNode;
			ack_finish.DestinationNode = specs.DestinationNode;

			// Receiver fin
			while (!status && retransmitTries < retransmissionTryLimit)
			{
				status = TransmitPacket(fin, destinationNode, time);
				res.PacketsSent++;
				if (status)
				{
					res.PacketsReceived++;
					res.ServiceSize += fin.ServiceSize;
					status = TransmitPacket(ack_finish, sourceNode, time);
					res.PacketsSent++;
					if (status)
					{
						res.PacketsReceived++;
						res.ServiceSize += ack_finish.ServiceSize;
					}
				}

				if (!status)
				{
					retransmitTries++;
					time += 0.2f; // wait time before retransmission
				}
			}
			res.RetransmissionCount += retransmitTries;
			retransmitTries = 0;
			if (!status)
				return false;
			status = false;



			/* End Finish Handshake */

			res.TransmissionTime = time;
			res.PacketsLossRate = (float)res.PacketsSent - (float)res.PacketsReceived / (float)res.PacketsSent;

		}
		else
		{
			Packet datagram = { ENetworkProtocol::UDP, specs.SourceNode, specs.DestinationNode, specs.ServiceSize, specs.PayloadSize };
			int32 dataLeft = res.MessageSize;

			double time = 0;
			NetworkNode* sourceNode = FindNode(specs.SourceNode);

			while (dataLeft > 0)
			{
				res.PacketsSent++;
				res.ServiceSize += datagram.ServiceSize;
				datagram.PayloadSize = std::min(datagram.PayloadSize, dataLeft);
				status = TransmitPacket(datagram, sourceNode, time);
				dataLeft -= datagram.PayloadSize;
				if (status)
					res.PacketsReceived++;

			}

			res.TransmissionTime = time;
			res.PacketsLossRate = 1.f - (float)res.PacketsReceived / (float)res.PacketsSent;

		}
		m_Result.push_back(res);
	}

	void Simulation::Draw()
	{
		if (!m_Nodes)
			return;

		ControlPanel();
		ImGui::SameLine();
		SimulationTable();

	}

	void Simulation::ControlPanel()
	{
		ImGui::BeginChild("Packet Settings Container", ImVec2(260, 0), true);
		ImGui::Text("Source Node");
		ImGui::SameLine(120);
		NodeSelector("##Source Node", &m_Settings.SourceNode, m_Settings.bRandomSourceNode, 100);

		ImGui::Text("Destination Node");
		ImGui::SameLine(120);
		NodeSelector("##Destination Node", &m_Settings.DestinationNode, m_Settings.bRandomDestinationNode, 100);


		ImGui::Text("Protocol");
		ImGui::SameLine(120);
		static const char* protocols[] = { "UDP", "TCP" };

		int32 itemSelectedIdx = +(m_Settings.Protocol);
		const char* comboTypePreviewValue;

		if (m_Settings.bRandomProtocol)
			comboTypePreviewValue = "Random";
		else
			comboTypePreviewValue = protocols[itemSelectedIdx];

		ImGui::SetNextItemWidth(100);
		if (ImGui::BeginCombo("##Protocol", comboTypePreviewValue))
		{
			for (int32 n = 0; n < 2; n++)
			{
				bool isSelected = (itemSelectedIdx == n);
				if (ImGui::Selectable(protocols[n], isSelected))
					itemSelectedIdx = n;

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			m_Settings.Protocol = ToENetworkProtocol(itemSelectedIdx);
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		ImGui::Checkbox("##RandomProtocol", &m_Settings.bRandomProtocol);

		ImGui::Text("MTU");
		ImGui::SameLine(120);
		ImGui::SetNextItemWidth(100);
		if (m_Settings.bRandomMTU)
		{
			char buff[7] = "Random";
			ImGui::InputText("##MTUSize", buff, 7, ImGuiInputTextFlags_ReadOnly);
		}
		else
		{
			if (ImGui::InputInt("##MTUSize", &m_Settings.MTU, 10, 100))
			{
				m_Settings.MTU = std::clamp(m_Settings.MTU, 500, 9000);
			}
		}
		ImGui::SameLine();
		ImGui::Checkbox("##RandomMTU", &m_Settings.bRandomMTU);

		ImGui::Text("Message Count");
		ImGui::SameLine(120);
		ImGui::SetNextItemWidth(100);
		if (m_Settings.bRandomMessageCount)
		{
			char buff[7] = "Random";
			ImGui::InputText("##MessageCount", buff, 7, ImGuiInputTextFlags_ReadOnly);
		}
		else
		{
			if (ImGui::InputInt("##MessageCount", &m_Settings.MessageCount, 1, 10))
			{
				m_Settings.MessageCount = std::clamp(m_Settings.MessageCount, 1, 10);
			}
		}

		ImGui::SameLine();
		ImGui::Checkbox("##RandomMessageCount", &m_Settings.bRandomMessageCount);


		ImGui::Text("Message Size");
		ImGui::SameLine(120);
		ImGui::SetNextItemWidth(100);
		if (m_Settings.bRandomMessageSize)
		{
			char buff[7] = "Random";
			ImGui::InputText("##MessageSize", buff, 7, ImGuiInputTextFlags_ReadOnly);
		}
		else
		{
			if (ImGui::InputInt("##MessageSize", &m_Settings.MessageSize, 0, 0))
			{
				m_Settings.MessageSize = std::clamp(m_Settings.MessageSize, 500, 10000000);
			}
		}
		ImGui::SameLine();
		ImGui::Checkbox("##RandomMessageSize", &m_Settings.bRandomMessageSize);

		ImGui::Spacing();
		float width = ImGui::GetContentRegionAvail().x;
		if (ImGui::Button("Send", ImVec2{ width, 30.f }))
		{
			Simulate(m_Settings);
		}
		if (ImGui::Button("Clear", ImVec2{ width, 30.f }))
		{
			m_Result.clear();
		}
		ImGui::EndChild();
	}

	void Simulation::SimulationTable()
	{
		ImGui::BeginChild("Simulation Table Container", ImVec2(0, 0), true);

		const ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable |
			ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_SizingFixedFit |
			ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;

		if (ImGui::BeginTable("Simulation Result Table", 10, tableFlags))
		{
			ImGui::TableSetupScrollFreeze(0, 1);

			ImGui::TableSetupColumn(" Protocol");
			ImGui::TableSetupColumn("Source");
			ImGui::TableSetupColumn("Destination");
			ImGui::TableSetupColumn("Size");
			ImGui::TableSetupColumn("Sent Packets");
			ImGui::TableSetupColumn("Received Packets");
			ImGui::TableSetupColumn("Service");
			ImGui::TableSetupColumn("Time");
			ImGui::TableSetupColumn("Retransmission");
			ImGui::TableSetupColumn("Loss");

			ImFont* boldFont = ImGui::GetIO().Fonts->Fonts[0];
			ImGui::PushFont(boldFont);
			ImGui::TableHeadersRow();
			ImGui::PopFont();

			static const char* types[]{ " UDP", " TCP" };

			for (int32 i = 0; i < m_Result.size(); i++)
			{
				PacketTransmissionResult& res = m_Result[i];
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(types[+res.Protocol]);

				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%d", res.SourceNode);
				ImGui::TableSetColumnIndex(2);
				ImGui::Text("%d", res.DestinationNode);

				ImGui::TableSetColumnIndex(3);
				ImGui::Text("%d", res.MessageSize);

				ImGui::TableSetColumnIndex(4);
				ImGui::Text("%d", res.PacketsSent);

				ImGui::TableSetColumnIndex(5);
				ImGui::Text("%d", res.PacketsReceived);

				ImGui::TableSetColumnIndex(6);
				ImGui::Text("%d", res.ServiceSize);

				ImGui::TableSetColumnIndex(7);
				if (res.Success)
					ImGui::Text("%.4f (s)", res.TransmissionTime);
				else
					ImGui::Text("Connection lost");

				ImGui::TableSetColumnIndex(8);
				if (res.Protocol == ENetworkProtocol::TCP)
					ImGui::Text("%d", res.RetransmissionCount);
				else
					ImGui::Text(" ");

				ImGui::TableSetColumnIndex(9);
				if (res.Protocol == ENetworkProtocol::UDP)
					DisplayDynamicPrecisionPercentage(res.PacketsLossRate);
				else
					ImGui::Text(" ");

			}


			ImGui::EndTable();
		}

		ImGui::EndChild();
	}

	void Simulation::NodeSelector(const char* label, int32* location, bool& random, int32 width)
	{
		std::string comboPreviewValueStr;
		if (random)
			comboPreviewValueStr = "Random";
		else if (*location == -1)
			comboPreviewValueStr = "Select ID";
		else
			comboPreviewValueStr = std::to_string(*location);

		const char* comboPreviewValue = comboPreviewValueStr.c_str();

		ImGui::SetNextItemWidth(width);

		if (ImGui::BeginCombo(label, comboPreviewValue))
		{
			for (int i = 0; i < m_Nodes->size(); i++)
			{
				int nodeID = static_cast<int32>((*m_Nodes)[i]->ID);
				bool isSelected = (*location == nodeID);
				if (ImGui::Selectable(std::to_string(nodeID).c_str(), isSelected))
				{
					*location = nodeID;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::SameLine();
		ImGui::Checkbox(("##" + String(label) + "Random").c_str(), &random);
	}

	void Simulation::DisplayDynamicPrecisionPercentage(float value)
	{
		float v = value * 100.f;
		int32 precision = 0;
		float fractional_part = glm::abs(v - glm::round(v));
		if (fractional_part > 0.001f)
		{
			if (fractional_part < 0.01f)
				precision = 3;
			else if (fractional_part < 0.1f)
				precision = 2;
			else
				precision = 1;
		}
		char buffer[32];
		snprintf(buffer, sizeof(buffer), ("%." + std::to_string(precision) + "f%%").c_str(), v);
		ImGui::Text("%s", buffer);

	}

	NetworkNode* Simulation::FindNode(int32 nodeID)
	{
		auto it = std::find_if(m_Nodes->begin(), m_Nodes->end(), [&](const UniquePtr<NetworkNode>& node)
							   {
								   return node->ID == nodeID;
							   });

		if (it == m_Nodes->end())
			return nullptr;
		return it->get();
	}

	Channel* Simulation::FindChannel(NetworkNode* node1, int32 node2)
	{
		auto it = std::find_if(node1->Channels.begin(), node1->Channels.end(), [&](const Channel* c)
							   {
								   return c->Node1->ID == node1->ID && c->Node2->ID == node2 || c->Node1->ID == node2 && c->Node2->ID == node1->ID;
							   });
		if (it == node1->Channels.end())
			return nullptr;
		return *it;
	}

	bool Simulation::TransmitPacket(const Packet& packet, NetworkNode* transitionNode, double& time, float errorOverride /*= -1.f*/)
	{

		int32 nextHop = transitionNode->RoutingTable[packet.DestinationNode].NextHopID;
		if (nextHop < 0)
			return false;

		Channel* channel = FindChannel(transitionNode, nextHop);
		if (!channel)
			return false;

		float transmissionRate = 10000000.f;
		if (channel->Type == EChannelType::HalfDuplex && packet.Protocol == ENetworkProtocol::TCP)
			transmissionRate *= 0.5f;
		time += (float)(packet.PayloadSize + packet.ServiceSize) * (float)channel->Weight / transmissionRate;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dist(0.0f, 100.0f);

		float errror = errorOverride;
		if (errorOverride == -1.f)
			errror = channel->ErrorProbability;

		float random = dist(gen);
		if (random < errror)
			return false;

		if (nextHop == packet.DestinationNode)
			return true;

		NetworkNode* nextNode = FindNode(nextHop);
		if (!nextNode)
			return false;

		if (errorOverride != -1.f)
			return TransmitPacket(packet, nextNode, time, errorOverride);
		return TransmitPacket(packet, nextNode, time);
	}

	PacketTransmissionSpecifications::PacketTransmissionSpecifications(const PacketTransmissionSettings& settings, std::vector<UniquePtr<NetworkNode>>* nodes)
	{
		std::random_device rd;
		std::mt19937 gen(rd());

		Protocol = settings.Protocol;
		SourceNode = settings.SourceNode;
		DestinationNode = settings.DestinationNode;
		MessageSize = settings.MessageSize;
		MTU = settings.MTU;



		if (settings.bRandomProtocol)
		{
			std::uniform_int_distribution<> dis(0, 1);
			Protocol = ToENetworkProtocol(dis(gen));
		}

		if (settings.bRandomSourceNode)
		{
			std::uniform_int_distribution<> dis(0, nodes->size() - 1);
			SourceNode = nodes->operator[](dis(gen))->ID;
		}

		if (settings.bRandomDestinationNode)
		{
			std::uniform_int_distribution<> dis(0, nodes->size() - 1);
			DestinationNode = nodes->operator[](dis(gen))->ID;
			while (nodes->size() > 1 && DestinationNode == SourceNode)
				DestinationNode = nodes->operator[](dis(gen))->ID;
		}

		if (settings.bRandomMessageSize)
		{
			std::uniform_int_distribution<> dis(500, 10000000);
			MessageSize = dis(gen);
		}
	}

}
