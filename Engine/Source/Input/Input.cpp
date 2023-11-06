#include "Input/Input.h"
#include "Core/Logger.h"

namespace Trinity
{
	Input::~Input()
	{
		destroy();
	}

	bool Input::create(Window& window)
	{
		WindowCallbacks& callbacks = window.getCallbacks();

		callbacks.onKey.subscribe([this](int32_t key, bool pressed, uint32_t mods) {
			onKeyCallback(key, pressed);
		});

		callbacks.onMousePos.subscribe([this](float x, float y) {
			onMousePosCallback(x, y);
		});

		callbacks.onMouseButton.subscribe([this](int32_t button, bool pressed, int32_t mods) {
			onMouseButtonCallback(button, pressed, mods);
		});

		callbacks.onMouseScroll.subscribe([this](float xoffset, float yoffset) {
			onMouseScrollCallback(xoffset, yoffset);
		});

		mKeyboard = std::make_unique<Keyboard>();
		mMouse = std::make_unique<Mouse>();
		mActionBindings.resize(InputEvent::Max);

		return true;
	}

	void Input::destroy()
	{
	}

	bool Input::loadConfig(const json& inputConfig)
	{
		if (inputConfig.contains("actions"))
		{
			for (auto& action : inputConfig["actions"])
			{
				InputAction inputAction;
				inputAction.name = action["name"].get<std::string>();

				if (action.contains("keyboardKeys"))
				{
					for (auto& key : action["keyboardKeys"])
					{
						inputAction.keyboardKeys.push_back(key.get<int32_t>());
					}
				}

				if (action.contains("mouseButtons"))
				{
					for (auto& button : action["mouseButtons"])
					{
						inputAction.mouseButtons.push_back(button.get<int32_t>());
					}
				}

				addAction(inputAction);
			}
		}

		if (inputConfig.contains("axes"))
		{
			for (auto& axis : inputConfig["axes"])
			{
				InputAxis inputAxis;
				inputAxis.name = axis["name"].get<std::string>();

				if (axis.contains("keyboardKeys"))
				{
					for (auto& key : axis["keyboardKeys"])
					{
						auto type = key["type"].get<int32_t>();
						auto scale = key["scale"].get<float>();

						inputAxis.keyboardKeys.push_back({ type, scale });
					}
				}

				if (axis.contains("mouseButtons"))
				{
					for (auto& button : axis["mouseButtons"])
					{
						auto type = button["type"].get<int32_t>();
						auto scale = button["scale"].get<float>();

						inputAxis.mouseButtons.push_back({ type, scale });
					}
				}

				if (axis.contains("mousePositions"))
				{
					for (auto& position : axis["mousePositions"])
					{
						auto type = position["type"].get<int32_t>();
						auto scale = position["scale"].get<float>();

						inputAxis.mousePositions.push_back({ type, scale });
					}
				}

				if (axis.contains("mouseScrolls"))
				{
					for (auto& scroll : axis["mouseScrolls"])
					{
						auto type = scroll["type"].get<int32_t>();
						auto scale = scroll["scale"].get<float>();

						inputAxis.mouseScrolls.push_back({ type, scale });
					}
				}

				addAxis(inputAxis);
			}
		}

		return true;
	}

	void Input::update()
	{
		const glm::vec2& relPosition = mMouse->getRelativePosition();
		const glm::vec2& currScroll = mMouse->getScroll();

		for (auto& inputAxis : mAxisMap)
		{
			auto it = mAxisBindings.find(inputAxis.first);
			if (it == mAxisBindings.end())
			{
				continue;
			}

			for (auto& key : inputAxis.second.keyboardKeys)
			{
				it->second.notify(mKeyboard->isKeyDown(key.type) ? key.scale : 0.0f);
			}

			for (auto& button : inputAxis.second.mouseButtons)
			{
				it->second.notify(mMouse->isButtonDown(button.type) ? button.scale : 0.0f);
			}

			for (auto& position : inputAxis.second.mousePositions)
			{
				it->second.notify(relPosition[position.type] * position.scale);
			}

			for (auto& scroll : inputAxis.second.mouseScrolls)
			{
				it->second.notify(currScroll[scroll.type] * scroll.scale);
			}
		}
	}

	void Input::postUpdate()
	{
		mKeyboard->postUpdate();
		mMouse->postUpdate();
	}

	void Input::addAction(const InputAction& action)
	{
		mActionMap.insert({ action.name, action });

		for (int32_t key : action.keyboardKeys)
		{
			mKeyboardKeyActionMap.insert({ key, action.name });
		}

		for (int32_t button : action.mouseButtons)
		{
			mMouseButtonActionMap.insert({ button, action.name });
		}
	}

	void Input::addAxis(const InputAxis& axis)
	{
		mAxisMap.insert({ axis.name, axis });
	}

	void Input::bindAction(const std::string& actionName, InputEvent event, std::function<void(int32_t)> callback)
	{
		auto& bindings = mActionBindings[event];
		bindings[actionName].subscribe(callback);
	}

	void Input::bindAxis(const std::string& axisName, std::function<void(float)> callback)
	{
		mAxisBindings[axisName].subscribe(callback);
	}

	void Input::onKeyCallback(int32_t key, bool pressed)
	{
		mKeyboard->update(key, pressed);

		auto keyIt = mKeyboardKeyActionMap.find(key);
		if (keyIt == mKeyboardKeyActionMap.end())
		{
			return;
		}

		auto& bindings = pressed ? mActionBindings[InputEvent::Pressed] :
			mActionBindings[InputEvent::Released];

		auto bindingIt = bindings.find(keyIt->second);
		if (bindingIt != bindings.end())
		{
			bindingIt->second.notify(key);
		}

		if (mKeyboard->isKeyiggered(key))
		{
			auto& trigBindings = mActionBindings[InputEvent::Triggered];
			auto trigIt = trigBindings.find(keyIt->second);

			if (trigIt != trigBindings.end())
			{
				trigIt->second.notify(key);
			}
		}
	}

	void Input::onMousePosCallback(float x, float y)
	{
		mMouse->updatePosition(x, y);
	}

	void Input::onMouseButtonCallback(int32_t button, bool pressed, int32_t mods)
	{
		mMouse->update(button, pressed);

		auto buttonIt = mMouseButtonActionMap.find(button);
		if (buttonIt == mMouseButtonActionMap.end())
		{
			return;
		}

		auto& bindings = pressed ? mActionBindings[InputEvent::Pressed] :
			mActionBindings[InputEvent::Released];

		auto bindingIt = bindings.find(buttonIt->second);
		if (bindingIt != bindings.end())
		{
			bindingIt->second.notify(button);
		}

		if (mMouse->isButtoniggered(button))
		{
			auto& trigBindings = mActionBindings[InputEvent::Triggered];
			auto trigIt = trigBindings.find(buttonIt->second);

			if (trigIt != trigBindings.end())
			{
				trigIt->second.notify(button);
			}
		}
	}

	void Input::onMouseScrollCallback(float xoffset, float yoffset)
	{
		mMouse->updateScroll(xoffset, yoffset);
	}
}