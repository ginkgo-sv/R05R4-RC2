/*
 * ext2_err.c:
 * This file is automatically generated; please do not edit it.
 */

static const char * const text[] = {
		"EXT2FS Library version 1.25",
		"Wrong magic number for ext2_filsys structure",
		"Wrong magic number for badblocks_list structure",
		"Wrong magic number for badblocks_iterate structure",
		"Wrong magic number for inode_scan structure",
		"Wrong magic number for io_channel structure",
		"Wrong magic number for unix io_channel structure",
		"Wrong magic number for io_manager structure",
		"Wrong magic number for block_bitmap structure",
		"Wrong magic number for inode_bitmap structure",
		"Wrong magic number for generic_bitmap structure",
		"Wrong magic number for test io_channel structure",
		"Wrong magic number for directory block list structure",
		"Wrong magic number for icount structure",
		"Wrong magic number for Powerquest io_channel structure",
		"Wrong magic number for ext2 file structure",
		"Wrong magic number for Ext2 Image Header",
		"Wrong magic number --- RESERVED_8",
		"Wrong magic number --- RESERVED_9",
		"Bad magic number in super-block",
		"Filesystem revision too high",
		"Attempt to write to filesystem opened read-only",
		"Can't read group descriptors",
		"Can't write group descriptors",
		"Corrupt group descriptor: bad block for block bitmap",
		"Corrupt group descriptor: bad block for inode bitmap",
		"Corrupt group descriptor: bad block for inode table",
		"Can't write an inode bitmap",
		"Can't read an inode bitmap",
		"Can't write an block bitmap",
		"Can't read an block bitmap",
		"Can't write an inode table",
		"Can't read an inode table",
		"Can't read next inode",
		"Filesystem has unexpected block size",
		"EXT2 directory corrupted",
		"Attempt to read block from filesystem resulted in short read",
		"Attempt to write block from filesystem resulted in short write",
		"No free space in the directory",
		"Inode bitmap not loaded",
		"Block bitmap not loaded",
		"Illegal inode number",
		"Illegal block number",
		"Internal error in ext2fs_expand_dir",
		"Not enough space to build proposed filesystem",
		"Illegal block number passed to ext2fs_mark_block_bitmap",
		"Illegal block number passed to ext2fs_unmark_block_bitmap",
		"Illegal block number passed to ext2fs_test_block_bitmap",
		"Illegal inode number passed to ext2fs_mark_inode_bitmap",
		"Illegal inode number passed to ext2fs_unmark_inode_bitmap",
		"Illegal inode number passed to ext2fs_test_inode_bitmap",
		"Attempt to fudge end of block bitmap past the real end",
		"Attempt to fudge end of inode bitmap past the real end",
		"Illegal indirect block found" ,
		"Illegal doubly indirect block found" ,
		"Illegal triply indirect block found" ,
		"Block bitmaps are not the same",
		"Inode bitmaps are not the same",
		"Illegal or malformed device name",
		"A block group is missing an inode table",
		"The ext2 superblock is corrupt",
		"Illegal generic bit number passed to ext2fs_mark_generic_bitmap",
		"Illegal generic bit number passed to ext2fs_unmark_generic_bitmap",
		"Illegal generic bit number passed to ext2fs_test_generic_bitmap",
		"Too many symbolic links encountered.",
		"The callback function will not handle this case",
		"The inode is from a bad block in the inode table",
		"Filesystem has unsupported feature(s)",
		"Filesystem has unsupported read-only feature(s)",
		"IO Channel failed to seek on read or write",
		"Memory allocation failed",
		"Invalid argument passed to ext2 library",
		"Could not allocate block in ext2 filesystem",
		"Could not allocate inode in ext2 filesystem",
		"Ext2 inode is not a directory",
		"Too many references in table",
		"File not found by ext2_lookup",
		"File open read-only",
		"Ext2 directory block not found",
		"Ext2 directory already exists",
		"Unimplemented ext2 library function",
		"User cancel requested",
		"Ext2 file too big",
		"Supplied journal device not a block device",
		"Journal superblock not found",
		"Journal must be at least 1024 blocks",
		"Unsupported journal version",
		"Error loading external journal",
    0
};

struct error_table {
    char const * const * msgs;
    long base;
    int n_msgs;
};
struct et_list {
    struct et_list *next;
    const struct error_table * table;
};
extern struct et_list *_et_list;

static const struct error_table et = { text, 2133571328L, 88 };

static struct et_list link = { 0, 0 };

void initialize_ext2_error_table(void);

void initialize_ext2_error_table(void) {
    if (!link.table) {
        link.next = _et_list;
        link.table = &et;
        _et_list = &link;
    }
}
