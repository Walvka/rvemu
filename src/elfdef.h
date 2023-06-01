#include "types.h"

#define EI_NIDENT       16
#define ELFMAG          "\177ELF"

#define EM_RISCV        243

#define EI_CLASS        4
#define ELFCLASSNONE    0
#define ELFCLASS32      1
#define ELFCLASS64      2
#define ELFCLASSNUM     3

#define PT_LOAD         1

#define PF_X            0x01
#define PF_W            0x02
#define PF_R            0x04


#define R_X86_64_PC32 2

typedef struct _ELF64_EHDR_T_{
    /*e_ident
    包含了Maigc Number和其它信息，共16字节
    0~3：前4字节为Magic Number，固定为ELFMAG
    4（EI_CLASS）：ELFCLASS32代表是32位ELF，ELFCLASS64 代表64位ELF
    5（EI_DATA）：ELFDATA2LSB代表小端，ELFDATA2MSB代表大端
    6（EI_VERSION）：固定为EV_CURRENT（1）
    7（EI_OSABI）：操作系统ABI标识（实际未使用）
    8（EI_ABIVERSION）：ABI版本（实际 未使用）
    9~15：对齐填充，无实际意义*/
    u8      e_ident[EI_NIDENT];

    /*e_type
    ELF的文件类型，定义如下：
        ET_NONE	    0x0000	未知目标文件格式
        ET_ERL	    0x0001	可重定位文件（如目标文件）
        ET_EXEC	    0x0002	可执行文件（可直接执行的文件）
        ET_DYN	    0x0003	共享目标文件（如SO库）
        ET_CORE	    0x0004	Core文件(转储格式)
        ET_LOPROC	0xff00	特定处理器文件
        ET_HIPROC	0xffff	特定处理器文件
    注：GCC使用编译选项 -pie 编译的可执行文件实际 也是DT_DYN类型*/
    u16     e_type;

    /*e_machine
        EM_NONE	        0	    No machine
        EM_SPARC	    2	    SPARC
        EM_386	        3	    Intel 80386
        EM_MIPS	        8	    MIPS I Architecture
        EM_PPC	        0x14	PowerPC
        EM_ARM	        0x28	Advanced RISC Machines ARM
        EM_RISCV        0xf3*/ 
    u16     e_machine;

    /*e_version
    文件版本，目前常见的ELF 文件版本均为EV_CURRENT（1）
    EV_NONE	    0	非法版本
    EV_CURRENT	1	当前版本*/
    u32     e_version;

    /*e_entry
    入口虚拟地址*/
    u64     e_entry;

    /*e_phoff
    段表文件偏移
    该字段表示程序表头偏移
    若当前的偏移量为00 00 00 34
    也就是实际的偏移量为52个字节
    这52个字节其实就是头部的信息数据结构体的大小*/
    u64     e_phoff;

    /*e_shoff
    节表文件偏移*/
    u64     e_shoff;

    /*e_flags
    处理器特定的标志，一般为0*/
    u32     e_flags;

    /*e_ehsize
    Elf_Header的大小（字节）*/
    u16     e_ehsize;

    /*e_phentsize
    段头（Program Header）的大小（字节）*/
    u16     e_phentsize;

    /*e_phnum
    段的数量*/
    u16     e_phnum;

    /*e_shentsize
    节头（Section Header）的大小（字节）*/
    u16     e_shentsize;

    /*e_shnum
    字的数量*/
    u16     e_shnum;

    /*e_shstrndx
    节字符串表的节索引*/
    u16     e_shstrndx;
}elf64_ehdr_t;

typedef struct _ELF64_PHDR_T_{
    u32 p_type;//此段类型
    u32 p_flags;//此段相关标记
    u64 p_offset;//此段的偏移值(位置)
    u64 p_vaddr;//此段放在内存虚拟地址的位置(运行时)
    u64 p_paddr;//此段放在内存物理地址的位置(运行时)
    u64 p_filesize;//此段在映像中所占用的字节数
    u64 p_memsize;//此段在内存中所占用的字节数
    u64 p_align;//此成员给出段在文件中和内存中如何对齐
}elf64_phdr_t;

typedef struct _ELF64_SHDR_T_{
    u32 s_name;//节区名
    u32 s_type;//节区类型
    u32 s_flags;//节区标志位
    u32 s_addr;//节区虚拟地址
    u32 s_offset;//节区偏移值(位置)
    u32 s_size;//节区长度
    u32 s_link;//节区链接信息
    u32 s_info;//节区链接信息
    u32 s_addralign;//节区对齐
    u32 s_entsize;//项的大小
}elf64_shdr_t;

typedef struct {
	u32 st_name;
	u8  st_info;
	u8  st_other;
	u16 st_shndx;
	u64 st_value;
	u64 st_size;
} elf64_sym_t;

typedef struct {
    u64 r_offset;
    u32 r_type;
    u32 r_sym;
    i64 r_addend;
} elf64_rela_t;