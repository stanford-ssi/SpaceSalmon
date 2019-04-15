/* Auto-generated config file conf_fatfs.h */
        #ifndef CONF_FATFS_H
        #define CONF_FATFS_H

        // <<< Use Configuration Wizard in Context Menu >>>

        // <h> Function Configurations
/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module configuration file  R0.11a (C)ChaN, 2015
/---------------------------------------------------------------------------*/

#define _FFCONF 64180	/* Revision ID */

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

// <q> Read Only File System Enable
// <i> This option switches read-only configuration. Read-only configuration removes writing API functions, f_write(), f_sync(), f_unlink(), f_mkdir(), f_chmod(), f_rename(), f_truncate(), f_getfree() and optional writing functions as 	well.
// <id> read_only_fs_enable
#ifndef _FS_READONLY
#   define _FS_READONLY 0x0
#endif

// <o> Minimisation Level (_FS_MINIMIZE)
// <i> The _FS_MINIMIZE option defines minimization level to remove some functions.
	// <0x0=> 0 - Full function
	// <0x1=> 1 - f_stat, f_getfree, f_unlink, f_mkdir, f_chmod, f_truncate and f_rename are removed
	// <0x2=> 2 - f_opendir and f_readdir are removed in addition to option 1
	// <0x3=> 3 - f_lseek is removed in addition to option 2
// <id> minimisation_level
#ifndef _FS_MINIMIZE
#   define _FS_MINIMIZE 0x0
#endif

// <q> String Functions Enable
// <i> To enable string functions.
// <id> strfunc_fs_enable
#ifndef _USE_STRFUNC
#   define _USE_STRFUNC 0x1
#endif

// <q> f_mkfs Function Enable
// <i> This option switches f_mkfs() function.
// <id> f_mkfs_fs_enable
#ifndef _USE_MKFS
#   define _USE_MKFS 0x1
#endif

// <q> f_forward() Function Enable
// <i> This option switches f_forward() function. 
// <id> use_forward_fs_enable
#ifndef _USE_FORWARD
#   define _USE_FORWARD 0x0
#endif

// <q> Fast Seek feature Enable
// <i> This option switches fast seek feature.
// <id> fast_seek_fs_enable
#ifndef _USE_FASTSEEK
#   define _USE_FASTSEEK 0x0
#endif

// <q> Filtered Directory read feature Enable
// <i> This option switches filtered directory read feature and related functions, f_findfirst() and f_findnext(). 
// <id> filtered_dir_enable
#ifndef _USE_FIND
#   define _USE_FIND 0x0
#endif

// <q> Volume Label Function Enable
// <i> This option switches volume label functions, f_getlabel() and f_setlabel().
// <id> volume_label_enable
#ifndef _USE_LABEL
#   define _USE_LABEL 0x0
#endif

// </h>

// <h> Drive/Volume Configurations
/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/----------------------------------------------------------------------------*/

// <o> Number of Volumes <1-10>
// <i> Number of volumes (logical drives) to be used. (1 to 10)
// <id> num_volumes
#ifndef _VOLUMES
#   define _VOLUMES 5
#endif

// <o> Minimum Sector Size 
// <i> Minimum sector size to be handled. Always set 512 for memory card and hard disk but a larger value may be required for on-board flash memory, floppy disk and optical disk.
	// <512=> 512
	// <1024=> 1024
	// <2048=> 2048
	// <4096=> 4096
// <id> min_sector_size
#ifndef _MIN_SS
#   define _MIN_SS 512
#endif

// <o> Maximum Sector Size 
// <i> Maximum sector size to be handled. Always set 512 for memory card and hard disk but a larger value may be required for on-board flash memory, floppy disk and optical disk.
	// <512=> 512
	// <1024=> 1024
	// <2048=> 2048
	// <4096=> 4096
// <id> max_sector_size
#ifndef _MAX_SS
#   define _MAX_SS 512
#endif

// <q> String Volume ID feature Enable
// <i>  When this feature is enabled, pre-defined strings can be used as drive number in the path name. _VOLUME_STRS defines the drive ID strings for each logical drives. Number of items must be equal to number of volumes. Valid characters for the drive ID strings are: A-Z and 0-9.
// <id> string_volume_enable
#ifndef _STR_VOLUME_ID
#   define _STR_VOLUME_ID 0
#endif

#define _VOLUME_STRS	"RAM","NAND","CF","SD1","SD2","USB1","USB2","USB3"

// <q> ATA-TRIM feature Enable
// <i>  This option switches ATA-TRIM feature.
// <id> trim_feature_enable
#ifndef _USE_TRIM
#   define _USE_TRIM 0
#endif

// <q> Free Space Info feature Enable
// <i>  If you need to know correct free space on the FAT32 volume disable the feature so that f_getfree() function at first time after volume mount will force a full FAT scan. When enabled, controls the use of last allocated cluster number.
// <id> free_space_info_enable
#ifndef _FS_NOFSINFO
#   define _FS_NOFSINFO 0
#endif

// <q> Multiple Partition Enable
// <i>  When this is disabled, each volume is bound to the same physical drive number and it can mount only first primary partition and when enabled each volume is tied to the partitions listed in VolToPart[].
// <id> multiple_partition_enable
#ifndef _MULTI_PARTITION
#   define _MULTI_PARTITION 0
#endif

// <q> Sector Erase Feature Enable
// <i> This enables sector erase feature
// <id> sectore_erase_enable
#ifndef _USE_ERASE
#   define _USE_ERASE 0
#endif

// </h>

// <h> Locale and Namespace Configurations
/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/----------------------------------------------------------------------------*/

// <o> OEM Code Page
// <i> The _CODE_PAGE specifies the OEM code page to be used on the target system. Incorrect setting of the code page can cause a file open failure.
	// <1=> ASCII only (Valid for non LFN cfg.)
	// <437=> U.S. (OEM)
	// <720=> Arabic (OEM)
	// <737=> Greek (OEM)
	// <775=> Baltic (OEM)
	// <850=> Multilingual Latin 1 (OEM)
	// <852=> Latin 2 (OEM)
	// <855=> Cyrillic (OEM)
	// <857=> Turkish (OEM)
	// <858=> Multilingual Latin 1 + Euro (OEM)
	// <862=> Hebrew (OEM))
	// <866=> Russian (OEM)
	// <874=> Thai (OEM, Windows)
	// <932=> Japanese Shift-JIS (DBCS, OEM, Windows) 
	// <936=> Simplified Chinese GBK (DBCS, OEM, Windows)
	// <949=> Korean (DBCS, OEM, Windows)
	// <950=> Traditional Chinese Big5 (DBCS, OEM, Windows)
	// <1250=> Central Europe (Windows)
	// <1251=> Cyrillic (Windows)
	// <1252=> Latin 1 (Windows)
	// <1253=> Greek (Windows)
	// <1254=> Turkish (Windows)
	// <1255=> Hebrew (Windows)
	// <1256=> Arabic (Windows)
	// <1257=> Baltic (Windows)
	// <1258=> Vietnam (OEM, Windows)
// <id> code_page
#ifndef _CODE_PAGE
#   define _CODE_PAGE 850
#endif

// <o> LFN Feature
// <i> The LFN working buffer occupies (_MAX_LFN + 1) * 2 bytes. To enable LFN, Unicode handling functions ff_convert() and ff_wtoupper() must be added to the project. When enable to use heap, memory control functions ff_memalloc() and ff_memfree() must be added to the project.
	// <0x0=> 0 - Disable LFN feature. _MAX_LFN and _LFN_UNICODE have no effect.
	// <0x1=> 1 - Enable LFN with static working buffer on the BSS. Always NOT reentrant.
	// <0x2=> 2 - Enable LFN with dynamic working buffer on the STACK.
	// <0x3=> 3 - Enable LFN with dynamic working buffer on the HEAP.
// <id> lfn_feature
#ifndef _USE_LFN
#   define _USE_LFN 0
#endif

// <o> Maximum LFN length to handle <12-255>
// <i> Maximum LFN length to handle (12 to 255)
// <id> max_lfn_length
#ifndef _MAX_LFN
#   define _MAX_LFN 255
#endif

// <o> Character Code Set
// <i>  To switch the character code set on FatFs API to Unicode, enable LFN feature.
	// <0x0=> 0 - ANSI/OEM
	// <0x1=> 1 - Unicode
// <id> character_code_set
#ifndef _LFN_UNICODE
#   define _LFN_UNICODE 0
#endif

// <o> Character Encoding
// <i> When character code is selected as Unicode, this option selects the character encoding on the file to be read/written via string I/O functions, f_gets(), f_putc(), f_puts and f_printf().
	// <0x0=> ANSI/OEM
	// <0x1=> UTF-16LE
	// <0x2=> UTF-16BE
	// <0x3=> UTF-8
// <id> character_encoding
#ifndef _STRF_ENCODE
#   define _STRF_ENCODE 3
#endif

// <o> Relative Path Feature
// <i> The relative path feature is configured here.
	// <0x0=> 0 - Disable relative path feature and remove related functions.
	// <0x1=> 1 - Enable relative path. f_chdrive() and f_chdir() are available.
	// <0x2=> 2 - f_getcwd() is available in addition to 1.
// <id> relative_path_enable 
#ifndef _FS_RPATH
#   define _FS_RPATH 2
#endif
// </h>

// <h> System Configurations
/*---------------------------------------------------------------------------/
/ System Configurations
/----------------------------------------------------------------------------*/

// <q> Time Stamp Feature Enable
// <i> This option switches timestamp feature. If the system does not have an RTC function or valid timestamp is not needed, disable this feature.
// <id> time_stamp_enable
#ifndef _FS_NORTC
#   define _FS_NORTC 0
#endif

#define _NORTC_MON	1
#define _NORTC_MDAY	1
#define _NORTC_YEAR	2015

// <o> File Lock Feature <0-100>
// <i> This option switches file lock feature to control duplicated file open and illegal operation to open objects. This option must not be enabled (value to be 0) if it is a read only filesystem. To avoid volume corruption, application program should avoid illegal open, remove and rename to the open objects. If value is greater tha 0, it defines how many files/sub-directories can be opened simultaneously under file lock control. Note that the file lock feature is independent of re-entrancy.
// <id> file_lock_enable
#ifndef _FS_NORTC
#   define _FS_NORTC 0
#endif

// <q> Tiny File System Enable
// <i> When this feature is enabled FatFs uses the sector buffer in the file system object instead of the sector buffer in the individual file object for file data transfer. This reduces memory consumption 512 bytes each file object.
// <id> tiny_fs_enable
#ifndef _FS_TINY
#   define _FS_TINY 0
#endif

// <q> Word Access Enable
// <i> This option defines which access method is used to the word data on the FAT volume. When the byte order on the memory is big-endian or address miss-aligned word access results incorrect behavior, this option should be disabled. If it is not the case, feature can be enabled to improve the performance and code size.
// <id> word_access_enable 
#ifndef _WORD_ACCESS
#   define _WORD_ACCESS 0
#endif

// <q> Reentrancy (Thread Safe) Enable
// <i> Enabling the feature adds synchronization handlers, ff_req_grant, ff_rel_grant, ff_del_syncobj and ff_cre_syncobj functions.
// <id> reentrant_enable 
#ifndef _FS_REENTRANT
#   define _FS_REENTRANT 0
#endif

// <o> File System Timeout <1-100000>
// <i> Timeout period in unit of time ticks (1 - 100000)
// <id> config_fs_timeout
#ifndef _FS_TIMEOUT
#   define _FS_TIMEOUT 1000
#endif

// <s> Synch Handler Name
// <i> This string will be sent to console
// <id> synch_handler
#ifndef _SYNC_t
#   define _SYNC_t "HANDLE"
#endif

// <o> File Sharing Feature <0-100>
// <i> To enable file sharing feature, set the value to 1 or greater. The value defines how many files can be opened simultaneously.
// <id> file_share_count
#ifndef _FS_SHARE
#   define _FS_SHARE 0
#endif

// </h>

        // <<< end of configuration section >>>

        #endif // CONF_FATFS_H
        
