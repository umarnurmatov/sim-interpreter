# Used Rice library example project for referece 
# https://github.com/ruby-rice/BitmapPlusPlus-ruby/blob/main/ext/CMakeLists.txt

list(PREPEND CMAKE_MODULE_PATH "${rice_SOURCE_DIR}")

find_package(Ruby REQUIRED)

add_library(elfio_ext 
  MODULE 
    elfio_ext.cpp
)

target_compile_features(elfio_ext 
  PRIVATE 
    cxx_std_20
)

target_link_libraries(elfio_ext 
  PRIVATE 
    elfio::elfio
    Ruby::Module
    Rice::Rice
)

get_target_property(RUBY_EXT_SUFFIX Ruby::Ruby INTERFACE_RUBY_EXTENSION_SUFFIX)

set_target_properties(elfio_ext PROPERTIES 
  PREFIX ""
  SUFFIX "${RUBY_EXT_SUFFIX}"
)
