#pragma once

namespace DROP
{

	class Game
	{
	public:
		virtual ~Game() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(float ts) {}
		virtual void OnUIRender() {}
	};

}