project(
  'synops', 'c',
  version : '0.1',
  license : 'BSD-3-Clause',
  default_options: [
    # Development settings
    'c_std=c17',
    'warning_level=3',
    'debug=true',
    'optimization=3',
    'b_lto=true',
  ],
  meson_version : '>= 0.55.1',
)

sqlite3_dep = dependency(
  'sqlite3',
  required: true,
)

libgit2_dep = dependency(
  'libgit2',
  required: true,
)

xxhash_dep = dependency(
  'libxxhash',
  required: true,
)

# Synops library
synops_include = include_directories('./include')
synops_lib = library(
  'synops', 'src/synops.c',
  sources : [ 'src/synops_files.c' ],
  dependencies : [ sqlite3_dep, libgit2_dep, xxhash_dep ],
  include_directories : synops_include,
  install : true,
)

# Additional install targets
install_headers('./include/synops.h')
import('pkgconfig').generate(
  synops_lib,
  description: 'Backend for knowledge management',
)

# Synops dependency
synops_dep = declare_dependency(
  link_with : synops_lib,
  include_directories : synops_include,
)
meson.override_dependency('synops', synops_dep)

# Default executable
synops_exe = executable(
  'synops', 'src/main.c',
  sources : [ ],
  dependencies : [ synops_dep ],
  install : true,
)
