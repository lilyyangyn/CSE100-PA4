actor_graph_lib = library('actor_graph', sources: ['ActorGraph.hpp', 'ActorGraph.cpp'], dependencies: [disjoint_set_dep])
actor_graph_dep = declare_dependency(include_directories: include_directories('.'), 
    link_with: actor_graph_lib, 
    dependencies: [disjoint_set_dep])