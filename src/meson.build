subdir('ActorGraph')
subdir('CityGraph')

help_util_dep = declare_dependency(include_directories: include_directories('.'),
    dependencies: [actor_graph_dep, city_graph_dep])

pathfinder_exe = executable('pathfinder.cpp.executable',
    sources: ['pathfinder.cpp'],
    dependencies: [cxxopts_dep, actor_graph_dep, help_util_dep])

linkpredictor_exe = executable('linkpredictor.cpp.executable',
    sources: ['linkpredictor.cpp'],
    dependencies: [cxxopts_dep, actor_graph_dep, help_util_dep])

movietraveler_exe = executable('movietraveler.cpp.executable',
    sources: ['movietraveler.cpp'],
    dependencies: [cxxopts_dep, actor_graph_dep])

navigationsystem_exe = executable('navigationsystem.cpp.executable',
sources: ['navigationsystem.cpp'],
dependencies: [cxxopts_dep, city_graph_dep, help_util_dep])