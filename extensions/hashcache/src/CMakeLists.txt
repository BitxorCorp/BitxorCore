cmake_minimum_required(VERSION 3.14)

bitxorcore_define_extension_src(hashcache)

# add dependency on hash cache plugin
target_link_libraries(bitxorcore.hashcache bitxorcore.plugins.hashcache.cache)
