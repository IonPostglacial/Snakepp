project('snakepp', 'cpp', 
    default_options : ['cpp_std=c++14'])

configure_file(input : 'src/index.html', output : 'index.html', copy : true)

executable('snake', 'src/snake.cpp',
    cpp_args : [],
    name_suffix : 'js',
    link_args : [
        '-Wl,--allow-undefined',
    ])