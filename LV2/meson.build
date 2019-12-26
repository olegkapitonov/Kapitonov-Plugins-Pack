
faust_lv2_gen = generator(faust,
    output: '@BASENAME@_dsp.cpp',
    arguments: ['-i', '-a', faustarch / 'lv2.cpp', '-cn', '@BASENAME@',
      '-o', '@OUTPUT@', '@INPUT@'])

gen_args = cxx.get_supported_arguments(
      ['-Wno-sign-compare', '-Wno-unused-variable', '-Wno-unused-function'])

foreach suffix, opts : plugins
  name = 'kpp_@0@'.format(suffix)
  deps = []
  foreach lib : opts.get('libs', [])
    deps += cxx.find_library(lib, required: true)
  endforeach

  bundle = '@0@.lv2'.format(name)
  uri = 'https://faustlv2.bitbucket.io/@0@'.format(name)

  install_subdir(name / bundle, install_dir: get_option('lv2dir'))

  srcs = []
  foreach src : opts.get('lv2src', [])
    srcs += name / src
  endforeach

  objs = []
  dsp_target = '@0@_dsp'.format(name)
  dsp = name / '@0@.dsp'.format(name)
  if srcs.length() > 0
    srcs += custom_target(dsp_target,
        input: dsp,
        output: '@BASENAME@_dsp.h',
        command: [faust, '-o', '@OUTPUT@', '@INPUT@'])
  else
    objs += static_library(dsp_target,
      faust_lv2_gen.process(dsp),
      cpp_args: gen_args).extract_all_objects()
  endif

  shared_library(name,
    srcs,
    cpp_args: ['-DPLUGIN_URI="@0@"'.format(uri), '-DFAUST_UI=0'],
    name_prefix: '',
    dependencies: [lv2] + deps,
    objects: objs,
    install: true,
    install_dir : get_option('lv2dir') / bundle)

  if xcb.found() and cairo.found() and opts.get('gui', false)
    shared_library('@0@ui'.format(name),
      name / '@0@_ui.src/@0@_gui.c'.format(name),
      c_args: ['-DPLUGIN_URI="@0@"'.format(uri), '-DFAUST_UI=1'],
      name_prefix: '',
      dependencies: [m_dep, xcb, xcb_aux, xcb_icccm, cairo],
      install: true,
      install_dir : get_option('lv2dir') / bundle)
  endif
endforeach
