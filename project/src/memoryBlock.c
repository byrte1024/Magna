// memoryBlock.c

#include "memoryBlock.h"
#include <stdlib.h>
#include <string.h>

static bool memory_block_lazy_init_data(MemoryBlock* block) {
	if (!block) return false;
	if (block->capacity == 0) block->capacity = 16;
	if (!block->data) {
		block->data = (char*)malloc(block->capacity);
		if (!block->data) return false;
		block->cursor = 0;
	}
	return block->data != NULL;
}

MemoryBlock* memory_block_init(size_t capacity) {
	MemoryBlock* block = (MemoryBlock*)malloc(sizeof(MemoryBlock));
	if (!block) return NULL;

	block->data = (char*)malloc(capacity);
	if (!block->data) {
		free(block);
		return NULL;
	}

	block->capacity = capacity;
	block->cursor = 0;
	return block;
}

void memory_block_free(MemoryBlock* block) {
	if (!block) return;
	free(block->data);
	block->data = NULL;
}

void memory_block_destroy(MemoryBlock* block) {
	if (!block) return;
	free(block->data);
	free(block);
}

void memory_block_clear(MemoryBlock* block) {
	if (!memory_block_lazy_init_data(block)) return;
	memset(block->data, 0, block->capacity);
	block->cursor = 0;
}

bool memory_block_resize(MemoryBlock* block, size_t new_capacity) {
	if (!block || new_capacity == 0) return false;

	if (!block->data) {
		block->data = (char*)malloc(new_capacity);
		if (!block->data) return false;
		block->cursor = 0;
		block->capacity = new_capacity;
		return true;
	}

	char* new_data = (char*)realloc(block->data, new_capacity);
	if (!new_data) return false;

	//Clear the new space if the new capacity is larger
	if (new_capacity > block->capacity) {
		memset(new_data + block->capacity, 0, new_capacity - block->capacity);
	}

	block->data = new_data;
	block->capacity = new_capacity;

	if (block->cursor > new_capacity) {
		block->cursor = new_capacity;
	}

	return true;
}

bool memory_block_ensurespaceforwrite(MemoryBlock* block, size_t size) {
	if (!memory_block_lazy_init_data(block) || !size) return false;

	if (block->cursor + size > block->capacity) {
		size_t new_capacity = block->capacity * 2;
		while (new_capacity < block->cursor + size) {
			new_capacity *= 2;
		}
		return memory_block_resize(block, new_capacity);
	}

	return true;
}

bool memory_block_write(MemoryBlock* block, const void* data, size_t size) {
	if (!memory_block_lazy_init_data(block) || !data || block->cursor + size > block->capacity) return false;
	memcpy(block->data + block->cursor, data, size);
	block->cursor += size;
	return true;
}

bool memory_block_read(MemoryBlock* block, void* data, size_t size) {
	if (!memory_block_lazy_init_data(block) || !data || block->cursor + size > block->capacity) return false;
	memcpy(data, block->data + block->cursor, size);
	block->cursor += size;
	return true;
}

bool memory_block_skip(MemoryBlock* block, size_t size) {
	if (!memory_block_lazy_init_data(block) || block->cursor + size > block->capacity) return false;
	block->cursor += size;
	return true;
}

bool memory_block_seek_to(MemoryBlock* block, size_t offset) {
	if (!memory_block_lazy_init_data(block) || offset > block->capacity) return false;
	block->cursor = offset;
	return true;
}

bool memory_block_seek_to_start(MemoryBlock* block) {
	if (!memory_block_lazy_init_data(block)) return false;
	block->cursor = 0;
	return true;
}

bool memory_block_seek_to_end(MemoryBlock* block) {
	if (!memory_block_lazy_init_data(block)) return false;
	block->cursor = block->capacity;
	return true;
}

bool memory_block_seek_relative(MemoryBlock* block, int64_t offset) {
	if (!memory_block_lazy_init_data(block)) return false;
	if (offset < 0 && (size_t)(-offset) > block->cursor) return false;
	if (offset > 0 && block->cursor + offset > block->capacity) return false;
	block->cursor += offset;
	return true;
}

bool memory_block_peek(const MemoryBlock* block, void* data, size_t size) {
	if (!block || !block->data || !data || block->cursor + size > block->capacity) return false;
	memcpy(data, block->data + block->cursor, size);
	return true;
}

bool memory_block_peek_at(const MemoryBlock* block, size_t offset, void* data, size_t size) {
	if (!block || !block->data || !data || offset + size > block->capacity) return false;
	memcpy(data, block->data + offset, size);
	return true;
}

bool memory_block_is_full(const MemoryBlock* block) {
	if (!block || !block->data) return false;
	return block->cursor >= block->capacity;
}

bool memory_block_is_valid(const MemoryBlock* block) {
	return block != NULL && block->capacity > 0;
}

void memory_block_dump(const MemoryBlock* block, size_t size, FILE* stream) {
	if (!block || !block->data || !stream || size == 0) return;

	size_t bytes_to_dump = (size > block->capacity) ? block->capacity : size;

	//format data as hex
	for (size_t i = 0; i < bytes_to_dump; i++) {
		fprintf(stream, "%02X ", (unsigned char)block->data[i]);
	}	

	fprintf(stream, "\n");
}

