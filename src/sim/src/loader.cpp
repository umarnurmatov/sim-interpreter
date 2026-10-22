#include "loader.hpp"
#include "elfio/elf_types.hpp"

int Loader::load_elf(BaseExecutor *exec, std::string filename)
{
  ELFIO::elfio reader;

  if (!reader.load(filename)) {
    std::cerr << "Loader: could not load file " << filename << '\n';
    return 1;
  }

  if (reader.get_class() != ELFIO::ELFCLASS32) {
    std::cerr << "Loader: elf class must be ELFCLASS32\n";
    return 1;
  }
  
  if (reader.get_encoding() != ELFIO::ELFDATA2LSB) {
    std::cerr << "Loader: elf encoding must be little-endian\n";
    return 1;
  }

  std::size_t seg_cnt = reader.segments.size();
  for (std::size_t i = 0; i < seg_cnt; ++i) {
    const ELFIO::segment* seg = reader.segments[i];
    seg->get_flags();

    if (seg->get_type() | ELFIO::PT_LOAD) {
      exec->load_bytes(seg->get_physical_address(), 
                       seg->get_data(), 
                       seg->get_memory_size());
    }
  }

  return 0;
}
