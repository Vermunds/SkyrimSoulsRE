#include "RE/Skyrim.h"

#include "BGSSaveLoadManagerEx.h"
#include "Offsets.h"
#include "Tasks.h"
#include "Utility.h"

namespace SkyrimSoulsRE::Hooks
{
	namespace Console
	{
		struct CommandData
		{
			std::string command;
			std::vector<std::string> arguments;
			UInt32 numArgs;
		};

		CommandData* ParseCommand(std::string a_fullCommand)
		{
			CommandData* data = new CommandData();

			bool argumentExpected = false;

			for (auto ch : a_fullCommand) {
				if (ch == ' ') {
					if (!(data->command.empty()))
					{
						argumentExpected = true;
					}
				}
				else {

					if (argumentExpected)
					{
						argumentExpected = false;
						data->numArgs++;
						data->arguments.push_back(std::string());
					}

					if (data->numArgs != 0) {
						std::tolower(ch);
						data->arguments[data->numArgs - 1].push_back(ch);
					}
					else {
						std::tolower(ch);
						data->command.push_back(ch);
					}
				}
			}
			return data;
		}

		void ExecuteCommand_Hook(const RE::FxDelegateArgs& a_args)
		{
			const std::string commandStr = a_args[0].GetString();

			CommandData* data = ParseCommand(commandStr);

			if (data->command == "save" || data->command == "savegame")
			{

				BGSSaveLoadManagerEx::OutputFlag flag = BGSSaveLoadManagerEx::OutputFlag::kNone;

				if (data->numArgs == 1)
				{
					Tasks::SaveGameDelegate::RegisterTask(flag, data->arguments[0].c_str());
					return;
				}
				else if (data->numArgs > 1 && 5 >= data->numArgs)
				{
					for (int i = 1; i < data->numArgs; i++)
					{
						std::string arg = data->arguments[i];
						SInt32 num = 0;

						if (SkyrimSoulsRE::StrToInt(num, arg.c_str()))
						{
							flag |= (BGSSaveLoadManagerEx::OutputFlag)(num << (i - 1));
						}
						else
						{
							return;
						}
					}
					Tasks::SaveGameDelegate::RegisterTask(flag, data->arguments[0].c_str());
					return;
				}

			}
			else if (data->command == "servetime" && data->numArgs == 0)
			{
				RE::UI* ui = RE::UI::GetSingleton();
				RE::InterfaceStrings* interfaceStrings = RE::InterfaceStrings::GetSingleton();
				RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

				ui->numPausesGame++;
				ui->GetMenu(interfaceStrings->console)->flags |= RE::IMenu::Flag::kPausesGame;

				player->ServePrisonTime();
				return;
			}

			void(*ExecuteCommand_Original)(const RE::FxDelegateArgs&);
			ExecuteCommand_Original = reinterpret_cast<void(*)(const RE::FxDelegateArgs&)>(Offsets::Console_ExecuteCommand_Original.GetUIntPtr());
			return ExecuteCommand_Original(a_args);
		}

		void Register_Func(RE::Console* a_console)
		{
			RE::FxDelegate* dlg = a_console->fxDelegate.get();
			dlg->callbacks.GetAlt("ExecuteCommand")->callback = ExecuteCommand_Hook;
		}
	}
}