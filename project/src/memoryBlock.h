#pragma once

// memoryBlock.h
#include <stdio.h>

#include "util.h"

typedef struct MemoryBlock {
	char* data;          // Pointer to the memory block data
	size_t capacity;     // Total capacity of the memory block
	size_t cursor;        // Current position in the memory block
} MemoryBlock;


MemoryBlock* memory_block_init(size_t capacity);
void memory_block_destroy(MemoryBlock* block);
void memory_block_free(MemoryBlock* block);
void memory_block_clear(MemoryBlock* block);
bool memory_block_resize(MemoryBlock* block, size_t new_capacity);

//Active functions (functions that move the cursor)

bool memory_block_write(MemoryBlock* block, const void* data, size_t size);
bool memory_block_read(MemoryBlock* block, void* data, size_t size);
bool memory_block_skip(MemoryBlock* block, size_t size);

// Functions to move the cursor without writing or reading data
bool memory_block_seek_to(MemoryBlock* block, size_t offset);
bool memory_block_seek_to_start(MemoryBlock* block);
bool memory_block_seek_to_end(MemoryBlock* block);
bool memory_block_seek_relative(MemoryBlock* block, int64_t offset);

// Passive functions (functions that do not move the cursor)
bool memory_block_peek(const MemoryBlock* block, void* data, size_t size);
bool memory_block_peek_at(const MemoryBlock* block, size_t offset, void* data, size_t size);
bool memory_block_is_full(const MemoryBlock* block);
bool memory_block_is_valid(const MemoryBlock* block);
void memory_block_dump(const MemoryBlock* block, size_t size, FILE* stream);
bool memory_block_ensurespaceforwrite(MemoryBlock* block, size_t size);
