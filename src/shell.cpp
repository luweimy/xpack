//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016年 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#include <iostream>
#include <vector>
#include <sys/time.h>
#include <signal.h>
#include "xpack/torch/torch.h"
#include "xpack/xpack.h"
#include "xpack/xpack-util.h"
#include "xpack/xpack-hash.h"

const char *PACKAGE_NOT_EXISTS = "package not exists.";
const char *PACKAGE_IS_VALID = "package is valid.";
const char *NAME_NOT_EXISTS = "name not exists.";
const char *NO_CONFLICTS = "no conflicts.";
const char *FILE_NOT_EXISTS = "file not exists.";
const char *NOT_EXISTS = "not exists.";
const char *FILE_ALREADY_EXISTS = "file already exists.";
const char *MAKE_TMP_PACKAGE_FAILED = "make tmp-package failed.";
const char *MERGE_TMP_PACKAGE_FAILED = "merge tmp-package failed.";
const char *INTERRUPT_ERROR = "interrupt operation may damage the package.";

// Utils

static void ErrorLog(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    if (isatty(fileno(stdout))) {
        torch::Console::SetForegroundColor(torch::Console::Color::Red);
        torch::Console::Apply();
    }
    vprintf(fmt, ap);
    if (isatty(fileno(stdout))) {
        torch::Console::Clear();
    }
    va_end(ap);
}

static void InfoLog(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

static void PathStatusLog(const std::string &path, bool status) {
    InfoLog("%s\t ... %s\n", torch::String::RightPad(path, 50, ' ').c_str(), status ? "ok" : "failed");
}

static void BenchmarkLog(size_t fnum, int64_t microseconds) {
    float seconds = (double)microseconds / 1000 / 1000;
    InfoLog("Benchmark:\n");
    InfoLog("\tFiles   : %u\n", fnum);
    InfoLog("\tSeconds : %f\n", seconds);
}

// MainCommand

bool OnCommand_Main(torch::Commander &command, std::vector<std::string> args) {
    if (command.HasOption("-v")) {
        InfoLog("version %s\n", xpack::Package::GetVersion().c_str());
        return true;
    }
    return false;
}

// SubCommand: Add

bool OnCommand_Add(torch::Commander &command, std::vector<std::string> args) {
    assert(args.size() == 2);
    std::string name;
    if (command.HasOption("-n")) {
        name = command.GetOptionArgs("-n").front();
    }
    else {
        name = args[1];
    }
    if (!torch::FileSystem::IsFile(args[0])) {
        ErrorLog("%s\n", PACKAGE_NOT_EXISTS);
        return true;
    }
    if (!torch::FileSystem::IsFile(args[1])) {
        ErrorLog("%s\n", FILE_NOT_EXISTS);
        return true;
    }
    
    bool force = command.HasOption("-f");
    bool compress = command.HasOption("-z");
    bool crypto = false;

    xpack::Package pack;
    if (!pack.Open(args[0], false)) {
        ErrorLog("%s\n", xpack::GetLastErrorMessage());
        return false;
    }

    if (command.HasOption("-p")) {
        std::string password = command.GetOptionArgs("-p").front();
        if (password.length() > 0) {
            crypto = true;
            pack.SetSecretKey((unsigned char *)password.c_str(), (int)password.length());
        }
    }

    std::string path = args[1];
    std::string key = name;
    if (key.length() <= 0) {
        key = path;
    }
    if (force) {
        pack.RemoveEntry(key);
    }
    if (!pack.AddEntry(key, torch::File::GetBytes(path), crypto, compress)) {
        PathStatusLog(args[1], false);
        ErrorLog("%s\n", xpack::GetLastErrorMessage());
        return true;
    }
    PathStatusLog(args[1], true);
    return true;
}

// Multi Add

bool OnCommand_MultiAdd(torch::Commander &command, std::vector<std::string> args) {
    assert(args.size() >= 2);
    if (!torch::FileSystem::IsFile(args[0])) {
        ErrorLog("%s\n", PACKAGE_NOT_EXISTS);
        return true;
    }
    
    bool force = command.HasOption("-f");
    bool compress = command.HasOption("-z");
    bool crypto = false;
    
    xpack::Package pack;
    if (!pack.Open(args[0], false)) {
        ErrorLog("%s\n", xpack::GetLastErrorMessage());
        return true;
    }
    if (command.HasOption("-p")) {
        std::string password = command.GetOptionArgs("-p").front();
        if (password.length() > 0) {
            crypto = true;
            pack.SetSecretKey((unsigned char *)password.c_str(), (int)password.length());
        }
    }
    
    for (int i = 1; i < args.size(); i++) {
        if (!torch::FileSystem::IsFile(args[i])) {
            ErrorLog("%s %s\n", args[i].c_str(), NOT_EXISTS);
            break;
        }
        if (force) {
            pack.RemoveEntry(args[i]);
        }
        if (!pack.AddEntry(args[i], torch::File::GetBytes(args[i]), crypto, compress)) {
            PathStatusLog(args[i], false);
            ErrorLog("%s\n", xpack::GetLastErrorMessage());
            break;
        }
        PathStatusLog(args[i], true);
    }
    return true;
}

// SubCommand: rm

bool OnCommand_Remove(torch::Commander &command, std::vector<std::string> args) {
    assert(args.size() == 2);
    if (!torch::FileSystem::IsFile(args[0])) {
        ErrorLog("%s\n", PACKAGE_NOT_EXISTS);
        return true;
    }
    xpack::Package pack;
    if (!pack.Open(args[0], false)) {
        ErrorLog("%s\n", xpack::GetLastErrorMessage());
        return true;
    }
    
    std::string wildcard = args[1];
    std::vector<std::string> removeLater;
    pack.ForeachEntryNames([&](const std::string &name){
        int ret = torch::WildcardMatcher::Match(wildcard, name);
        if (ret == 1) {
            removeLater.push_back(name);
        }
        else if (ret < 0) {
            ErrorLog("%s\n", torch::WildcardMatcher::FormatError(ret));
            return false;
        }
        return true;
    });
    
    if (removeLater.size() > 0) {
        for (auto name : removeLater) {
            bool ok = pack.RemoveEntry(name);
            PathStatusLog(name, ok);
            if (!ok) {
                ErrorLog("%s(%s)\n", NAME_NOT_EXISTS, xpack::GetLastErrorMessage());
                break;
            }
        }
    }
    return true;
}

// SubCommand: cat

bool OnCommand_Cat(torch::Commander &command, std::vector<std::string> args) {
    assert(args.size() == 2);
    if (!torch::FileSystem::IsFile(args[0])) {
        ErrorLog("%s\n", PACKAGE_NOT_EXISTS);
        return true;
    }
    
    xpack::Package pack;
    if (!pack.Open(args[0])) {
        ErrorLog("%s\n", xpack::GetLastErrorMessage());
        return true;
    }
    
    if (command.HasOption("-p")) {
        std::string password = command.GetOptionArgs("-p").front();
        if (password.length() > 0) {
            pack.SetSecretKey((unsigned char *)password.c_str(), (int)password.length());
        }
    }

    bool exist = false;
    std::string wildcard = args[1];
    pack.ForeachEntryNames([&](const std::string &name){
        int ret = torch::WildcardMatcher::Match(wildcard, name);
        if (ret == 1) {
            exist = true;
            torch::Data content = pack.GetEntryDataByName(name);
            if (!content.IsNull()) {
                InfoLog("%s", content.ToString().c_str());
            }
        }
        else if (ret < 0) {
            ErrorLog("%s\n", torch::WildcardMatcher::FormatError(ret));
            return false;
        }
        return true;
    });
    
    if (!exist) {
        ErrorLog("%s\n", NAME_NOT_EXISTS);
        return true;
    }

    return true;
}

// SubCommand: ls

bool OnCommand_List(torch::Commander &command, std::vector<std::string> args) {
    assert(args.size() >= 1);
    if (!torch::FileSystem::IsFile(args[0])) {
        ErrorLog("%s\n", PACKAGE_NOT_EXISTS);
        return true;
    }
    xpack::Package pack;
    if (!pack.Open(args[0])) {
        ErrorLog("%s\n", xpack::GetLastErrorMessage());
        return true;
    }
    std::string wildcard = args.size()>1 ? args[1] : "";
    
    std::vector<std::string> allnames = pack.GetEntryNames();
    std::vector<std::string> displaynames;
    if (wildcard.length() <= 0) {
        displaynames = allnames;
    }
    else {
        for (auto name : allnames) {
            int ret = torch::WildcardMatcher::Match(wildcard, name);
            if (ret == 1) {
                displaynames.push_back(name);
            }
            else if (ret < 0) {
                ErrorLog("%s\n", torch::WildcardMatcher::FormatError(ret));
                return true;
            }
        }
    }
    
    bool isSort = command.HasOption("-s");
    if (isSort) {
        std::sort(displaynames.begin(), displaynames.end(), [&](const std::string &name1, const std::string name2){
            size_t l = pack.GetEntrySizeByName(name1);
            size_t r = pack.GetEntrySizeByName(name2);
            return l > r;
        });
    }
    
    bool isDisplayMore = command.HasOption("-l");

    if (isDisplayMore) {
        size_t maxl = 0;
        for (auto name : displaynames) {
            size_t l = torch::ByteToHumanReadableString((int)pack.GetEntrySizeByName(name)).length();
            if (l > maxl) {
                maxl = l;
            }
        }
        for (auto name : displaynames) {
            std::string info;
            xpack::Context *ctx = pack.GetContxt();
            auto shsct = ctx->hash->GetById(ctx->HashMaker(name, 0));
            auto hsct = ctx->hash->QueryByName(name);
            uint32_t size = pack.GetEntrySizeByName(name);
            std::string stringflags = torch::ToBinaryHumanReadable<uint8_t>(hsct->flags);
            for (int i = 0; i < stringflags.length(); i++) {
                stringflags[i] = (stringflags[i] == '0') ? '-' : '+';
            }
            info += torch::String::Format("%s(id)", torch::ToHexHumanReadable<uint32_t>(hsct->hash).c_str());
            info += " - ";
            info += torch::String::Format("%s(crc)", torch::ToHexHumanReadable<uint32_t>(hsct->crc).c_str());
            info += "   ";
            info += torch::String::Format("%s", stringflags.c_str());
            info += torch::String::LeftPad(torch::String::Format("%d(bi)", hsct->block_index), 9, ' ');
            info += torch::String::LeftPad(torch::String::Format("%s", torch::ByteToHumanReadableString((int)size).c_str()), (int)maxl + 2, ' ');
            info += torch::String::RightPad(torch::String::Format("%s", shsct->conflict_refc > 0 ? "!C" : " "), 3, ' ');
            InfoLog("%s%s\n", info.c_str(), name.c_str());
        }
    }
    else {
        int maxsz = 0;
        for (auto name : displaynames) {
            if (name.length() > maxsz) {
                maxsz = (int)name.length();
            }
        }
        
        struct column_max_record {
            int minsz; int column;
        };
        std::vector<column_max_record> column_records = {
            {50, 1}, {25, 2}, {13, 3}, {0, 4}
        };
        
        for (auto record : column_records) {
            if (maxsz >= record.minsz) {
                for (int i = 0; i < displaynames.size(); i+=record.column) {
                    for (int j = 0; j < record.column; j++) {
                        if (i + j < displaynames.size()) {
                            InfoLog("%s", torch::String::RightPad(displaynames[i + j], maxsz + 2, ' ').c_str());
                        }
                    }
                    InfoLog("\n");
                }
                break;
            }
        }
    }

    return true;
}

// SubCommand: make

bool OnCommand_Make(torch::Commander &command, std::vector<std::string> args) {
    assert(args.size() == 1);
    std::string path = args.front();
    
    if (torch::FileSystem::IsPathExist(path)) {
        if (command.HasOption("-f")) {
            torch::FileSystem::Remove(path);
        }
        else {
            ErrorLog("%s\n", FILE_ALREADY_EXISTS);
            return true;
        }
    }
    
    bool ok = xpack::PackageHelper::MakeNew(path);
    if (!ok) {
        ErrorLog("%s\n", xpack::GetLastErrorMessage());
    }
    return true;
}

// SubCommand: dump

bool OnCommand_Dump(torch::Commander &command, std::vector<std::string> args) {
    if (!torch::FileSystem::IsFile(args[0])) {
        ErrorLog("%s\n", PACKAGE_NOT_EXISTS);
        return true;
    }
    xpack::Package pack;
    if (!pack.Open(args[0])) {
        ErrorLog("%s\n", xpack::GetLastErrorMessage());
        return true;
    }
    if (command.HasOption("--signature")) {
        InfoLog("%s\n", xpack::DumpUtils::DumpSignature(pack.GetContxt()).c_str());
        return true;
    }
    if (command.HasOption("--header")) {
        InfoLog("%s\n", xpack::DumpUtils::DumpHeader(pack.GetContxt()).c_str());
        return true;
    }
    if (command.HasOption("--name")) {
        InfoLog("%s\n", xpack::DumpUtils::DumpNames(pack.GetContxt()).c_str());
        return true;
    }
    if (command.HasOption("--data")) {
        InfoLog("%s\n", xpack::DumpUtils::DumpData(pack.GetContxt()).c_str());
        return true;
    }
    if (command.HasOption("--data-offsize")) {
        std::vector<std::string> args = command.GetOptionArgs("--data-offsize");
        uint32_t offset = (uint32_t)atoll(args[0].c_str());
        uint32_t size   = (uint32_t)atoll(args[1].c_str());
        InfoLog("%s\n", xpack::DumpUtils::DumpData(pack.GetContxt(), offset, size).c_str());
        return true;
    }
    if (command.HasOption("--block")) {
        InfoLog("%s\n", xpack::DumpUtils::DumpBlocks(pack.GetContxt(), true).c_str());
        return true;
    }
    if (command.HasOption("--block-name")) {
        std::string name = command.GetOptionArgs("--block-name").front();
        InfoLog("%s\n", xpack::DumpUtils::DumpBlock(pack.GetContxt(), name).c_str());
        return true;
    }
    if (command.HasOption("--block-index")) {
        int32_t index = (int32_t)atol(command.GetOptionArgs("--block-index").front().c_str());
        InfoLog("%s\n", xpack::DumpUtils::DumpBlock(pack.GetContxt(), index).c_str());
        return true;
    }
    if (command.HasOption("--block-unused")) {
        InfoLog("%s\n", xpack::DumpUtils::DumpBlockUnused(pack.GetContxt()).c_str());
        return true;
    }
    if (command.HasOption("--block-unused-content")) {
        InfoLog("%s\n", xpack::DumpUtils::DumpBlockContentUnused(pack.GetContxt()).c_str());
        return true;
    }
    if (command.HasOption("--hash")) {
        InfoLog("%s\n", xpack::DumpUtils::DumpHashs(pack.GetContxt()).c_str());
        return true;
    }
    if (command.HasOption("--hash-id")) {
        uint32_t hashid = atoi(command.GetOptionArgs("--hash-id").front().c_str());
        InfoLog("%s\n", xpack::DumpUtils::DumpHash(pack.GetContxt(), hashid).c_str());
        return true;
    }
    if (command.HasOption("--hash-name")) {
        std::string name = command.GetOptionArgs("--hash-name").front();
        InfoLog("%s\n", xpack::DumpUtils::DumpHash(pack.GetContxt(), name).c_str());
        return true;
    }
    if (command.HasOption("--hash-conflict")) {
        std::string conflicts = xpack::DumpUtils::DumpAllHashIDMarkedConflict(pack.GetContxt());
        if (conflicts.length() <= 0) {
            conflicts = NO_CONFLICTS;
        }
        InfoLog("%s\n", conflicts.c_str());
        return true;
    }
    if (command.HasOption("--hash-chain-name")) {
        std::string name = command.GetOptionArgs("--hash-chain-name").front();
        std::string conflicts = xpack::DumpUtils::DumpHashIDChainByEntryName(pack.GetContxt(), name);
        if (conflicts.length() <= 0) {
            conflicts = NO_CONFLICTS;
        }
        InfoLog("%s\n", conflicts.c_str());
        return true;
    }
    if (command.HasOption("--hash-chain-all")) {
        std::string conflicts = xpack::DumpUtils::DumpAllHashIDChain(pack.GetContxt());
        if (conflicts.length() <= 0) {
            conflicts = NO_CONFLICTS;
        }
        InfoLog("%s\n", conflicts.c_str());
        return true;
    }

    return false;
}

// SubCommand: unpack

bool OnCommand_Unpack(torch::Commander &command, std::vector<std::string> args) {
    assert(args.size() == 2);
    if (!torch::FileSystem::IsFile(args[0])) {
        ErrorLog("%s\n", PACKAGE_NOT_EXISTS);
        return true;
    }
    bool force = command.HasOption("-f");
    
    xpack::Package pack;
    if (!pack.Open(args[0])) {
        ErrorLog("%s\n", xpack::GetLastErrorMessage());
        return false;
    }
    
    if (command.HasOption("-p")) {
        std::string password = command.GetOptionArgs("-p").front();
        if (password.length() > 0) {
            pack.SetSecretKey((unsigned char *)password.c_str(), (int)password.length());
        }
    }

    return xpack::PackageHelper::ExtractTo(pack, args[1], force, [](const std::string &name, bool status){
        PathStatusLog(name, status);
        if (!status) {
            ErrorLog("%s\n", xpack::GetLastErrorMessage());
        }
        return status;
    });

    return true;
}

// SubCommand: check

bool OnCommand_Check(torch::Commander &command, std::vector<std::string> args) {
    assert(args.size() == 1);
    if (!torch::FileSystem::IsFile(args[0])) {
        ErrorLog("%s\n", PACKAGE_NOT_EXISTS);
        return true;
    }
    xpack::Package pack;
    if (!pack.Open(args[0])) {
        ErrorLog("%s\n", xpack::GetLastErrorMessage());
        return true;
    }
    if (command.HasOption("-c")) {
        torch::Data content;
        bool ok = pack.ForeachEntryNames([&](const std::string &name) {
            if (!pack.GetEntryDataByName(name, content)) {
                ErrorLog("%s \n", xpack::GetLastErrorMessage());
                return false;
            }
            return true;
        });
        if (!ok) { // crc check failed
            return true;
        }
    }
    InfoLog("%s\n", PACKAGE_IS_VALID);
    return true;
}

// SubCommand: merge

bool OnCommand_Merge(torch::Commander &command, std::vector<std::string> args) {
    assert(args.size() == 2);
    if (!torch::FileSystem::IsFile(args[0])) {
        ErrorLog("%s\n", PACKAGE_NOT_EXISTS);
        return true;
    }
    if (!torch::FileSystem::IsFile(args[1])) {
        ErrorLog("%s\n", PACKAGE_NOT_EXISTS);
        return true;
    }

    bool force = command.HasOption("-f");
    xpack::PackageHelper::Merge(args[0], args[1], force, [](const std::string &name, bool status){
        PathStatusLog(name, status);
        if (!status) {
            ErrorLog("%s\n", xpack::GetLastErrorMessage());
        }
        return status;
    });
    return true;
}

// SubCommand: diff

bool OnCommand_Diff(torch::Commander &command, std::vector<std::string> args) {
    assert(args.size() == 2);
    std::string leftName = args[0];
    std::string rightName = args[1];

    if (!torch::FileSystem::IsFile(leftName)) {
        ErrorLog("%s\n", PACKAGE_NOT_EXISTS);
        return true;
    }
    if (!torch::FileSystem::IsFile(rightName)) {
        ErrorLog("%s\n", PACKAGE_NOT_EXISTS);
        return true;
    }
    
    xpack::Package leftPackage;
    if (!leftPackage.Open(leftName)) {
        ErrorLog("%s\n", xpack::GetLastErrorMessage());
        return true;
    }
    xpack::Package rightPackage;
    if (!rightPackage.Open(rightName)) {
        ErrorLog("%s\n", xpack::GetLastErrorMessage());
        return true;
    }

    std::vector<std::string> leftPackageNames = leftPackage.GetEntryNames();
    std::vector<std::string> rightPackageNames = rightPackage.GetEntryNames();
    std::sort(leftPackageNames.begin(), leftPackageNames.end());
    std::sort(rightPackageNames.begin(), rightPackageNames.end());
    std::vector<std::string> allPackageNames = torch::collection::SortedVectorUnion(leftPackageNames, rightPackageNames);
    
    int maxsz = 0;
    for (auto name : allPackageNames) {
        if (name.length() > maxsz) {
            maxsz = (int)name.length();
        }
    }
    
    const char *eqs = " = ";
    const char *neqs = " ~ ";
    std::string display;
    display = torch::String::LeftPad('['+leftName+']', maxsz, ' ') + " ? " + '['+rightName+']' + '\n';
    for (auto name : allPackageNames) {
        std::string leftLine, rightLine;
        bool lok = std::find(leftPackageNames.begin(), leftPackageNames.end(), name) != leftPackageNames.end();
        uint32_t lcrc = 0;
        if (lok) {
            leftLine += name;
            lcrc = leftPackage.GetContxt()->hash->QueryByName(name)->crc;
        }
        leftLine = torch::String::LeftPad(leftLine, maxsz, ' ');
        bool rok = std::find(rightPackageNames.begin(), rightPackageNames.end(), name) != rightPackageNames.end();
        uint32_t rcrc = 0;
        if (rok) {
            rightLine += name;
            rcrc = rightPackage.GetContxt()->hash->QueryByName(name)->crc;
        }
        display += (leftLine + ((lok==rok && lcrc==rcrc) ? eqs : neqs) + rightLine) + '\n';
    }
    InfoLog("%s\n", display.c_str());

    return true;
}

// SubCommand: optimize

bool OnCommand_Optimize(torch::Commander &command, std::vector<std::string> args) {
    assert(args.size() == 1);
    if (!torch::FileSystem::IsFile(args[0])) {
        ErrorLog("%s\n", PACKAGE_NOT_EXISTS);
        return true;
    }
    std::string tmp = torch::Path::GetTemp();
    bool ok = xpack::PackageHelper::MakeNew(tmp);
    if (!ok) {
        ErrorLog("%s\n", MAKE_TMP_PACKAGE_FAILED);
    }
    ok = xpack::PackageHelper::Merge(tmp, args[0], false, [](const std::string &name, bool status){
        PathStatusLog(name, status);
        if (!status) {
            ErrorLog("%s\n", xpack::GetLastErrorMessage());
        }
        return status;
    });
    if (!ok) {
        ErrorLog("%s\n", MERGE_TMP_PACKAGE_FAILED);
    }
    torch::FileSystem::Remove(args[0]);
    torch::FileSystem::Move(tmp, args[0]);
    return true;
}

// Benchmark

bool OnCommand_Benchmark(torch::Commander &command, std::vector<std::string> args) {
    assert(args.size() >= 1);
    bool needcrc = command.HasOption("-c");
    
    if (command.HasOption("-p")) {
        std::string path = command.GetOptionArgs("-p").front();
        if (!torch::FileSystem::IsFile(path)) {
            ErrorLog("%s\n", PACKAGE_NOT_EXISTS);
            return true;
        }
        
        xpack::Package pkg;
        pkg.SetNeedCrcVerify(needcrc);
        if (!pkg.Open(path)) {
            ErrorLog("%s\n", xpack::GetLastErrorMessage());
            return true;
        }
        
        timeval tv_start, tv_stop;
        gettimeofday(&tv_start, nullptr);
        int64_t start = tv_start.tv_sec * 1000 * 1000 + tv_start.tv_usec;

        for (auto name : args) {
            if (!pkg.IsEntryExist(name)) {
                ErrorLog("%s\n", NAME_NOT_EXISTS);
                return true;
            }
            torch::Data buffer = pkg.GetEntryDataByName(name);
        }
        
        gettimeofday(&tv_stop, nullptr);
        int64_t stop = tv_stop.tv_sec * 1000 * 1000 + tv_stop.tv_usec;
        BenchmarkLog(args.size(), stop - start);
        
        return true;
    }
    else {
        timeval tv_start, tv_stop;
        gettimeofday(&tv_start, nullptr);
        int64_t start = tv_start.tv_sec * 1000 * 1000 + tv_start.tv_usec;

        for (auto name : args) {
            torch::File fstream;
            bool ok = fstream.Open(name, "rb");
            if (!ok) {
                ErrorLog("%s\n", FILE_NOT_EXISTS);
                return true;
            }
            torch::Data buffer = fstream.GetBytes();
            fstream.Close();
        }
        
        gettimeofday(&tv_stop, nullptr);
        int64_t stop = tv_stop.tv_sec * 1000 * 1000 + tv_stop.tv_usec;
        BenchmarkLog(args.size(), stop - start);
        return true;
    }
}

#ifndef XPACK_TEST

void SignalInterrupt(int signum) {
    ErrorLog("%s\n", INTERRUPT_ERROR);
}

int main(int argc, const char * argv[]) {
    signal(SIGINT, SignalInterrupt);
    signal(SIGQUIT, SignalInterrupt);
    signal(SIGTERM, SignalInterrupt);
    signal(SIGKILL, SignalInterrupt);

    std::string desc = torch::String::Format("xpack command line client, version %s.", xpack::Package::GetVersion().c_str());
    torch::Arguments app(0, desc, OnCommand_Main);
    
    app.MainCommand()
    .Usage("usage: xpack <subcommand> [options] [args]")
    .Option("-h", 0, "show help document")
    .Option("-v", 0, "show version information");

    // Add
    app.SubCommand("add", 2, "add file to package.", OnCommand_Add)
    .Usage("usage: xpack add <package> <file> [options]")
    .Option("-h", 0, "show subcommand help document", torch::Arguments::CallbackFail)
    .Option("-f", 0, "adding force overwrite existing file")
    .Option("-n", 1, "name of entry store in package. ARG(name)")
    .Option("-p", 1, "Password to encrypt/decrypt file data. ARGC:1, password")
    .Option("-z", 0, "Compress file data with zip");
    
    // Multi Add
    app.SubCommand("madd", app.RequireArgsAtLeast(2), "add multi files to package.", OnCommand_MultiAdd)
    .Usage("usage: xpack madd <package> <file> [file ...] [options]")
    .Option("-h", 0, "show subcommand help document", torch::Arguments::CallbackFail)
    .Option("-f", 0, "adding force overwrite existing files")
    .Option("-p", 1, "Password to encrypt/decrypt file data. ARGC:1, password")
    .Option("-z", 0, "Compress file data with zip");
    
    // Remove
    app.SubCommand("rm", 2, "remove file from package(support 'wildcard').", OnCommand_Remove)
    .Usage("usage: xpack rm <package> <'wildcard'> [options]")
    .Option("-h", 0, "show subcommand help document", torch::Arguments::CallbackFail);

    // Cat
    app.SubCommand("cat", 2, "print entry content(support 'wildcard').", OnCommand_Cat)
    .Usage("usage: xpack cat <package> <'wildcard'> [options]")
    .Option("-h", 0, "show subcommand help document", torch::Arguments::CallbackFail)
    .Option("-p", 1, "Password to encrypt/decrypt file data. ARGC:1, password");

    // List
    app.SubCommand("ls", app.RequireArgsAtLeast(1), "list all entries(support 'wildcard').", OnCommand_List)
    .Usage("usage: xpack ls <package> ['wildcard'] [options]")
    .Option("-h", 0, "show subcommand help document", torch::Arguments::CallbackFail)
    .Option("-l", 0, "display entry long infomation")
    .Option("-s", 0, "display entry names sorted by file size");
    
    // Make
    app.SubCommand("make", 1, "make empty package.", OnCommand_Make)
    .Usage("usage: xpack make <package> [options]")
    .Option("-h", 0, "show subcommand help document", torch::Arguments::CallbackFail)
    .Option("-f", 0, "force overwrite an existing file");
    
    // Dump
    app.SubCommand("dump", 1, "dump package information.", OnCommand_Dump)
    .Usage("usage: xpack dump <package> [options]")
    .Option("-h", 0, "show subcommand help document", torch::Arguments::CallbackFail)
    .Option("--signature", 0, "dump signature section")
    .Option("--header", 0, "dump header section")
    .Option("--name", 0, "dump name section")
    .Option("--data", 0, "dump whole content data")
    .Option("--data-offsize", 2, "dump part of package data, ARG(offset, size)")
    .Option("--block", 0, "dump all blocks(no unused)")
    .Option("--block-name", 1, "dump block list with name, ARG(name)")
    .Option("--block-index", 1, "dump one block with block index, ARG(index)")
    .Option("--block-unused", 0, "dump all unused blocks")
    .Option("--block-unused-content", 0, "dump all unused content blocks(block's content is unused)")
    .Option("--hash", 0, "dump all hashs(no unused)")
    .Option("--hash-id", 1, "dump hash with hashid, ARG(hashid)")
    .Option("--hash-name", 1, "dump hash with name, ARG(name)")
    .Option("--hash-conflict", 0, "dump all conflict hashid list")
    .Option("--hash-chain-name", 1, "dump hashid chain with name, ARG(name)")
    .Option("--hash-chain-all", 0, "dump all hashid chains");

    // Unpack
    app.SubCommand("unpack", 2, "unpack all entries to target folder.", OnCommand_Unpack)
    .Usage("usage: xpack unpack <package> [pathto] [options]")
    .Option("-h", 0, "show subcommand help document", torch::Arguments::CallbackFail)
    .Option("-f", 0, "unpack force overwrite existing files")
    .Option("-p", 1, "Password to encrypt/decrypt file data. ARGC:1, password");

    // Check
    app.SubCommand("check", 1, "check package is valid.", OnCommand_Check)
    .Usage("usage: xpack check <package> [options]")
    .Option("-h", 0, "show subcommand help document", torch::Arguments::CallbackFail)
    .Option("-c", 0, "check crc32 of every entry");
    
    // Merge
    app.SubCommand("merge", 2, "merge other package into main package.", OnCommand_Merge)
    .Usage("usage: xpack merge <main-package> <other-package> [options]")
    .Option("-h", 0, "show subcommand help document", torch::Arguments::CallbackFail)
    .Option("-f", 0, "merge force overwrite existing files");

    // Optimize
    app.SubCommand("optimize", 1, "rebuild to optimize package.", OnCommand_Optimize)
    .Usage("usage: xpack optimize <package> [options]")
    .Option("-h", 0, "show subcommand help document", torch::Arguments::CallbackFail);
    
    // Diff
    app.SubCommand("diff", 2, "show two package differences.", OnCommand_Diff)
    .Usage("usage: xpack diff <main-package> <other-package> [options]")
    .Option("-h", 0, "show subcommand help document", torch::Arguments::CallbackFail);

    // Benchmark
    app.SubCommand("benchmark", app.RequireArgsAtLeast(1), "benchmark test.", OnCommand_Benchmark)
    .Usage("usage: xpack benchmark <filename> [filename ...] [options]")
    .Option("-p", 1, "xpack package path")
    .Option("-c", 0, "package read with crc32 check")
    .Option("-h", 0, "show subcommand help document", torch::Arguments::CallbackFail);

    bool ok = app.Parse(argc, argv);
    if (!ok) {
        return -1;
    }
    
    assert(torch::HeapCounter() == 0);
    return 0;
}

#endif // XPACK_TEST


