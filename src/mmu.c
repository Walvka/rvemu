#include "rvemu.h"

static void load_phdr(elf64_phdr_t *phdr, elf64_ehdr_t* ehdr, i64 i, FILE *file){
    /*
        int fseek(FILE *stream, long int offset, int whence)
            stream -- 这是指向 FILE 对象的指针，该 FILE 对象标识了流。
            offset -- 这是相对 whence 的偏移量，以字节为单位。
            whence -- 这是表示开始添加偏移 offset 的位置。它一般指定为下列常量之一：
                SEEK_SET	文件的开头
                SEEK_CUR	文件指针的当前位置
                SEEK_END	文件的末尾
    */
    if(fseek(file, ehdr->e_phoff + ehdr->e_phentsize * i, SEEK_SET) != 0){//从program header的位置开始，加几个header长度就是第几个header
        fatal("seek file failed");
    }
    /*
        size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
            ptr -- 这是指向带有最小尺寸 size*nmemb 字节的内存块的指针。
            size -- 这是要读取的每个元素的大小，以字节为单位。
            nmemb -- 这是元素的个数，每个元素的大小为 size 字节。
            stream -- 这是指向 FILE 对象的指针，该 FILE 对象指定了一个输入流。
    */
    if(fread((void*)phdr, 1, ehdr->e_phentsize, file) != sizeof(elf64_phdr_t)){
        fatal("file too small");
    }
}

static int flags_to_mmap_prot(u32 flags){
    return  (flags & PF_R ? PROT_READ : 0) |
            (flags & PF_W ? PROT_WRITE : 0) |
            (flags & PF_X ? PROT_EXEC : 0);
}

static void mmu_load_segment(mmu_t *mmu, elf64_phdr_t *phdr, int fd){
    /*
    头文件：#include <unistd.h>
    size_t getpagesize(void);
        返回一分页的大小，单位为字节(byte)。此为系统的分页大小，不一定会和硬件分页大小相同。返回内存分页大小。
    */
    int page_size = getpagesize();
    u64 offset = phdr->p_offset;
    u64 vaddr = TO_HOST(phdr->p_vaddr);
    u64 aligned_vaddr = ROUNDDOWN(vaddr, page_size);
    u64 filesize = phdr->p_filesize + (vaddr - aligned_vaddr);
    u64 memsize = phdr->p_memsize + (vaddr - aligned_vaddr);

    int prot = flags_to_mmap_prot(phdr->p_flags);
    u64 addr = (u64)mmap((void*)aligned_vaddr,filesize, prot, MAP_PRIVATE|MAP_FIXED, fd, ROUNDDOWN(offset, page_size));
    assert(addr == aligned_vaddr);

    u64 remaining_bss = ROUNDUP(memsize, page_size) - ROUNDUP(filesize, page_size);

    if(remaining_bss > 0){
        u64 addr = (u64)mmap((void*)(aligned_vaddr + ROUNDUP(filesize, page_size)), remaining_bss, prot, MAP_ANONYMOUS|MAP_PRIVATE|MAP_FIXED, -1, 0);
        assert(addr == aligned_vaddr + ROUNDUP(filesize, page_size));
    }

    mmu->host_alloc = MAX(mmu->host_alloc, (aligned_vaddr + ROUNDUP(memsize, page_size)));
    mmu->base = mmu->alloc = TO_GUEST(mmu->host_alloc);

}

void mmu_load_elf(mmu_t *mmu, int fd){
    u8 buf[sizeof(elf64_ehdr_t)];
    FILE *file = fdopen(fd, "rb");
    if(fread(buf, 1, sizeof(elf64_ehdr_t), file) != sizeof(elf64_ehdr_t)){
        fatal("file too small");
    }

    elf64_ehdr_t *ehdr = (elf64_ehdr_t *)buf;

    if(*(u32 *)ehdr != *(u32 *)ELFMAG){
        fatal("bad elf file");
    }

    if((EM_RISCV != ehdr->e_machine) || (ELFCLASS64 != ehdr->e_ident[EI_CLASS])) {
        fatal("only riscv64 elf file is supported");
    }

    mmu->entry = (u64)ehdr->e_entry;

    elf64_phdr_t phdr;
    for(i64 i=0; i<ehdr->e_phnum; i++){//遍历所有的program header
        load_phdr(&phdr, ehdr, i, file);

        if(phdr.p_type == PT_LOAD){
            mmu_load_segment(mmu, &phdr, fd);
        }
    }
}