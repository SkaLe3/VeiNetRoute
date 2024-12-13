#include "LayerStack.h"
#include "VNR-Core/IO/Log.h"



VNR::LayerStack::~LayerStack()
{
	Clear();
}

void VNR::LayerStack::PushLayer(Layer* layer)
{

	m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
	m_LayerInsertIndex++;
}

void VNR::LayerStack::PushOverlay(Layer* overlay)
{
	m_Layers.emplace_back(overlay);
}

void VNR::LayerStack::PopLayer(Layer* layer)
{
	auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
	if (it != m_Layers.begin() + m_LayerInsertIndex)
	{
		layer->OnDetach();
		m_Layers.erase(it);
		m_LayerInsertIndex--;
	}
	else
	{
		VNR_CORE_WARN("Attempt to pop a layer \"{0}\".Layer\"{1}\" do not exist", layer->GetName(), layer->GetName());
	}
}

void VNR::LayerStack::PopOverlay(Layer* overlay)
{
	auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
	if (it != m_Layers.end())
	{
		overlay->OnDetach();
		m_Layers.erase(it);
	}
	else
	{
		VNR_CORE_WARN("Attempt to pop a overlay \"{0}\".Overlay\"{1}\" do not exist", overlay->GetName(), overlay->GetName());
	}
}

void VNR::LayerStack::Clear()
{
	for (Layer* layer : m_Layers)
	{
		layer->OnDetach();
		delete layer;
	}
	m_Layers.clear();
}
