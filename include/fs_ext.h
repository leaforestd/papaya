#ifndef FS_EXT__H
#define FS_EXT__H
#include<valType.h>
#include<fs.h>
//#define INODE_FILE_REGULAR 1
#define INODE_FILE_DIR 4
//inode和block中文件类型编码不一样
#define BLOCK_FILE_REGULAR 1
#define BLOCK_FILE_DIR 2
typedef struct{
	__le32 s_inodes_count; // 文件系统中inode的总数
	__le32 s_blocks_count; // 文件系统中块的总数
	__le32 s_r_blocks_count; // 保留块的总数
	__le32 s_free_blocks_count; // 未使用的块的总数（包括保留块）
	__le32 s_free_inodes_count; // 未使用的inode的总数
	__le32 s_first_data_block; // 块ID，在小于1KB的文件系统中为0，大于1KB的文件系统中为1
	__le32 s_log_block_size; // 用以计算块的大小（1024算术左移该值即为块大小）
	__le32 s_log_frag_size; // 用以计算段大小（为正则1024算术左移该值，否则右移）
	__le32 s_blocks_per_group; // 每个块组中块的总数
	__le32 s_frags_per_group; // 每个块组中段的总数
	__le32 s_inodes_per_group; // 每个块组中inode的总数
	__le32 s_mtime; // POSIX中定义的文件系统装载时间
	__le32 s_wtime; // POSIX中定义的文件系统最近被写入的时间
	__le16 s_mnt_count; // 最近一次完整校验后被装载的次数
	__le16 s_max_mnt_count; // 在进行完整校验前还能被装载的次数
	__le16 s_magic; // 文件系统标志，ext2中为0xEF53
	__le16 s_state; // 文件系统的状态
	__le16 s_errors; // 文件系统发生错误时驱动程式应该执行的操作
	__le16 s_minor_rev_level; // 局部修订级别
	__le32 s_lastcheck; // POSIX中定义的文件系统最近一次检查的时间
	__le32 s_checkinterval; // POSIX中定义的文件系统最近检查的最大时间间隔
	__le32 s_creator_os; // 生成该文件系统的操作系统
	__le32 s_rev_level; // 修订级别
	__le16 s_def_resuid; // 报留块的默认用户ID
	__le16 s_def_resgid; // 保留块的默认组ID
	// 仅用于使用动态inode大小的修订版（EXT2_DYNAMIC_REV）
	 __le32 s_first_ino; // 标准文件的第一个可用inode的索引（非动态为11）
	 __le16 s_inode_size; // inode结构的大小（非动态为128）
	 __le16 s_block_group_nr; // 保存此超级块的块组号
	 __le32 s_feature_compat; // 兼容特性掩码
	 __le32 s_feature_incompat; // 不兼容特性掩码
	 __le32 s_feature_ro_compat; // 只读特性掩码
	 __u8 s_uuid[16]  ; // 卷ID，应尽可能使每个文件系统的格式唯一
	 char s_volume_name[16]  ; // 卷名（只能为ISO-Latin-1字符集，以’＼0’结束）
	 char s_last_mounted[64]  ; // 最近被安装的目录
	 __le32 s_algorithm_usage_bitmap; // 文件系统采用的压缩算法
	// // 仅在EXT2_COMPAT_PREALLOC标志被设置时有效
	 __u8 s_prealloc_blocks; // 预分配的块数
	 __u8 s_prealloc_dir_blocks; // 给目录预分配的块数
	 __u16 s_padding1;
	 // 仅在EXT3_FEATURE_COMPAT_HAS_JOURNAL标志被设置时有效，用以支持日志
	 __u8 s_journal_uuid[16]  ; // 日志超级块的卷ID
	 __u32 s_journal_inum; // 日志文件的inode数目
	 __u32 s_journal_dev; // 日志文件的设备数
	 __u32 s_last_orphan; // 要删除的inode列表的起始位置
	 __u32 s_hash_seed[4]  ; // HTREE散列种子
	 __u8 s_def_hash_version; // 默认使用的散列函数
	 __u8 s_reserved_char_pad;
	 __u16 s_reserved_word_pad;
	 __le32 s_default_mount_opts;
	 __le32 s_first_meta_bg; // 块组的第一个元块
	 __u32 s_reserved[190]  ;
}SUPER_BLOCK;
/**
static int memberwidth_SUPER_BLOCK[]={4,4,4,4,4,4,4,4,4,4,4,4,4,2,2,2,2,2,2,4,4,4,4,2,2,4,2,2,4,4,4,1,1,1,4,1,1,2,1,4,4,4,4,1,1,2,4,4,4};
static char* membername_SUPER_BLOCK[]={"s_inodes_count","s_blocks_count","s_r_blocks_count","s_free_blocks_count","s_free_inodes_count","s_first_data_block","s_log_block_size","s_log_frag_size","s_blocks_per_group","s_frags_per_group","s_inodes_per_group","s_mtime","s_wtime","s_mnt_count","s_max_mnt_count","s_magic","s_state","s_errors","s_minor_rev_level","s_lastcheck","s_checkinterval","s_creator_os","s_rev_level","s_def_resuid","s_def_resgid","s_first_ino","s_inode_size","s_block_group_nr","s_feature_compat","s_feature_incompat","s_feature_ro_compat","s_uuid[16]","s_volume_name[16]","s_last_mounted[64]","s_algorithm_usage_bitmap","s_prealloc_blocks","s_prealloc_dir_blocks","s_padding1","s_journal_uuid[16]","s_journal_inum","s_journal_dev","s_last_orphan","s_hash_seed[4]","s_def_hash_version","s_reserved_char_pad","s_reserved_word_pad","s_default_mount_opts","s_first_meta_bg","s_reserved[190]"};
static int memberlen_SUPER_BLOCK[]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,16,16,64,1,1,1,1,16,1,1,1,4,1,1,1,1,1,190};
*/
typedef struct{
	__le32 bg_block_bitmap; // 块位图所在的第一个块的块ID
	__le32 bg_inode_bitmap; // inode位图所在的第一个块的块ID
	__le32 bg_inode_table; // inode表所在的第一个块的块ID
	__le16 bg_free_blocks_count; // 块组中未使用的块数
	__le16 bg_free_inodes_count; // 块组中未使用的inode数
	__le16 bg_used_dirs_count; // 块组分配的目录的inode数
	__le16 bg_pad;
	__le32 bg_reserved[3];
}GROUP_DESC;
/*8
static int memberwidth_GROUP_DESC[]={4,4,4,2,2,2,2,4};
static char* membername_GROUP_DESC[]={"bg_block_bitmap","bg_inode_bitmap","bg_inode_table","bg_free_blocks_count","bg_free_inodes_count","bg_used_dirs_count","bg_pad","bg_reserved[3]"};
static int memberlen_GROUP_DESC[]={1,1,1,1,1,1,1,3};
*/
typedef struct{
	__le16 i_mode; // 文件格式和访问权限
	__le16 i_uid; // 文件所有者ID的低16位
	__le32 i_size; // 文件字节数
	__le32 i_atime; // 文件上次被访问的时间
	__le32 i_ctime; // 文件创建时间
	__le32 i_mtime; // 文件被修改的时间
	__le32 i_dtime; // 文件被删除的时间（如果存在则为0）
	__le16 i_gid; // 文件所有组ID的低16位
	__le16 i_links_count; // 此inode被连接的次数
	int block_count;
	__le32 i_flags; // 此inode访问数据时ext2的实现方式
	int os_information;
	int blocks[12];
	int seedblock1;
	int seedblock2;
	int seedblock3;
	int padden1[128/4-25];//ERR for right struct
}INODE;

typedef struct{
	int inode;
	u16 record_len;
	u8 name_len;
	u8 file_type;
	char name[128];
}DIRENT;

void fs_ext(void);
#endif
