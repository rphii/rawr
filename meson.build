project('rawr', 'c', meson_version: '>=1.1', version: '0.0.0')

sources = [
  'rawr/main.c',
  ]

cc = meson.get_compiler('c')

m_dep = cc.find_library('m', required : false)
rlc_dep = dependency('rlc', fallback : ['rlc', 'rlc_dep'], default_options: ['default_library=static'])
rlso_dep = dependency('rlso', fallback : ['rlso', 'rlso_dep'], default_options: ['default_library=static'])
rlarg_dep = dependency('rlarg', fallback : ['rlarg', 'rlarg_dep'], default_options: ['default_library=static'])
# this will probably not work...
stb_dep = dependency('stb', fallback : ['stb', 'stb_dep'], default_options: ['default_library=static'])

rawr = executable('rawr',
  sources,
  dependencies: [m_dep, rlc_dep, rlso_dep, rlarg_dep, stb_dep],
  install: true,
  )

