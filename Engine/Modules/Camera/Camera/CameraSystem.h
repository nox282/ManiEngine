#pragma once

#include <Camera/Camera.h>
#include <Core/System/System.h>
#include <ECS/Entity.h>
#include <glm/glm.hpp>

namespace Mani
{
	class EntityRegistry;
	class SystemContainer;
	struct Transform;

	struct CameraConfig
	{
		float fov = 45.f;
		float width = 800.f;
		float height = 800.f;
		float nearClipPlane = .1f;
		float farClipPlane = 1000.f;
	};

	struct CameraComponent
	{
		CameraConfig config;

		glm::mat4 projection;
		glm::mat4 view;
	};

	class CameraSystem : public SystemBase
	{
	public:
		virtual std::string_view getName() const override;
		virtual bool shouldTick(EntityRegistry& registry) const override;

		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override;
		virtual void onDeinitialize(EntityRegistry& registry) override;

		virtual void tick(float deltaTime, EntityRegistry& registry) override;
	
		const CameraComponent* getCameraComponent(const EntityRegistry& registry) const;
		Transform* getCameraTransform(EntityRegistry& registry) const;

		void setCameraConfig(EntityRegistry& registry, const CameraConfig& config);

	private:
		EntityId m_cameraId;
	};
}