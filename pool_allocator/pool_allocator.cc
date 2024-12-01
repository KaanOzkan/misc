#include <iostream>
#include <ostream>

// From: http://dmitrysoshnikov.com/compilers/writing-a-pool-allocator/

struct Chunk {
  // When a chunk is free, the `next` contains the address of the next chunk in a list.
  // When it's allocated, this space is used by the user
  Chunk *next;
};

class PoolAllocator {
public:
  PoolAllocator(size_t chunksPerBlock) : ChunksPerBlock(chunksPerBlock) {}
  void *allocate(size_t size);
  void deallocate(void *ptr, size_t size);

private:
  // Number of chunks per larger block
  size_t ChunksPerBlock;
  // Allocation pointer
  Chunk *Alloc = nullptr;
  // Allocates a larger block (pool) for chunks
  Chunk *allocateBlock(size_t chunkSize);
};

// Returns the first free chunk in the block. If there are no chunks left in the block,
// allocates a new block
void *PoolAllocator::allocate(size_t size) {
  // No chunks left in the current block or no blocks exist. Allocate new
  if (Alloc == nullptr) {
    Alloc = allocateBlock(size);
  }

  // Return value is the current position of the allocation pointer
  Chunk *freeChunk = Alloc;

  // Advance the allocation pointer to the next chunk
  // When no chunks left, `Alloc` will be set to `nullptr` and next request will cause an allocation
  Alloc = Alloc->next;

  return freeChunk;
}

// Allocates a new block from OS
// Returns a Chunk pointer set to the beginning of the block
Chunk *PoolAllocator::allocateBlock(size_t chunkSize) {
  std::cout << "\nAllocating block (" << ChunksPerBlock << "chunks):\n\n";
  size_t blockSize = ChunksPerBlock * chunkSize;

  // First chunk of new block
  Chunk *blockBegin = reinterpret_cast<Chunk *>(malloc(blockSize));

  // Chain all the chunks in the block
  Chunk *chunk = blockBegin;
  for (int i = 0; i < ChunksPerBlock; ++i) {
    chunk->next = reinterpret_cast<Chunk *>(reinterpret_cast<char *>(chunk) + chunkSize);
    chunk = chunk->next;
  } 
  chunk->next = nullptr; 

  return blockBegin;
}

// Puts the chunk into the front of the chunks list
void PoolAllocator::deallocate(void *chunk, size_t size) {
  // Freed chunk's next pointer points to the current allocation pointer
  reinterpret_cast<Chunk *>(chunk)->next = Alloc;

  // Set the allocation pointer to the returned (free) chunk
  Alloc = reinterpret_cast<Chunk *>(chunk);
}

// Client usage example

// Overload `new` and `delete` to use custom allocator in any object
struct Object {
  // 2 64 bit elements, 16 bytes
  uint64_t data[2]; 
  
  static PoolAllocator allocator;

  static void* operator new(size_t size) {
    return allocator.allocate(size);
  }
  
  static void operator delete(void* ptr, size_t size) {
    return allocator.deallocate(ptr, size);
  }
};

// Instatiate using 8 chunks per block
PoolAllocator Object::allocator{8};

using std::cout;
using std::endl;

int main(int argc, char const * argv[]) {
  // Allocate 10 pointers to `Object` instance:
  constexpr int arraySize = 10;
  Object *objects[arraySize];
  // 16 bytes
  cout << "size(Object) = " << sizeof(Object) << endl << endl;

  // Allocate 10 objects, which causes two larger block allocations since we have 8 chunks per block
  cout << "About to allocate " << arraySize << " objects" << endl;
  for (int i = 0; i < arraySize; ++i) {
    objects[i] = new Object();
    cout << "new [" << i << "] = " << objects[i] << endl;
  }
  cout << endl;

  // Deallocate all objects
  for (int i = arraySize - 1; i >= 0; --i) {
    cout << "delete [" << i << "] = " << objects[i] << endl;
    delete objects[i];
  }
  cout << endl;

  // New object reuses previous block
  objects[0] = new Object();
  cout << "new [0] = " << objects[0] << endl <<  endl;
}
