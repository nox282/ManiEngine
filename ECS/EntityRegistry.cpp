#include "EntityRegistry.h"

ECSEngine::EntityId ECSEngine::EntityRegistry::create()
{
	if (m_entities.size() >= UINT64_MAX && m_entityPool.size() == 0)
	{
		return UINT64_MAX;
	}

	if (m_entityPool.size() > 0)
	{
		EntityId id = m_entityPool.back();
		m_entityPool.pop_back();
		m_entities[id].bisAlive = true;
		return id;
	}

	Entity entity;
	entity.id = m_entities.size();
	entity.bisAlive = true;
	m_entities.push_back(entity);
	return entity.id;
}

bool ECSEngine::EntityRegistry::destroy(EntityId entityId)
{
	if (!isValid(entityId))
	{
		return false;
	}

	Entity& entity = m_entities[entityId];
	entity.bisAlive = false;
	entity.components.reset();

	m_entityPool.push_back(entity.id);
	return true;
}

bool ECSEngine::EntityRegistry::isValid(EntityId entityId) const
{
	if (entityId >= m_entities.size() && entityId == UINT64_MAX)
	{
		return false;
	}

	return m_entities[entityId].bisAlive;
}

bool ECSEngine::EntityRegistry::removeComponent_Internal(EntityId entityId, ComponentId componentId)
{
	if (!isValid(entityId))
	{
		return false;
	}

	if (!hasComponent_Internal(entityId, componentId))
	{
		// Entity doesn't have a component of that type.
		return false;
	}

	Entity& entity = m_entities[entityId];
	entity.components.set(componentId, false);
	return true;
}

bool ECSEngine::EntityRegistry::hasComponent_Internal(EntityId entityId, ComponentId typeId)
{
	if (!isValid(entityId))
	{
		return false;
	}

	return m_entities[entityId].components.test(typeId);
}
