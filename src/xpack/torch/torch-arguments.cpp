//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2014-2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#include "torch-arguments.h"
#include <assert.h>

using namespace torch;

Arguments::Callback Arguments::CallbackFail = [](torch::Commander &command, std::vector<std::string> args) {
    return false;
};

std::string& StringRightPad(std::string &s, const int width)
{
    int diff = width - (int)s.length();
    if (diff > 0) {
        s = s + std::string(diff, ' ');
    }
    return s;
}

// Commander

Commander::Commander(const std::string &subcommand, int require, const std::string &desc, Callback callback)
:command(subcommand)
,require(require)
,description(desc)
,callback(callback)
{
}

Commander& Commander::Usage(const std::string &usage)
{
    this->usage = usage;
    return *this;
}

Commander& Commander::Option(const std::string &option, int require, const std::string &desc, Callback callback)
{
    m_optionRegistry.push_back((struct Option){option, require, desc, callback});
    return *this;
}

std::vector<std::string> Commander::GetOptionArgs(const std::string &option)
{
    auto iterator = m_optionArgsMap.find(option);
    if (iterator == m_optionArgsMap.end()) {
        return std::vector<std::string>();
    }
    return iterator->second;
}

bool Commander::HasOption(const std::string &option)
{
    auto iterator = m_optionArgsMap.find(option);
    if (iterator == m_optionArgsMap.end()) {
        return false;
    }
    return true;
}

std::string Commander::BuildHelp()
{
    std::string help;
    
    if (this->usage.length() > 0) {
        help += this->usage + '\n';
    }
    if (this->description.length() > 0) {
        help += this->description + '\n';
    }
    if (m_optionRegistry.size() <= 0) {
        return help;
    }
    
    int width = 3;
    for (auto option : m_optionRegistry) {
        if (option.option.length() > width) {
            width = (int)option.option.length();
        }
    }
    
    help += "\noptions:\n";
    for (auto option : m_optionRegistry) {
        help += '\t' + StringRightPad(option.option, width + 1);
        help += ": " + option.description + '\n';
    }
    
    return help;
}

bool Commander::Execute(std::vector<std::string> args)
{
    if (!this->BuildArgs(args)) {
        return false;
    }
    for (auto x : m_optionArgsMap) {
        struct Option *option = this->GetOptionByName(x.first);
        if (!option || (option->callback && !option->callback(*this, x.second))) {
            return false;
        }
    }
    if (this->callback && !this->callback(*this, m_commandArgs)) {
        return false;
    }
    return true;
}

std::vector<std::string> Commander::CutArgs(std::vector<std::string> &args, int index, int require)
{
    /* 
     * 从参数列表中截取当前Option/Command的参数，并将其参数全部从参数列表删除
     */
    std::vector<std::string> extractedArgs;
    if (require < 0) {
        int least = - require - 1;
        require = this->GetOptionArgsNumberBeforeNextOption(args, index);
        if (require < least) {
            return std::move(extractedArgs);
        }
    }
    if (index + require > args.size()) {
        return std::move(extractedArgs);
    }
    for (int i = index; (i < index + require) && (require-- > 0);) {
        if (this->GetOptionByName(args[i])) {
            return std::move(std::vector<std::string>());
        }
        extractedArgs.push_back(args[i]);
        args.erase(args.begin() + i);
    }
    return std::move(extractedArgs);
}

int Commander::GetOptionArgsNumberBeforeNextOption(const std::vector<std::string> &args, int index)
{
    /*
     * 获取到下一个Option之前的所有参数的个数，用于不定参数确定要收取的参数个数
     * 参数：
     *  - args: 参数列表，不能包含当前Option
     *  - index: 当前处理的列表项游标index
     */
    int num = 0;
    for (int i = index; i < args.size(); i++) {
        if (!this->GetOptionByName(args[i])) {
            num++;
        }
        else {
            break;
        }
    }
    return num;
}

struct Commander::Option* Commander::GetOptionByName(const std::string &name)
{
    /*
     * 通过Option名字获取Option记录结构，找不到则返回nullptr
     */
    for (int i = 0; i < m_optionRegistry.size(); i++) {
        if (m_optionRegistry[i].option == name) {
            return &m_optionRegistry[i];
        }
    }
    return nullptr;
}

bool Commander::BuildArgs(std::vector<std::string> &args)
{
    /*
     * 先构建Option参数，然后使用参数列表中剩下的参数构建Command参数
     */
    for (int i = 0; i < args.size(); i++) { // build option args
        std::string argv = args[i];
        struct Option *option = this->GetOptionByName(argv);
        if (!option) {
            continue;
        }
        args.erase(args.begin() + i);
        std::vector<std::string> optionArgs = this->CutArgs(args, i--, option->require);
        if ((option->require >= 0 && optionArgs.size() != option->require) ||
            (option->require < 0 && optionArgs.size() < (- option->require - 1))){
            return false;
        }
        m_optionArgsMap.insert(std::make_pair(argv, optionArgs));
    }

    m_commandArgs = this->CutArgs(args, 0, this->require);
    if (this->require >= 0) {
        return m_commandArgs.size() == this->require;
    }
    else {
        return m_commandArgs.size() >= (- this->require - 1);
    }
    return true;
}

// Arguments

Arguments::Arguments(int require, const std::string &desc, Callback callback)
:m_mainCommand(nullptr)
{
    m_mainCommand = new Commander("", require, desc, callback);
}

Arguments::~Arguments()
{
    for (auto cmd : m_subcommandRegistry) {
        delete cmd;
    }
    if (m_mainCommand) {
        delete m_mainCommand;
    }
}

Commander& Arguments::MainCommand()
{
    return *m_mainCommand;
}

Commander& Arguments::SubCommand(const std::string &subcommand, int require, const std::string &desc, Callback callback)
{
    Commander *commandObject = new Commander(subcommand, require, desc, callback);
    m_subcommandRegistry.push_back(commandObject);
    return *commandObject;
}

bool Arguments::Parse(int argc, const char * argv[])
{
    this->BuildArgs(argc, argv);
    Commander *command = this->GetSubCommand();
    if (!command) {
        bool ok = m_mainCommand->Execute(m_systemArgs);
        if (!ok) {
            this->OnHelp(m_mainCommand);
        }
        return ok;
    }
    this->ClearArgsToSubCommand(m_systemArgs, command->command);
    bool ok = command->Execute(m_systemArgs);
    if (!ok) {
        this->OnHelp(command);
    }
    return ok;
}

std::string Arguments::BuildHelp(Commander *command)
{
    command = command == nullptr ? this->m_mainCommand : command;
    std::string help = command->BuildHelp();
    if (command != this->m_mainCommand || m_subcommandRegistry.size() <= 0) {
        return help;
    }
    
    int width = 3;
    help += "\nsubcommands:\n";
    for (auto command : m_subcommandRegistry) {
        if (command->command.length() > width) {
            width = (int)command->command.length();
        }
    }
    for (auto command : m_subcommandRegistry) {
        help += '\t' + StringRightPad(command->command, width + 1);
        help += ": " + command->description + '\n';
    }
    return help;
}

int Arguments::RequireArgsAtLeast(int number)
{
    assert(number >= 0);
    return - number - 1;
}

void Arguments::ClearArgsToSubCommand(std::vector<std::string> &args, const std::string &subcommand)
{
    /*
     * 清除到指定命令的所有参数，用于清理和当前要运行的Commander无关的参数
     */
    while (args.size() > 0) {
        std::string argv = args.front();
        args.erase(args.begin());
        if (argv == subcommand) {
            break;
        }
    }
}

void Arguments::BuildArgs(int argc, const char * argv[])
{
    /*
     * 构建系统参数，对其进行预处理
     */
    for (int i = 0; i < argc; i++) {
        m_systemArgs.push_back(argv[i]);
    }
    m_systemArgs.erase(m_systemArgs.begin());
    m_application = m_systemArgs.front();
}

Commander* Arguments::GetSubCommand()
{
    /*
     * 从参数列表中查找最先的一个SubCommand，若找不到会返回nullptr，若存在多个则忽略
     */
    for (auto argv : m_systemArgs) {
        for (auto cmd : m_subcommandRegistry) {
            if (cmd->command == argv) {
                return cmd;
            }
        }
    }
    return nullptr;
}

void Arguments::OnHelp(Commander *command)
{
    printf("%s\n", this->BuildHelp(command).c_str());
}


