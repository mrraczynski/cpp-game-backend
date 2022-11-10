#pragma once
#include <string>
#include <vector>
#include<memory>
#include <optional>
#include <unordered_map>

namespace command
{
	template <typename Action>
	class Command
	{
	public:
		Command(std::string_view name, std::optional<Action&&> action = std::nullopt) : name_(name), action_(std::move(action)) {}
		Command()
		{
			action_ = std::nullopt;
		}

		void AddCommand(std::string_view name, std::optional<Action&&> action = std::nullopt);
		std::shared_ptr<Command> GetCommand(std::string_view);
		Action& GetAction();

	private:			
		std::unordered_map<std::string, std::shared_ptr<Command>> next_commands;
		std::optional<Action> action_;
		std::string_view name_;
	};

}