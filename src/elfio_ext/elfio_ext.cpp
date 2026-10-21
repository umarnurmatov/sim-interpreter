#include <cstdint>
#include <stdexcept>
#include <string>

#include <elfio/elfio.hpp>
#include <rice/rice.hpp>
#include <rice/stl.hpp>

#define DEFINE_RW_ACCESSOR(name, type)            \
   define_method("get_" #name, &type::get_##name) \
  .define_method("set_" #name, &type::set_##name)

extern "C" void Init_elfio_ext()
{
  auto module = Rice::define_module("Elfio");

  /* --------------------------- Section ------------------------------- */

  Rice::define_class_under<ELFIO::section>(module, "Section")
    .define_method("get_index",   &ELFIO::section::get_index)
    .define_method("get_offset",  &ELFIO::section::get_offset)
    .define_method("set_data",    static_cast<void (ELFIO::section::*)(const std::string&)>(&ELFIO::section::set_data))
    .define_method("append_data", static_cast<void (ELFIO::section::*)(const std::string&)>(&ELFIO::section::append_data))
    .DEFINE_RW_ACCESSOR(name,       ELFIO::section)
    .DEFINE_RW_ACCESSOR(type,       ELFIO::section)
    .DEFINE_RW_ACCESSOR(flags,      ELFIO::section)
    .DEFINE_RW_ACCESSOR(info,       ELFIO::section)
    .DEFINE_RW_ACCESSOR(link,       ELFIO::section)
    .DEFINE_RW_ACCESSOR(addr_align, ELFIO::section)
    .DEFINE_RW_ACCESSOR(entry_size, ELFIO::section)
    .DEFINE_RW_ACCESSOR(address,    ELFIO::section)
    .DEFINE_RW_ACCESSOR(size,       ELFIO::section);

  /* --------------------------- Segment ------------------------------- */

  Rice::define_class_under<ELFIO::segment>(module, "Segment")
    .define_method("get_index",            &ELFIO::segment::get_index)
    .define_method("get_offset",           &ELFIO::segment::get_offset)
    .define_method("add_section_index",    &ELFIO::segment::add_section_index)
    .define_method("get_sections_num",     &ELFIO::segment::get_sections_num)
    .define_method("get_section_index_at", &ELFIO::segment::get_section_index_at)
    .DEFINE_RW_ACCESSOR(type,             ELFIO::segment)
    .DEFINE_RW_ACCESSOR(flags,            ELFIO::segment)
    .DEFINE_RW_ACCESSOR(align,            ELFIO::segment)
    .DEFINE_RW_ACCESSOR(virtual_address,  ELFIO::segment)
    .DEFINE_RW_ACCESSOR(physical_address, ELFIO::segment)
    .DEFINE_RW_ACCESSOR(file_size,        ELFIO::segment)
    .DEFINE_RW_ACCESSOR(memory_size,      ELFIO::segment);

  /* --------------------------- ElfioSections ------------------------------- */
  // internal section repr

  Rice::define_class_under<ELFIO::elfio::Sections>(module, "ElfioSections")
    .define_method("add",  &ELFIO::elfio::Sections::add, Rice::Return().keepAlive())
    .define_method("size", &ELFIO::elfio::Sections::size);

  /* --------------------------- ElfioSegments ------------------------------- */
  // internal segment repr

  Rice::define_class_under<ELFIO::elfio::Segments>(module, "ElfioSegments")
    .define_method("add",  &ELFIO::elfio::Segments::add, Rice::Return().keepAlive())
    .define_method("size", &ELFIO::elfio::Segments::size);

  /* --------------------------- WRITER ------------------------------- */

  Rice::define_class_under<ELFIO::elfio>(module, "Writer")
    .define_constructor(Rice::Constructor<ELFIO::elfio>())
    .define_method     ("create",       &ELFIO::elfio::create)
    .define_method     ("get_class",    &ELFIO::elfio::get_class)
    .define_method     ("get_encoding", &ELFIO::elfio::get_encoding)

    .DEFINE_RW_ACCESSOR(os_abi,      ELFIO::elfio)
    .DEFINE_RW_ACCESSOR(abi_version, ELFIO::elfio)
    .DEFINE_RW_ACCESSOR(type,        ELFIO::elfio)
    .DEFINE_RW_ACCESSOR(machine,     ELFIO::elfio)
    .DEFINE_RW_ACCESSOR(flags,       ELFIO::elfio)
    .DEFINE_RW_ACCESSOR(entry,       ELFIO::elfio)

    .define_method("sections", [](ELFIO::elfio& writer) -> ELFIO::elfio::Sections& { return writer.sections; }, 
                   Rice::Return().keepAlive())

    .define_method("segments", [](ELFIO::elfio& writer) -> ELFIO::elfio::Segments& { return writer.segments; }, 
                   Rice::Return().keepAlive())

    .define_method("save", [](ELFIO::elfio& writer, const std::string& filename) {
      if (!writer.save(filename))
        throw std::runtime_error("Cannot save ELF file: " + filename);
    });


  /* --------------------------- constants ------------------------------- */

#define DEFINE_CONSTANT(name) module.define_constant(#name, static_cast<unsigned int>(ELFIO::name))
  DEFINE_CONSTANT(ELFCLASS32);
  DEFINE_CONSTANT(ELFCLASS64);
  DEFINE_CONSTANT(ELFDATA2LSB);
  DEFINE_CONSTANT(ELFDATA2MSB);
  DEFINE_CONSTANT(ELFOSABI_NONE);
  DEFINE_CONSTANT(ET_REL);
  DEFINE_CONSTANT(ET_EXEC);
  DEFINE_CONSTANT(ET_DYN);
  DEFINE_CONSTANT(EM_NONE);
  DEFINE_CONSTANT(SHT_PROGBITS);
  DEFINE_CONSTANT(SHT_NOBITS);
  DEFINE_CONSTANT(SHT_STRTAB);
  DEFINE_CONSTANT(SHT_SYMTAB);
  DEFINE_CONSTANT(SHF_ALLOC);
  DEFINE_CONSTANT(SHF_WRITE);
  DEFINE_CONSTANT(SHF_EXECINSTR);
  DEFINE_CONSTANT(PT_LOAD);
  DEFINE_CONSTANT(PF_R);
  DEFINE_CONSTANT(PF_W);
  DEFINE_CONSTANT(PF_X);
}

#undef DEFINE_CONSTANT
#undef DEFINE_ACCESSORS
