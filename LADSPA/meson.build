
faust_ladspa_gen = generator(faust,
    output: '@BASENAME@_dsp.cpp',
    arguments: ['-i', '-a', '@SOURCE_DIR@' / 'LADSPA' / 'ladspa-faustarch.cpp',
      '-o', '@OUTPUT@', '@INPUT@'])

foreach suffix, opts : plugins
  name = 'kpp_@0@'.format(suffix)
  if opts.get('ladspa', false)
    shared_library(name,
      faust_ladspa_gen.process(name / '@0@.dsp'.format(name)),
      name_prefix: '',
      cpp_args: ['-Dmydsp=@0@'.format(name)],
      install: true,
      install_dir : get_option('ladspadir'))
  endif
endforeach
