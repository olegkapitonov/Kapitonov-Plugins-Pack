project('kpp', 'cpp', 'c', license: 'GPL-3.0-or-later', version: '1.2',
  default_options : ['buildtype=release'])

cc = meson.get_compiler('cpp')
m_dep = cc.find_library('m', required : false)
cxx = meson.get_compiler('cpp')

boost = cxx.has_header('boost/circular_buffer.hpp', required: true)
xcb = dependency('xcb', required: get_option('gui'))
xcb_aux = dependency('xcb-aux', required: get_option('gui'))
xcb_icccm = dependency('xcb-icccm', required: get_option('gui'))
cairo = dependency('cairo', required: get_option('gui'))

faust = find_program('faust', required: true)
faustarch = run_command(faust, '-archdir', check: true).stdout().strip()

ladspa = cxx.has_header('ladspa.h', required: get_option('ladspa'))
lv2 = dependency('lv2', required: get_option('lv2'))

if not lv2.found() and not ladspa
  error('must enable either ladspa or lv2')
endif

plugins = {
  'bluedream': {
    'gui': true,
    'ladspa': true
  }, 'deadgate': {
    'ladspa': true
  }, 'distruction': {
    'gui': true,
    'ladspa': true
  }, 'fuzz': {
    'gui': true,
    'ladspa': true
  }, 'octaver': {
    'ladspa': true
  }, 'single2humbucker': {
    'ladspa': true
  }, 'tubeamp': {
    'gui': true,
    'lv2src': ['kpp_tubeamp.cpp'],
    'libs' : ['fftw3f', 'pthread']
  },
}

add_project_arguments(
          '-DFAUST_META=1',
          '-DFAUST_MIDICC=1',
          '-DFAUST_MTS=1',
          '-DVOICE_CTRLS=1', language: ['c', 'cpp'])

if lv2.found()
  subdir('LV2')
endif

if ladspa
  subdir('LADSPA')
endif
