#pragma once

#include <glm/fwd.hpp>

namespace Mani
{
	struct PhysXStaticBoxComponent
	{
		glm::vec3 extent = glm::vec3(1.0f);
	};

	struct PhysXStaticSphereComponent
	{
		float radius = 0.f;
	};

	struct PhysXDynamicComponent
	{
		float mass = 1.f;
		float angularDamping = .5f;
	};

	struct PhysXDynamicBoxComponent : public PhysXDynamicComponent
	{
		glm::vec3 extent = glm::vec3(1.0f);
	};

	struct PhysXDynamicSphereComponent : public PhysXDynamicComponent
	{
		float radius = 0.f;
	};
}