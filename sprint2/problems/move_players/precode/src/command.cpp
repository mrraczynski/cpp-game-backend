#include "command.h"

namespace command
{
	template <typename Action>
	void Command<Action>::AddCommand(std::string_view name, std::optional<Action&&> action)
	{
		next_commands.insert(std::make_shared<Command>(name, std::move(action)));
	}
	template <typename Action>
	std::shared_ptr<Command<Action>> Command<Action>::GetCommand(std::string_view command)
	{
		if (next_commands.find(command) != next_commands.end())
		{
			return next_commands[command];
		}
		else
		{
			return std::nullopt;
		}

	}
	template <typename Action>
	Action& Command<Action>::GetAction()	
	{
		if (action_ != std::nullopt)
		{
			return action_;
		}
		else
		{
			return std::nullopt;
		}
	}
}