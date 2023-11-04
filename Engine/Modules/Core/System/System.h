#pragma once

#include <Core/Core.h>
#include <string_view>

namespace ECSEngine
{
	class EntityRegistry;
	class SystemContainer;
	
	class Core_API SystemBase
	{
	public:
		virtual std::string_view getName() const;
		virtual float getFrameTime() const;
		virtual bool shouldTick(EntityRegistry& registry) const;

		void initialize(EntityRegistry& registry, SystemContainer& systemContainer);
		void deinitialize(EntityRegistry& registry);

		virtual void tick(float deltaTime, EntityRegistry& registry);
	
	protected:
		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer);
		virtual void onDeinitialize(EntityRegistry& registry);

	private:
		bool m_bIsInitialized = false;
	};
}