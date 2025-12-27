/*
 * file_utils.h - File Handling Utilities
 *
 * This header provides utility functions for file operations
 * including size calculation, path validation, and safe file I/O.
 */

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stddef.h>
#include <sys/types.h>

/* ============================================================================
 * Constants
 * ============================================================================ */

/* File serving root directory */
#define FILE_ROOT           "./test_files"

/* Maximum file size to serve (10 MB) */
#define MAX_FILE_SIZE       (10 * 1024 * 1024)

/* ============================================================================
 * Path Functions
 * ============================================================================ */

/*
 * validate_path - Check if a path is safe to serve
 *
 * @param path: Requested path (e.g., "/small.txt")
 * @return: 1 if path is safe, 0 otherwise
 *
 * Security checks:
 * - Path must not be NULL or empty
 * - Path must start with '/'
 * - Path must not contain ".." (directory traversal attack)
 * - Path must not contain null bytes
 *
 * CRITICAL: This prevents attackers from accessing files outside FILE_ROOT
 */
int validate_path(const char *path);

/*
 * build_full_path - Combine root directory and requested path
 *
 * @param buffer: Output buffer for full path
 * @param buflen: Size of buffer
 * @param root: Root directory (e.g., "./test_files")
 * @param path: Requested path (e.g., "/small.txt")
 * @return: 0 on success, -1 on error
 *
 * Example: root="./test_files", path="/small.txt"
 *          result="./test_files/small.txt"
 *
 * Must check for buffer overflow!
 */
int build_full_path(char *buffer, size_t buflen, const char *root, const char *path);

/* ============================================================================
 * File Information Functions
 * ============================================================================ */

/*
 * get_file_size - Get the size of a file
 *
 * @param filepath: Path to file
 * @return: File size in bytes, or -1 on error
 *
 * Use stat() or fstat() to get file size.
 * Don't open the file unnecessarily.
 */
ssize_t get_file_size(const char *filepath);

/*
 * file_exists - Check if a file exists and is readable
 *
 * @param filepath: Path to file
 * @return: 1 if file exists and is readable, 0 otherwise
 *
 * Use access() with R_OK flag.
 */
int file_exists(const char *filepath);

/*
 * is_regular_file - Check if path is a regular file (not directory)
 *
 * @param filepath: Path to check
 * @return: 1 if regular file, 0 otherwise
 *
 * Use stat() and check S_ISREG().
 */
int is_regular_file(const char *filepath);

/* ============================================================================
 * File I/O Functions
 * ============================================================================ */

/*
 * read_file_to_buffer - Read entire file into memory
 *
 * @param filepath: Path to file
 * @param buffer: Pointer to buffer pointer (will be allocated)
 * @param size: Output parameter for file size
 * @return: 0 on success, -1 on error
 *
 * This function:
 * 1. Gets file size
 * 2. Allocates buffer (caller must free!)
 * 3. Reads entire file into buffer
 *
 * WARNING: Only use for small files! Large files should be streamed.
 */
int read_file_to_buffer(const char *filepath, char **buffer, size_t *size);

/*
 * write_buffer_to_file - Write buffer to file
 *
 * @param filepath: Path to output file
 * @param buffer: Data to write
 * @param size: Number of bytes to write
 * @return: 0 on success, -1 on error
 *
 * Creates file if it doesn't exist, truncates if it does.
 */
int write_buffer_to_file(const char *filepath, const char *buffer, size_t size);

/* ============================================================================
 * Cache Support Functions (for Part C)
 * ============================================================================ */

/*
 * compute_file_hash - Compute a simple hash of file path for cache lookup
 *
 * @param path: File path
 * @return: Hash value
 *
 * This is used to index into the cache hash table.
 * A simple hash like djb2 is sufficient.
 */
unsigned long compute_file_hash(const char *path);

/*
 * generate_cache_key - Generate cache key from path
 *
 * @param buffer: Output buffer for cache key
 * @param buflen: Size of buffer
 * @param path: File path
 * @return: 0 on success, -1 on error
 *
 * The cache key should be a normalized version of the path.
 */
int generate_cache_key(char *buffer, size_t buflen, const char *path);

#endif /* FILE_UTILS_H */
