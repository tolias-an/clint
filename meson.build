project('clint', 'c',
  default_options: [
    'buildtype=debugoptimized',
    'c_std=c17',
    'warning_level=3'
  ]
)

json_c_dep = dependency('json-c')
unity_dep = dependency('unity', required : false)

sources = [
  'src' / 'main.c',
  'src' / 'util.c',
]

include_dir = include_directories('include')

executable('clint',
  sources : sources,
  include_directories: include_dir,
  dependencies : json_c_dep,
  install : true,
)

if unity_dep.found()
  tests = {
    'util' : [ 'test' / 'util.c', 'src' / 'util.c' ],
  }

  foreach name, sources : tests
    _test = executable(name,
      sources: sources + sources,
      include_directories: include_dir,
      dependencies : [ json_c_dep, unity_dep ],
    )

    test(name, _test)
  endforeach
endif
