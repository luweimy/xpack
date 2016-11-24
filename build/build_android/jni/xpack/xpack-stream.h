//
//  xpack
//
//  Created by Luwei.
//  Copyright (c) 2016 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy
//

#ifndef __XPACK__STREAM__
#define __XPACK__STREAM__

#include <stdio.h>
#include <string>
#include "torch/torch.h"
#include "xpack-def.h"

namespace xpack {
    
    class Stream {
    public:
        Stream();
        virtual ~Stream();
        
        virtual bool Open(const std::string &path, bool readonly = true) = 0;
        virtual bool IsEnd() = 0;
        virtual bool Flush() = 0;
        
        virtual size_t Size() = 0;
        virtual bool ReSize(size_t size) = 0;

        virtual bool GetContent(void *buffer, size_t size, size_t offset) = 0;
        virtual bool PutContent(void *buffer, size_t size, size_t offset) = 0;
        
        virtual bool GetContent(torch::Data &content, size_t offset);
        
        virtual bool GetSignature(MetaSignature *buffer, size_t offset);
        virtual bool PutSignature(MetaSignature *buffer, size_t offset);
        virtual bool GetHeader(MetaHeader *buffer, size_t offset);
        virtual bool PutHeader(MetaHeader *buffer, size_t offset);
        
        virtual bool GetHashs(void *buffer, size_t offset, size_t count);
        virtual bool PutHashs(void *buffer, size_t offset, size_t count);
        virtual bool GetBlocks(void *buffer, size_t offset, size_t count);
        virtual bool PutBlocks(void *buffer, size_t offset, size_t count);

    };
    
    class FileStream : public Stream {
    public:
        FileStream();
        ~FileStream();
        
        bool Open(const std::string &path, bool readonly = true);

        bool IsEnd();
        bool Flush();
        
        size_t Size();
        bool ReSize(size_t size);
        
        bool GetContent(void *buffer, size_t size, size_t offset);
        bool PutContent(void *buffer, size_t size, size_t offset);
        
    private:
        torch::File m_fstream;
    };
    
}

#endif /* __XPACK__STREAM__ */
