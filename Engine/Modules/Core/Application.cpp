#pragma once

#include "Application.h"
#include <Core/System/SystemContainer.h>
#include <Core/World/WorldSystem.h>
#include <Core/CoreTime.h>
#include <Core/CoreAssert.h>
#include <Core/Log/LogSystem.h>

using namespace ECSEngine;

Application* Application::sm_application = nullptr;

Application::Application()
{
	// there should be only one application instance.
	ECSE_ASSERT(sm_application == nullptr, "an Application instance already exists.");
	sm_application = this;

	m_systemContainer = new SystemContainer();
	m_systemContainer->initialize();
	m_systemContainer->createSystem<LogSystem>();
	m_systemContainer->createSystem<WorldSystem>();
}

Application::~Application()
{
	m_systemContainer->deinitialize();
	m_systemContainer->destroySystem<LogSystem>();
	m_systemContainer->destroySystem<WorldSystem>();
	delete m_systemContainer;
	sm_application = nullptr;
}

Application& ECSEngine::Application::get()
{
	return *sm_application;
}

SystemContainer& Application::getSystemContainer()
{
	return *m_systemContainer;
}

void Application::run()
{
	Time::onApplicationStart();
	m_systemContainer->initialize();
	m_isRunning = true;
	while (m_isRunning)
	{
		Time::onNewFrame();
		tick(Time::getDeltaTime());
	}
	m_systemContainer->deinitialize();
}

void Application::stop()
{
	m_isRunning = false;
}

void Application::tick(float deltaTime)
{
	m_systemContainer->tick(deltaTime);
}
