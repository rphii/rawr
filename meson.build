project('rawr', 'c', meson_version: '>=1.1', version: '0.0.0')
add_project_arguments('-DVERSION="'+meson.project_version()+'"', language : 'c')

sources = [
  'rawr/main.c',
  ]

cc = meson.get_compiler('c')

m_dep = cc.find_library('m', required : false)
rlc_dep = dependency('rlc', fallback : ['rlc', 'rlc_dep'], default_options: ['default_library=static'])
rlso_dep = dependency('rlso', fallback : ['rlso', 'rlso_dep'], default_options: ['default_library=static'])
rlarg_dep = dependency('rlarg', fallback : ['rlarg', 'rlarg_dep'], default_options: ['default_library=static'])

rawr = executable('rawr',
  sources,
  dependencies: [m_dep, rlc_dep, rlso_dep, rlarg_dep],
  install: true,
  )

install_data('bash/rawr', install_dir: get_option('completion_dir'))

